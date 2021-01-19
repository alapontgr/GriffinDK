////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2021/01/13
//	File: 	GFDEBUG.cpp
//
//	Copyright (c) 2021 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfMesh.h"
#include "Common/GfCore/GfCore.h"
#include "Common/GfFile/GfFile.h"
#include "Common/GfMemory/GfRAII.h"

#include "Common/GfRender/GfRender.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#pragma warning(disable:4996)
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#include "tinygltf/tiny_gltf.h"
#pragma warning(default:4996)

////////////////////////////////////////////////////////////////////////////////

static GfString s_VertexPosKey = "POSITION";
static GfString s_VertexNormalKey = "NORMAL";
static GfString s_VertexUvKey = "TEXCOORD_0";
static GfString s_VertexTangentKey = "TANGENT";

////////////////////////////////////////////////////////////////////////////////

static GfUniquePtr<char[]> LoadFileBin(const char* szFilepath, u32& uiOutFileSize) 
{
	GfFileHandle kFile;
	GfFile::OpenFile(szFilepath, EFileAccessMode::Read, kFile);
	GfFile::GetFileSize(kFile);
	uiOutFileSize = static_cast<u32>(kFile.GetFileSize());
	if (uiOutFileSize > 0) 
	{
		GfUniquePtr<char[]> pSrc(new char[uiOutFileSize]);
		u32 uiRead = GfFile::ReadBytes(kFile, uiOutFileSize, pSrc.get());
		GfFile::CloseFile(kFile);
		GF_ASSERT(uiRead == uiOutFileSize, "Invalid size read");
		return pSrc;
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////

static bool doesFileExist(const std::string &abs_filename, void *) 
{
	return GfFile::DoesFileExist(abs_filename.c_str());
}

static GfString expandFilePath(const std::string & relPath, void *) 
{
	return GfPaths::getAssetPath(relPath);
}

static bool readWholeFile(std::vector<unsigned char>* outBuffer, std::string* error, const std::string & filePath, void *)
{
	GfFileHandle kFile;
	GfFile::OpenFile(filePath.c_str(), EFileAccessMode::Read, kFile);
	GfFile::GetFileSize(kFile);
	u32 fileSize = static_cast<u32>(kFile.GetFileSize());
	if (fileSize > 0) 
	{
		outBuffer->resize(fileSize);
		u32 uiRead = GfFile::ReadBytes(kFile, fileSize, outBuffer->data());
		GfFile::CloseFile(kFile);
		if (uiRead == fileSize) 
		{
			return true;
		}
		if (error) 
		{
			*error = "Failed to read file: " + filePath;
		}
	}
	return false;
}

static bool writeWholeFile(std::string * error, const std::string & filePath, 
	const std::vector<unsigned char> & data, void * userData)
{
	if (data.size() > 0) 
	{
		GfFileHandle kFile;
		GfFile::OpenFile(filePath.c_str(), EFileAccessMode::Write, kFile);
		u32 fileSize = static_cast<u32>(kFile.GetFileSize());
		u32 uiWriten = GfFile::WriteBytes(kFile, u32(data.size()), reinterpret_cast<const void*>(data.data()));
		GfFile::CloseFile(kFile);
		if (uiWriten == u32(data.size())) 
		{
			return true;
		}
		if (error) 
		{
			*error = "Failed to write to file: " + filePath;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////

GfSubMesh::GfSubMesh() 
	: m_parent(nullptr)
	, m_vertexCount(0)
	, m_indexCount(0)
	, m_indexUShort(false)
	, m_indexBufferOffset(0)
	, m_vertexBuffersOffsets{0,0}
	, m_indexData(nullptr)
	, m_posVertexData(nullptr)
	, m_normUvTangentVertexData(nullptr)
{}
GfSubMesh::~GfSubMesh() {}

////////////////////////////////////////////////////////////////////////////////

GfMesh::GfMesh() 
	: m_name("<NO NAME>")
	, m_vertexBuffer(-1)
	, m_indexBuffer(-1)
{}
GfMesh::~GfMesh()
{
	destroy();
}

void GfMesh::create(const GfRenderContext& ctx)
{
	GF_ASSERT(m_vertexBuffer == -1 && m_indexBuffer == -1, "Already initialized");
	m_indexBuffer = GfBufferFactory::create();
	m_vertexBuffer = GfBufferFactory::create();
	GfBufferFactory::addRef(m_indexBuffer);
	GfBufferFactory::addRef(m_vertexBuffer);

	// Calculate sizes and offsets
	u32 indexBufferSize = 0;
	u32 vertexBufferSize = 0;
	const u32 vertexSizePos = static_cast<u32>(sizeof(PosVertexBuffer));
	const u32 vertexSizeNormUvTangent = static_cast<u32>(sizeof(NormalsTangentUvVertexBuffer));
	for (const GfUniquePtr<GfSubMesh>& mesh : m_subMeshes) 
	{
		mesh->m_vertexBuffersOffsets[u32(VertexBufferType::Positions)] = vertexBufferSize;
		vertexBufferSize += vertexSizePos * mesh->getVertexCount();
		mesh->m_vertexBuffersOffsets[u32(VertexBufferType::NormalTangentUv)] = vertexBufferSize;
		vertexBufferSize += vertexSizeNormUvTangent * mesh->getVertexCount();

		mesh->m_indexBufferOffset = indexBufferSize;
		u32 indexSize = mesh->doesUseShortForIndex() ? static_cast<u32>(sizeof(u16)) : static_cast<u32>(sizeof(u32));
		indexBufferSize += mesh->getIndexCount() * indexSize;
	}

	GfBuffer::GfBufferDesc indexBufferDesc;

}

void GfMesh::destroy()
{
	m_name = "<NO NAME>";
	m_subMeshes.clear();
	GfBufferFactory::removeRef(m_indexBuffer);
	GfBufferFactory::removeRef(m_vertexBuffer);
	m_vertexBuffer = -1;
	m_indexBuffer = -1;
}

////////////////////////////////////////////////////////////////////////////////

GfUniquePtr<GfMesh> GfGltfLoader::Load(const GfString& filePath) 
{
	GfString path = filePath;
	std::replace( path.begin(), path.end(), '\\', '/');
	size_t pivot = path.find_last_of('/');
	GfString prefixPath = "./";
	if (pivot != GfString::npos) 
	{
		prefixPath = path.substr(0, pivot);
	}

	// Load GLTF
	u32 fileSize = 0;
	GfUniquePtr<char[]> gltfFileData = LoadFileBin(GfPaths::getAssetPath(filePath).data(), fileSize);
	GF_VERIFY(fileSize > 0, "File could not be opened.");

	GfUniquePtr<tinygltf::Model> model = std::make_unique<tinygltf::Model>();
	GfString error, warnings;
	tinygltf::TinyGLTF loader;
	tinygltf::FsCallbacks callbacks{};
	callbacks.ExpandFilePath = &expandFilePath;
	callbacks.ReadWholeFile = &readWholeFile;
	callbacks.WriteWholeFile = &writeWholeFile;
	callbacks.FileExists = &doesFileExist;
	loader.SetFsCallbacks(callbacks);
	if (loader.LoadASCIIFromString(model.get(), &error, &warnings, gltfFileData.get(), fileSize, prefixPath)) 
	{
		if (warnings.size() > 0) 
		{
			GF_WARNING("Loading Damaged Helmet triggered the following warnings:\n%s", warnings.data());
		}


		GF_VERIFY(model->meshes.size() == 1, "Just one model per gltf supported atm.");

		GfUniquePtr<GfMesh> engineModel = std::make_unique<GfMesh>();
		const tinygltf::Mesh& mesh = model->meshes[0];
		engineModel->m_name = mesh.name;
	
		for (const tinygltf::Primitive& prim : mesh.primitives) 
		{
			GF_VERIFY((prim.mode == TINYGLTF_MODE_TRIANGLES), "Just supporting triangle based meshes.");
			
			GfUniquePtr<GfSubMesh> subMesh = std::make_unique<GfSubMesh>();
			subMesh->m_parent = engineModel.get();

			s32 indexIdx = prim.indices;
			s32 posIdx = -1;
			s32 normalIdx = -1;
			s32 tangentIdx = -1;
			s32 uvIdx = -1;

			// Find accesors for attributes
			for (GfMap<GfString, s32>::const_iterator it = prim.attributes.begin(); it != prim.attributes.end(); it++) 
			{
				if (it->first == s_VertexPosKey) 
				{
					posIdx = it->second;
				}
				else if (it->first == s_VertexUvKey) 
				{
					uvIdx = it->second;
				}
				else if (it->first == s_VertexNormalKey) 
				{
					normalIdx = it->second;
				}
				else if (it->first == s_VertexTangentKey) 
				{
					tangentIdx = it->second;
				}
			}

			// Get Index information
			GF_VERIFY(indexIdx >= 0, "No valid accessor for indices");
			{
				const tinygltf::Accessor& indexBufferAccessor = model->accessors[indexIdx];
				const tinygltf::BufferView& view = model->bufferViews[indexBufferAccessor.bufferView];
				
				GF_VERIFY(indexBufferAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT ||
					indexBufferAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT, "Invalid type for index buffer elements");

				subMesh->m_indexUShort = (indexBufferAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT);
				subMesh->m_indexCount = static_cast<u32>(indexBufferAccessor.count);
				u32 elementSize = subMesh->m_indexUShort ? u32(sizeof(u16)) : u32(sizeof(u32));
				u32 bufferSize = static_cast<u32>(indexBufferAccessor.count * elementSize);
				subMesh->m_indexData = GfUniquePtr<char[]>(new char[bufferSize]);
				const unsigned char* rawData = model->buffers[view.buffer].data.data() + (view.byteOffset + indexBufferAccessor.byteOffset);
				GF_VERIFY (view.byteStride == 0, "Index buffer should not be interleaved") 
				memcpy(subMesh->m_indexData.get(), rawData, bufferSize);
			}

			// Get Position data
			GF_VERIFY(posIdx >= 0, "Invalid position data accessor");
			{
				const tinygltf::Accessor& accessor = model->accessors[posIdx];
				const tinygltf::BufferView& view = model->bufferViews[accessor.bufferView];
				GF_VERIFY(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && accessor.type == TINYGLTF_TYPE_VEC3, "Positions must be vec3");
				subMesh->m_vertexCount = static_cast<u32>(accessor.count);
				subMesh->m_posVertexData = GfUniquePtr<PosVertexBuffer[]>(new PosVertexBuffer[accessor.count]);
				if (view.byteStride == 0) 
				{
					memcpy(subMesh->m_posVertexData.get(), model->buffers[view.buffer].data.data() + view.byteOffset, view.byteLength);
				}
				else 
				{
					const unsigned char* src = model->buffers[view.buffer].data.data() + view.byteOffset + accessor.byteOffset;
					for (u32 i = 0; i < u32(accessor.count); ++i) 
					{
						memcpy(&subMesh->m_posVertexData[i], src, sizeof(PosVertexBuffer));
						src += view.byteStride;
					}
				}
			}

			subMesh->m_normUvTangentVertexData = GfUniquePtr<NormalsTangentUvVertexBuffer[]>(new NormalsTangentUvVertexBuffer[subMesh->m_vertexCount]);

			// Uv data
			if(uvIdx >= 0)
			{
				const tinygltf::Accessor& accessor = model->accessors[uvIdx];
				const tinygltf::BufferView& view = model->bufferViews[accessor.bufferView];
				GF_VERIFY(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && accessor.type == TINYGLTF_TYPE_VEC2, "UVs must be vec2");
				GF_VERIFY(static_cast<u32>(accessor.count) == subMesh->m_vertexCount, "Invalid number of UV coordinates");
				const unsigned char* src = model->buffers[view.buffer].data.data() + view.byteOffset + accessor.byteOffset;
				u32 stride = (view.byteStride != 0) ? static_cast<u32>(view.byteStride) : static_cast<u32>(sizeof(v2));
				for (u32 i = 0; i < subMesh->m_vertexCount; ++i) 
				{
					memcpy(&subMesh->m_normUvTangentVertexData[i].m_uv, &src[stride * i], sizeof(v2));
				}
			}
			else
			{
				GF_ERROR("%s", "No texture information for mesh found");
			}
			
			// Normal data
			if(normalIdx >= 0)
			{
				const tinygltf::Accessor& accessor = model->accessors[normalIdx];
				const tinygltf::BufferView& view = model->bufferViews[accessor.bufferView];
				GF_VERIFY(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && accessor.type == TINYGLTF_TYPE_VEC3, "Normals must be vec3");
				GF_VERIFY(static_cast<u32>(accessor.count) == subMesh->m_vertexCount, "Invalid number of normals");
				const unsigned char* src = model->buffers[view.buffer].data.data() + view.byteOffset + accessor.byteOffset;
				u32 stride = (view.byteStride != 0) ? static_cast<u32>(view.byteStride) : static_cast<u32>(sizeof(v3));
				for (u32 i = 0; i < subMesh->m_vertexCount; ++i) 
				{
					memcpy(&subMesh->m_normUvTangentVertexData[i].m_normal, src, sizeof(v3));
					src += stride;
				}
			}
			else
			{
				GF_ERROR("TODO. %s", "Generate normals based on triangle edges.");
			}

			// Tangent data
			if(tangentIdx >= 0)
			{
				const tinygltf::Accessor& accessor = model->accessors[tangentIdx];
				const tinygltf::BufferView& view = model->bufferViews[accessor.bufferView];
				GF_VERIFY(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT && accessor.type == TINYGLTF_TYPE_VEC3, "Tangents must be vec3");
				GF_VERIFY(static_cast<u32>(accessor.count) == subMesh->m_vertexCount, "Invalid number of tangents");
				const unsigned char* src = model->buffers[view.buffer].data.data() + view.byteOffset + accessor.byteOffset;
				u32 stride = (view.byteStride != 0) ? static_cast<u32>(view.byteStride) : static_cast<u32>(sizeof(v3));
				for (u32 i = 0; i < subMesh->m_vertexCount; ++i) 
				{
					memcpy(&subMesh->m_normUvTangentVertexData[i].m_tangent, src, sizeof(v3));
					src += stride;
				}
			}
			else
			{
				GF_WARNING("TODO. %s", "Generate tangents based on triangle edges.");
			}

			engineModel->m_subMeshes.push_back(std::move(subMesh));
		}
		return engineModel;
	}
	else 
	{
		GF_ERROR("Error loading Damaged helmet gltf:\n%s", error.data());
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
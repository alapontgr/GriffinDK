////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2021/01/14
//	File: 	GfMesh.h
//
//	Copyright (c) 2021 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Common/GfCore/GfCoreMinimal.h"
#include "Common/GfRender/GraphicResources/GfGraphicResources.h"

////////////////////////////////////////////////////////////////////////////////

struct PosVertexBuffer 
{
	v3 m_pos;
};

struct NormalsTangentUvVertexBuffer 
{
	v3 m_normal;
	v3 m_tangent;
	v2 m_uv;
};

enum class VertexBufferType : u32 
{
	Positions,
	NormalTangentUv,
	COUNT
};

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;
class GfCmdBuffer;

class GfSubMesh
{
public:

	friend class GfGltfLoader;
	friend class GfMesh;

	GfSubMesh();
	//GfSubMesh(const GfSubMesh&& other);
	~GfSubMesh();

	u32 getVertexCount() const 
	{
		return m_vertexCount;
	}

	u32 getIndexCount() const 
	{
		return m_indexCount;
	}

	bool doesUseShortForIndex() const 
	{
		return m_indexUShort;
	}

	u32 getIndexBufferOffset() const { return m_indexBufferOffset; }
	
	u32 getVertexBufferOffset(u32 vertexBufferIdx) const { return m_vertexBuffersOffsets[vertexBufferIdx]; }

private:
	GfSubMesh& operator= (const GfSubMesh&);
	GfSubMesh(const GfSubMesh&);

	class GfMesh* m_parent;
	u32 m_vertexCount;
	u32 m_indexCount;
	bool m_indexUShort;

	u32 m_indexBufferOffset;
	u32 m_vertexBuffersOffsets[static_cast<u32>(VertexBufferType::COUNT)];

	GfUniquePtr<char[]> m_indexData;
	GfUniquePtr<PosVertexBuffer[]> m_posVertexData;
	GfUniquePtr<NormalsTangentUvVertexBuffer[]> m_normUvTangentVertexData;
};

class GfMesh 
{
public:
	
	friend class GfGltfLoader;
	
	GfMesh();
	//GfMesh(const GfMesh&& other);
	~GfMesh();

	void create(const GfRenderContext& ctx, const GfCmdBuffer& cmdBuffer);
	void destroy(); // Queues the buffer to be destroyed

	u32 getMeshCount() const 
	{
		return static_cast<u32>(m_subMeshes.size());
	}

	const GfSubMesh& getMeshAt(u32 idx) 
	{
		GF_VERIFY(idx >= 0 && idx < getMeshCount(), "Trying to access to an invalid mesh");
		return *m_subMeshes[idx].get();
	}

	GfBuffer::Id getVertexBuffer() const { return m_vertexBuffer; }
	GfBuffer::Id getIndexBuffer() const { return m_indexBuffer; }

private:
	GfMesh& operator= (const GfMesh&);
	GfMesh(const GfMesh&);

	GfString m_name;
	GfVector<GfUniquePtr<GfSubMesh>> m_subMeshes;
	GfBuffer::Id m_vertexBuffer;
	GfBuffer::Id m_indexBuffer;
};

////////////////////////////////////////////////////////////////////////////////

class GfGltfLoader 
{
public:
	static GfUniquePtr<GfMesh> Load(const GfString& filePath);
};
////////////////////////////////////////////////////////////////////////////////
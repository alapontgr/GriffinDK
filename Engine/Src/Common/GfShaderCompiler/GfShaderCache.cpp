////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	File: 	GfShaderCache.cpp
//
//	Copyright (c) 2021 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////

#include "Common/GfShaderCompiler/GfShaderCache.h"
#include "Common/GfFile/GfFile.h"
#include "json/single_include/nlohmann/json.hpp"

#include <sstream>

////////////////////////////////////////////////////////////////////////////////

u32 alignPivot(u32 pivot, u32 alignment) 
{
	return (pivot + (alignment - (pivot % alignment)));
}

////////////////////////////////////////////////////////////////////////////////

GfShaderSerializer::GfShaderSerializer() 
{
	m_mutators.fill(-1);
	m_mutatorCount = 0;
	m_usedStages = 0;
}

void GfShaderSerializer::serialize(u8* data, u32& size) const
{
	u32 pivot = 0;

	GfPipelineBlobHeader* header = reinterpret_cast<GfPipelineBlobHeader*>(data);
	pivot += static_cast<u32>(sizeof(GfPipelineBlobHeader));
	if (data) 
	{
		header->m_mutatorCount = m_mutatorCount;
		header->m_usedStages = m_usedStages;
		header->m_stringCount = static_cast<u32>(m_stringCache.size());
		header->m_bytecodesCount = static_cast<u32>(m_bytecodeSizes.size());
		header->m_variantsCount = static_cast<u32>(m_variants.size());
	}

	// Serialize strings
	{
		u32 stringCount = static_cast<u32>(m_stringCache.size());
		u32 tableSize = static_cast<u32>(sizeof(u32)) * stringCount;
		u32 stringsReqSize = 0;
		u32* stringOffsets(nullptr);
		u32* stringSizes(nullptr);
		u8* stringsData(nullptr);
		if (data) 
		{
			stringOffsets = reinterpret_cast<u32*>(header+1);
			stringSizes = stringOffsets + stringCount;
			stringsData = reinterpret_cast<u8*>(stringSizes+stringCount);
			header->m_stringCacheOffsets = pivot;
			header->m_stringCacheSizes = pivot + stringCount * tableSize;
		}
		pivot += tableSize * 2;
		for (u32 i = 0; i < stringCount; ++i) 
		{
			u32 reqSize = static_cast<u32>(m_stringCache[i].size() + 1);
			if (data) 
			{
				stringOffsets[i] = pivot + stringsReqSize;
				stringSizes[i] = static_cast<u32>(m_stringCache[i].size());
				memcpy(stringsData + stringsReqSize, m_stringCache[i].c_str(), reqSize);
			}
			stringsReqSize += reqSize; // Include null terminator
		}
		pivot = alignPivot(pivot + stringsReqSize, 16);
	}

	// Serialize mutators
	{
		u32 mutatorsReqSize = static_cast<u32>(sizeof(u32) * m_mutators.size());
		if (data) 
		{
			header->m_mutatorsOffset = pivot;
			memcpy(data + pivot, m_mutators.data(), mutatorsReqSize);
		}
		pivot = alignPivot(pivot + mutatorsReqSize, 16);
	}

	// Serialize bytecodes
	{
		u32 bytecodesCount = static_cast<u32>(m_bytecodeCache.size());
		u32 tableSize = static_cast<u32>(sizeof(u32)) * bytecodesCount;
		u32 bytecodesReqSize = 0;
		u32* bytecodesOffsets(nullptr);
		u32* bytecodesSizes(nullptr);
		u8* bytecodesData(nullptr);
		if (data) 
		{
			bytecodesOffsets = reinterpret_cast<u32*>(data + pivot);
			bytecodesSizes = bytecodesOffsets + bytecodesCount;
			bytecodesData = reinterpret_cast<u8*>(bytecodesSizes + bytecodesCount);
			header->m_bytecodeCacheOffsets = pivot;
			header->m_bytecodeSizes = pivot + bytecodesCount * tableSize;			
		}
		pivot += tableSize * 2; // *2 because dealing with two tables (bytecodesSizes & bytecodesOffsets)
		for (u32 i=0; i<bytecodesCount; ++i) 
		{
			if (data) 
			{
				bytecodesOffsets[i] = pivot + bytecodesReqSize;
				bytecodesSizes[i] = m_bytecodeSizes[i];
				memcpy(bytecodesData + bytecodesReqSize, m_bytecodeCache[i].get(), m_bytecodeSizes[i]);
			}
			bytecodesReqSize += m_bytecodeSizes[i];
		}
		pivot = alignPivot(pivot + bytecodesReqSize, 16);
	}

	// Serialize descriptors
	{
		u32 descriptorsReqSize = static_cast<u32>(sizeof(GfDescriptorBindingSlot) * m_descriptors.size());
		if (data && descriptorsReqSize) 
		{
			header->m_descriptorsOffset = pivot;
			memcpy(data + pivot, &m_descriptors[0], descriptorsReqSize);
		}
		pivot = alignPivot(pivot + descriptorsReqSize, 16);
	}

	// Serialize variants data
	{
		u32 variantHashesArraySize = static_cast<u32>(sizeof(GfVariantHash) * m_variants.size());
		u32 variantDataSize = static_cast<u32>(sizeof(GfShaderVariantData));
		u32* variantHashes(nullptr);
		u8* variantData(nullptr);
		if (data) 
		{
			header->m_variantsCount = static_cast<u32>(m_variants.size());
			header->m_variantsHashesArrayOffset = pivot;
			header->m_variantsOffset = pivot + variantHashesArraySize;
			variantHashes = reinterpret_cast<u32*>(data + pivot);
			variantData = (data + pivot + variantHashesArraySize);
		}
		u32 variantsReqSize = 0;
		u32 variantIdx = 0;
		for (auto it = m_variants.begin(); it != m_variants.end(); ++it) 
		{
			GF_ASSERT(((pivot + variantsReqSize) % alignof(GfShaderVariantData)) == 0, "Invalid alignment");
			if (data) 
			{
				variantHashes[variantIdx++] = it->first;
				memcpy(variantData, &it->second->m_data, variantDataSize);
			}
			variantsReqSize += variantDataSize;
		}
		pivot = alignPivot(pivot + variantsReqSize, 16);
	}
	
	if (data) 
	{
		GF_ASSERT(size == pivot, "Invalid size");
	}
	else 
	{
		size = pivot;
	}
}

s32 GfShaderSerializer::addString(const GfString& token)
{
	size_t idx = m_stringCache.size();
	m_stringCache.push_back(token);
	return static_cast<s32>(idx);
}

void GfShaderSerializer::enableStage(ShaderStage::Type stage)
{
	m_usedStages |= (1<<stage);
}

void GfShaderSerializer::addMutator(const GfString& mutatorName)
{
	if (m_mutatorCount < s_MAX_MUTATOR_COUNT) 
	{
		m_mutators[m_mutatorCount++] = addString(mutatorName);
	}
}

GfString GfShaderSerializer::getMutatorNameAt(u32 idx) const
{
	if (idx < m_mutatorCount) 
	{
		return m_stringCache[m_mutators[idx]];
	}
	return "";
}

GfString GfShaderSerializer::mutatorHashToDefines(GfVariantHash hash) const
{
	GfString header("");
	for (u32 i = 0; i < m_mutatorCount; ++i) 
	{
		if ((hash & (1 << i)) != 0) 
		{
			header += "#define " + getMutatorNameAt(i) + " 1 \n";
		}
	}
	return std::move(header);
}

u32 GfShaderSerializer::getMutatorCount() const
{
	return m_mutatorCount;
}

u32 GfShaderSerializer::getStageEnabled(ShaderStage::Type stage) const
{
	return (m_usedStages & (1<<stage)) != 0;
}

s32 GfShaderSerializer::addShaderBytecode(ShaderBytecode shaderBytecode, u32 size)
{
	size_t idx = m_bytecodeCache.size();
	m_bytecodeCache.push_back(std::move(shaderBytecode));
	m_bytecodeSizes.push_back(size);
	return static_cast<s32>(idx);
}

s32 GfShaderSerializer::addBindingsArray(const GfDescriptorBindingSlot* bindingsArray, u32 count)
{
	u32 idx = static_cast<u32>(m_descriptors.size());
	m_descriptors.insert(m_descriptors.end(), bindingsArray, bindingsArray + count);
	return idx;
}

GfShaderSerializer::ShaderVariant* GfShaderSerializer::getVariant(GfVariantHash mutatorHash)
{
	auto entry = m_variants.find(mutatorHash);
	if (entry == m_variants.end())
	{
		GfUniquePtr<ShaderVariant> variant = std::make_unique<ShaderVariant>();
		ShaderVariant* ptr = variant.get();
		m_variants[mutatorHash] = std::move(variant);
		return ptr;
	}
	return entry->second.get();
}

void GfShaderSerializer::ShaderVariant::setVariantShaderBytecode(ShaderStage::Type stage, s32 bytecodeIdx)
{
	m_data.m_stagesBytecodeIdxs[stage] = bytecodeIdx;
}

void GfShaderSerializer::ShaderVariant::setDescriptorSetLayoutRangeAndHash(u32 set, s16 idx, s16 count, u64 hash)
{
	m_data.m_setBindingsIdx[set] = idx;
	m_data.m_setsBindingsCount[set] = count;
	m_data.m_setsLayoutHash[set] = hash;
}

s32 GfShaderSerializer::ShaderVariant::getBytecodeIndexForStage(ShaderStage::Type stage) const
{
	return m_data.m_stagesBytecodeIdxs[stage];
}

GfUniquePtr<u8[]> GfShaderSerializer::serializeToBlob(u32& reqSize) const
{
	serialize(nullptr, reqSize);
	GfUniquePtr<u8[]> data(new u8[reqSize]);
	serialize(data.get(), reqSize);
	return data;
}

////////////////////////////////////////////////////////////////////////////////

void GfShaderDeserializer::deserialize(GfUniquePtr<u8[]>&& blob)
{
	m_binary = std::move(blob);
	GfPipelineBlobHeader* header = reinterpret_cast<GfPipelineBlobHeader*>(blob.get());
	m_stringOffsets.set(reinterpret_cast<u32*>(blob.get() + header->m_stringCacheOffsets), header->m_stringCount);
	m_bytecodeOffsets.set(reinterpret_cast<u32*>(blob.get() + header->m_bytecodeCacheOffsets), header->m_bytecodesCount);
	m_bytecodeSizes.set(reinterpret_cast<u32*>(blob.get() + header->m_bytecodeSizes), header->m_bytecodesCount);
	m_mutators.set(reinterpret_cast<s32*>(blob.get() + header->m_mutatorsOffset), header->m_mutatorCount);
	// Deserialize variants data
	m_variantsDataCache.clear();
	for (u32 i = 0; i < header->m_variantsCount; ++i) 
	{
		u32 variantHash = reinterpret_cast<const u32*>(m_binary.get() + header->m_variantsHashesArrayOffset)[i];
		const GfShaderVariantData* variantData = reinterpret_cast<const GfShaderVariantData*>(m_binary.get() + header->m_variantsOffset) + i;
		m_variantsDataCache[variantHash] = variantData;
	}
}

s32 GfShaderDeserializer::findIdxForMutator(const GfString& mutatorName) const
{
	GfPipelineBlobHeader* header = reinterpret_cast<GfPipelineBlobHeader*>(m_binary.get());
	for (s32 i = 0; i < static_cast<s32>(header->m_mutatorCount); ++i) 
	{
		if (mutatorName == getCachedString(m_mutators[i])) 
		{
			return i;
		}
	}
	return -1;
}

const GfShaderVariantData* GfShaderDeserializer::getVariantData(u32 variantHash) const
{
	auto entry = m_variantsDataCache.find(variantHash);
	return (entry != m_variantsDataCache.end()) ? entry->second : nullptr;
}

const GfDescriptorBindingSlot* GfShaderDeserializer::getDescriptorBindings(const GfShaderVariantData* variant, const u32 descSet, u32& bindingCount, u64& layoutHash) const
{
	GF_ASSERT(descSet < s_MAX_DESCRIPTOR_SETS, "Invalid descriptor set index");
	GF_ASSERT(variant, "Invalid variant");
	s16 bindingsIdx = variant->m_setBindingsIdx[descSet];
	if (bindingsIdx >= 0 && variant->m_setsBindingsCount[descSet] > 0) 
	{
		bindingCount = static_cast<u32>(variant->m_setsBindingsCount[descSet]);
		layoutHash = variant->m_setsLayoutHash[descSet];
		return getDescriptorBindings(variant->m_setBindingsIdx[descSet]);
	}
	return nullptr;
}

const u32* GfShaderDeserializer::getStageBytecodeForVariant(const GfShaderVariantData* variant, ShaderStage::Type stage, size_t& bytecodeSize) const
{
	GF_ASSERT(variant, "Invalid variant");
	bytecodeSize = static_cast<size_t>(getBytecodeSize(variant->m_stagesBytecodeIdxs[stage]));
	return getBytecodePtr(variant->m_stagesBytecodeIdxs[stage]);
}

bool GfShaderDeserializer::isGraphics() const
{
	GfPipelineBlobHeader* header = reinterpret_cast<GfPipelineBlobHeader*>(m_binary.get());
	return (header->m_usedStages & ((1<<ShaderStage::Vertex) | (1 << ShaderStage::Fragment))) != 0;
}

bool GfShaderDeserializer::isCompute() const
{
	GfPipelineBlobHeader* header = reinterpret_cast<GfPipelineBlobHeader*>(m_binary.get());
	return (header->m_usedStages & (1<<ShaderStage::Compute)) != 0;;
}

u32 GfShaderDeserializer::getUsedStages() const
{
	GfPipelineBlobHeader* header = reinterpret_cast<GfPipelineBlobHeader*>(m_binary.get());
	return header->m_usedStages;
}

const GfDescriptorBindingSlot* GfShaderDeserializer::getDescriptorBindings(u32 idx) const
{
	GfPipelineBlobHeader* header = reinterpret_cast<GfPipelineBlobHeader*>(m_binary.get());
	GfDescriptorBindingSlot* descriptorBindings = reinterpret_cast<GfDescriptorBindingSlot*>(m_binary.get() + header->m_descriptorsOffset);
	return descriptorBindings + idx;
}

const char* GfShaderDeserializer::getCachedString(u32 idx) const
{
	const char* str = reinterpret_cast<const char*>(m_binary.get() + m_stringOffsets[idx]);
	return str;
}

const u32* GfShaderDeserializer::getBytecodePtr(u32 idx) const
{
	const u32* bytecode = reinterpret_cast<const u32*>(m_binary.get() + m_bytecodeOffsets[idx]);
	return bytecode;
}

u32 GfShaderDeserializer::getBytecodeSize(u32 idx) const
{
	return m_bytecodeSizes[idx];
}

////////////////////////////////////////////////////////////////////////////////

static const char* s_ShaderCacheFilename = "GfShaderCache.json";

void GfShaderCacheFile::init(const GfString& cacheDirPath)
{
	m_fileHashToName.clear();
	m_filenameToHash.clear();
	m_cacheDirPath = cacheDirPath;
	std::replace( m_cacheDirPath.begin(), m_cacheDirPath.end(), '\\', '/');
	if (m_cacheDirPath[m_cacheDirPath.size() - 1] != '/') 
	{
		m_cacheDirPath += '/';
	}
	GfString filePath = m_cacheDirPath + s_ShaderCacheFilename;
	if (GfFile::DoesFileExist(filePath.c_str()))
	{
		loadParseCache(filePath);
	}
}

bool GfShaderCacheFile::skipCompilation(const GfString& shaderName, const u64 srcHash) const
{
	u64 nameHash = GfHash::compute(shaderName);
	const auto entry = m_filenameToHash.find(nameHash);
	if (entry != m_filenameToHash.end())
	{
		return entry->second == srcHash;
	}
	return false;
}

void GfShaderCacheFile::registerShaderBlob(const GfString& filename, const u64 srcHash, const GfShaderSerializer& shader)
{
	u64 nameHash = GfHash::compute(filename);
	m_filenameToHash[nameHash] = srcHash;
	m_fileHashToName[nameHash] = filename;

	GfString shaderFile = getShaderFilename(nameHash, m_cacheDirPath);

	u32 blobSize(0);
	GfUniquePtr<u8[]> blob = shader.serializeToBlob(blobSize);
	
	// Write file
	GfFileHandle kFile;
	if (GfFile::OpenFile(shaderFile.c_str(), EFileAccessMode::Write, kFile)) 
	{
		GfFile::WriteBytes(kFile, blobSize, blob.get());
		GfFile::CloseFile(kFile);
	}

	saveCache(m_cacheDirPath + s_ShaderCacheFilename);
}

GfString GfShaderCacheFile::getShaderFile(const GfString& shaderName) const
{
	u64 nameHash = GfHash::compute(shaderName);
	const auto entry = m_filenameToHash.find(nameHash);
	GfString shaderFile = m_cacheDirPath;
	if (entry != m_filenameToHash.end()) 
	{
		return getShaderFilename(nameHash, m_cacheDirPath);
	}
	return "";
}

GfString GfShaderCacheFile::getShaderFilename(const u64 srcHash, const GfString& basePath)
{
	GfString shaderFile = basePath;
	std::ostringstream oss;
	oss << srcHash;
	shaderFile += oss.str();
	return std::move(shaderFile);
}

void GfShaderCacheFile::loadParseCache(const GfString& filePath)
{
	if (GfFile::DoesFileExist(filePath.c_str())) 
	{
		u32 fileSize(0);
		GfFileHandle kFile;
		GfFile::OpenFile(filePath.c_str(), EFileAccessMode::Read, kFile);
		GfFile::GetFileSize(kFile);
		fileSize = static_cast<u32>(kFile.GetFileSize());
		if (fileSize > 0)
		{
			GfUniquePtr<char[]> pSrc(new char[fileSize + 1]);
			u32 uiRead = GfFile::ReadBytes(kFile, fileSize, pSrc.get());
			GfFile::CloseFile(kFile);
			GF_ASSERT(uiRead == fileSize, "Invalid size read");
			pSrc[fileSize] = 0;
			
			nlohmann::json jsonFileRoot = nlohmann::json::parse(pSrc.get());
			nlohmann::json cacheJson = jsonFileRoot["cache"];
			for (auto& element : cacheJson) 
			{
				u64 filenameHash = element["fileHash"];
				u64 srcHash = element["srcHash"];

				GfString shaderFilePath = getShaderFilename(filenameHash, m_cacheDirPath);
				if (GfFile::DoesFileExist(shaderFilePath.c_str())) 
				{
					m_filenameToHash[filenameHash] = srcHash;
				}
			}
		}
	}
}

void GfShaderCacheFile::saveCache(const GfString& filePath)
{
	GfFile::CreateDir(m_cacheDirPath.c_str());

	nlohmann::json root;
	nlohmann::json cache;
	for (auto& it : m_filenameToHash) 
	{
		nlohmann::json entry;
		entry["fileHash"] = it.first;
		entry["srcHash"] = it.second;
		entry["ShaderFilename"] = m_fileHashToName[it.first];
		cache.push_back(entry);
	}
	root["cache"] = cache;
	GfString jsonFile = root.dump();

	GfFileHandle handle;
	if (GfFile::OpenFile(filePath.c_str(), EFileAccessMode::Write, handle)) 
	{
		GfFile::WriteBytes(handle, jsonFile.size(), jsonFile.data());
		GfFile::CloseFile(handle);
	}
}

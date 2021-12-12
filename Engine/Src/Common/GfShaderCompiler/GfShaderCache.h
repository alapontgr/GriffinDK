#pragma once
////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	File: 	GfShaderCache.h
//
//	Copyright (c) 2021 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFSHADERCACHE_H__
#define __GFSHADERCACHE_H__
////////////////////////////////////////////////////////////////////////////////

#include "Common/GfCore/GfCoreMinimal.h"
#include "Common/GfRender/GfRenderCommon.h"

////////////////////////////////////////////////////////////////////////////////

struct GfPipelineBlobHeader 
{
	u32 m_mutatorCount;
	u32 m_usedStages;
	
	u32 m_stringCount;
	// Array with the offsets to the beginning of every string
	u32 m_stringCacheOffsets; 
	// Array with the size of each string
	u32 m_stringCacheSizes; 
	
	u32 m_bytecodesCount;
	// Offset to the beginning of each blob of shader bytecodes.
	u32 m_bytecodeCacheOffsets; 
	// Offset to the array of sizes of each blob of shader bytecodes.
	u32 m_bytecodeSizes; 

	// Offset to the first element of the array of descriptors.
	u32 m_descriptorsOffset; 

	// Offset to the array of mutators.
	u32 m_mutatorsOffset;

	// Serialize shader variants
	u32 m_variantsCount;
	u32 m_variantsHashesArrayOffset;
	// Offset to the array of variants data
	u32 m_variantsOffset;
};

class GfShaderSerializer
{
public:

	friend class GfShaderCompiler;
	using ShaderBytecode = GfUniquePtr<u32[]>;
	
	using BytecodeHash = u64;

	struct ShaderVariant 
	{
		ShaderVariant() 
		{
			m_data.m_stagesBytecodeIdxs.fill(-1);
			m_data.m_setBindingsIdx.fill(-1);
			m_data.m_setsLayoutHash.fill(0);
		}

		void setVariantShaderBytecode(ShaderStage::Type stage, s32 bytecodeIdx);

		void setDescriptorSetLayoutRangeAndHash(u32 set, s16 idx, u64 hash);

		s32 getBytecodeIndexForStage(ShaderStage::Type stage) const;

		GfShaderVariantData m_data;
	};

	GfShaderSerializer();

	void enableStage(ShaderStage::Type stage);

	void addMutator(const GfString& mutatorName);

	GfString getMutatorNameAt(u32 idx) const;

	GfString mutatorHashToDefines(GfVariantHash hash) const;

	u32 getMutatorCount() const;

	u32 getStageEnabled(ShaderStage::Type stage) const;

	ShaderVariant* getVariant(GfVariantHash mutatorHash);

	// Queues the shader bytecode at the end of the m_bytecodeCache
	s32 addShaderBytecode(ShaderBytecode shaderBytecode, u32 size);

	s32 addBindingsArray(const GfArray<GfDescriptorBindingSlot, s_MAX_BINDINGS_PER_SET>& bindings, const u64 hash);

	GfUniquePtr<u8[]> serializeToBlob(u32& size) const;

private:	

	void serialize(u8* data, u32& size) const;

	s32 addString(const GfString& token);

	static constexpr u32 s_MAX_MUTATOR_COUNT = 32;

	GfVector<GfString> m_stringCache;
	
	// Cache of bytecodes (spirvs)
	GfVector<ShaderBytecode> m_bytecodeCache;
	GfVector<u32> m_bytecodeSizes;
	
	// Add arrays of descriptor bindings to the cache (m_descriptors), 
	// track the first entry of each array with (m_descriptorBindingArrayOffsets)
	GfVector<GfDescriptorBindingSlot> m_descriptors;
	GfUMap<u64, u32> m_bindingsIndexCache;

	GfUMap<u32, GfUniquePtr<ShaderVariant>> m_variants;
	GfArray<s32, s_MAX_MUTATOR_COUNT> m_mutators;
	u32 m_mutatorCount;
	u32 m_usedStages;
};

class GfShaderDeserializer
{
public:

	GfShaderDeserializer();

	void deserialize(GfUniquePtr<u8[]>&& blob);

	void deserialize(const GfWeakArray<u8>& blob);

	s32 findIdxForMutator(const GfString& mutatorName) const;

	const GfShaderVariantData* getVariantData(GfVariantHash variantHash) const;

	const GfWeakArray<GfDescriptorBindingSlot> getDescriptorBindings(const GfShaderVariantData* variant, const u32 descSet, u64& layoutHash) const;

	const u32* getStageBytecodeForVariant(const GfShaderVariantData* variant, ShaderStage::Type stage, size_t& bytecodeSize) const;

	bool isGraphics() const;

	bool isCompute() const;

	u32 getUsedStages() const;

private:

	const GfDescriptorBindingSlot* getDescriptorBindings(u32 idx) const;

	const char* getCachedString(u32 idx) const;

	const u32* getBytecodePtr(u32 idx) const;

	void deserializeBlob();

	u32 getBytecodeSize(u32 idx) const;

	GfUniquePtr<u8[]> m_binary;
	GfWeakArray<u8> m_binaryWeak;
	const u8* m_blob;

	GfWeakArray<u32> m_stringOffsets;
	GfWeakArray<u32> m_bytecodeOffsets;
	GfWeakArray<u32> m_bytecodeSizes;
	GfWeakArray<s32> m_mutators;
	GfUMap<u32, const GfShaderVariantData*> m_variantsDataCache;
};

////////////////////////////////////////////////////////////////////////////////

class GfShaderCache
{
public:
	GfShaderCache() {}
	virtual bool skipCompilation(const GfString& shaderName, const u64 srcHash) const = 0;
	virtual void registerShaderBlob(const GfString& filename, const u64 srcHash, const GfShaderSerializer& shader) = 0;
private:
};

class GfShaderCacheFile : public GfShaderCache
{
public:
	GfShaderCacheFile() {}
	
	void init(const GfString& cacheDirPath);

	virtual bool skipCompilation(const GfString& shaderName, const u64 srcHash) const override;
	
	virtual void registerShaderBlob(const GfString& filename, const u64 srcHash, const GfShaderSerializer& shader) override;

	GfString getShaderFile(const GfString& shaderName) const;
	
	GfString getShaderFilename(const GfString& shaderName, const GfString& cacheDirPath) const;

	GfWeakArray<u8> getShaderBlob(const GfString& shaderName);


private:

	struct BlobWithSize 
	{
		GfUniquePtr<u8[]> m_blob;
		u32 m_size;
	};

	static GfString getShaderFilename(const u64 shaderFileHash, const GfString& basePath);

	void loadParseCache(const GfString& filePath);

	void saveCache(const GfString& filePath);

	GfWeakArray<u8> loadShaderBlobFromFile(const GfString& shaderName);

	GfString m_cacheDirPath;
	GfUMap<u64, u64> m_filenameToHash;
	GfUMap<u64, GfString> m_fileHashToName;
	GfUMap<u64, BlobWithSize> m_fileHashToBlob;
};

////////////////////////////////////////////////////////////////////////////////
#endif
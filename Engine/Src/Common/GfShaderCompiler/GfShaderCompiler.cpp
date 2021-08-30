////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	File: 	GfShaderCompiler.cpp
//	Copyright (c) 2021 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////

#include "Common/GfShaderCompiler/GfShaderCompiler.h"
#include "glslang/glslang/Public/ShaderLang.h"
#include "glslang/SPIRV/GlslangToSpv.h"
#include "spirv-cross/spirv_cross.hpp"
#include <sstream>
#include <algorithm>

////////////////////////////////////////////////////////////////////////////////

static GfUMap<u64, EShaderStage::Type> s_hashToShaderStage = 
{
	{GfHash::compute("GF_VS"), EShaderStage::Vertex},
	{GfHash::compute("GF_FS"), EShaderStage::Fragment},
	{GfHash::compute("GF_CS"), EShaderStage::Compute},
};

static GfUMap<EShaderStage::Type, GfString> s_stageToDefine = 
{
	{EShaderStage::Vertex, GfString("#define GF_VS 1\n")},
	{EShaderStage::Fragment, GfString("#define GF_FS 1\n")},
	{EShaderStage::Compute, GfString("#define GF_CS 1\n")},
};
static GfString s_Version = "#version 450\n";

static GfUMap<EShaderStage::Type, EShLanguage> s_shaderStageToGLSLangStage =
{
	{EShaderStage::Vertex, EShLangVertex},
	//{EShLangTessControl},
	//{EShLangTessEvaluation},
	//{EShLangGeometry},
	{EShaderStage::Fragment, EShLangFragment},
	{EShaderStage::Compute, EShLangCompute},
	//{EShLangRayGen},
	//{EShLangRayGenNV = EShLangRayGen},
	//{EShLangIntersect},
	//{EShLangIntersectNV = EShLangIntersect},
	//{EShLangAnyHit},
	//{EShLangAnyHitNV = EShLangAnyHit},
	//{EShLangClosestHit},
	//{EShLangClosestHitNV = EShLangClosestHit},
	//{EShLangMiss},
	//{EShLangMissNV = EShLangMiss},
	//{EShLangCallable},
	//{EShLangCallableNV = EShLangCallable},
	//{EShLangTaskNV},
	//{EShLangMeshNV},
};

static GfString addShaderStageHeader(const GfString& src, EShaderStage::Type stage) 
{
	return s_Version + s_stageToDefine[stage] + src;
}

static TBuiltInResource getShaderLimits() 
{
	TBuiltInResource limits{};

	limits.maxLights = 32;
	limits.maxClipPlanes = 6;
	limits.maxTextureUnits = 32;
	limits.maxTextureCoords = 32;
	limits.maxVertexAttribs = 64;
	limits.maxVertexUniformComponents = 4096;
	limits.maxVaryingFloats = 64;
	limits.maxVertexTextureImageUnits = 32;
	limits.maxCombinedTextureImageUnits = 80;
	limits.maxTextureImageUnits = 32;
	limits.maxFragmentUniformComponents = 4096;
	limits.maxDrawBuffers = 32;
	limits.maxVertexUniformVectors = 128;
	limits.maxVaryingVectors = 8;
	limits.maxFragmentUniformVectors = 16;
	limits.maxVertexOutputVectors = 16;
	limits.maxFragmentInputVectors = 15;
	limits.minProgramTexelOffset = -8;
	limits.maxProgramTexelOffset = 7;
	limits.maxClipDistances = 8;
	limits.maxComputeWorkGroupCountX = 65535;
	limits.maxComputeWorkGroupCountY = 65535;
	limits.maxComputeWorkGroupCountZ = 65535;
	limits.maxComputeWorkGroupSizeX = 1024;
	limits.maxComputeWorkGroupSizeY = 1024;
	limits.maxComputeWorkGroupSizeZ = 64;
	limits.maxComputeUniformComponents = 1024;
	limits.maxComputeTextureImageUnits = 16;
	limits.maxComputeImageUniforms = 8;
	limits.maxComputeAtomicCounters = 8;
	limits.maxComputeAtomicCounterBuffers = 1;
	limits.maxVaryingComponents = 60;
	limits.maxVertexOutputComponents = 64;
	limits.maxGeometryInputComponents = 64;
	limits.maxGeometryOutputComponents = 128;
	limits.maxFragmentInputComponents = 128;
	limits.maxImageUnits = 8;
	limits.maxCombinedImageUnitsAndFragmentOutputs = 8;
	limits.maxCombinedShaderOutputResources = 8;
	limits.maxImageSamples = 0;
	limits.maxVertexImageUniforms = 0;
	limits.maxTessControlImageUniforms = 0;
	limits.maxTessEvaluationImageUniforms = 0;
	limits.maxGeometryImageUniforms = 0;
	limits.maxFragmentImageUniforms = 8;
	limits.maxCombinedImageUniforms = 8;
	limits.maxGeometryTextureImageUnits = 16;
	limits.maxGeometryOutputVertices = 256;
	limits.maxGeometryTotalOutputComponents = 1024;
	limits.maxGeometryUniformComponents = 1024;
	limits.maxGeometryVaryingComponents = 64;
	limits.maxTessControlInputComponents = 128;
	limits.maxTessControlOutputComponents = 128;
	limits.maxTessControlTextureImageUnits = 16;
	limits.maxTessControlUniformComponents = 1024;
	limits.maxTessControlTotalOutputComponents = 4096;
	limits.maxTessEvaluationInputComponents = 128;
	limits.maxTessEvaluationOutputComponents = 128;
	limits.maxTessEvaluationTextureImageUnits = 16;
	limits.maxTessEvaluationUniformComponents = 1024;
	limits.maxTessPatchComponents = 120;
	limits.maxPatchVertices = 32;
	limits.maxTessGenLevel = 64;
	limits.maxViewports = 16;
	limits.maxVertexAtomicCounters = 0;
	limits.maxTessControlAtomicCounters = 0;
	limits.maxTessEvaluationAtomicCounters = 0;
	limits.maxGeometryAtomicCounters = 0;
	limits.maxFragmentAtomicCounters = 8;
	limits.maxCombinedAtomicCounters = 8;
	limits.maxAtomicCounterBindings = 1;
	limits.maxVertexAtomicCounterBuffers = 0;
	limits.maxTessControlAtomicCounterBuffers = 0;
	limits.maxTessEvaluationAtomicCounterBuffers = 0;
	limits.maxGeometryAtomicCounterBuffers = 0;
	limits.maxFragmentAtomicCounterBuffers = 1;
	limits.maxCombinedAtomicCounterBuffers = 1;
	limits.maxAtomicCounterBufferSize = 16384;
	limits.maxTransformFeedbackBuffers = 4;
	limits.maxTransformFeedbackInterleavedComponents = 64;
	limits.maxCullDistances = 8;
	limits.maxCombinedClipAndCullDistances = 8;
	limits.maxSamples = 4;

	limits.limits.nonInductiveForLoops = 1;
	limits.limits.whileLoops = 1;
	limits.limits.doWhileLoops = 1;
	limits.limits.generalUniformIndexing = 1;
	limits.limits.generalAttributeMatrixVectorIndexing = 1;
	limits.limits.generalVaryingIndexing = 1;
	limits.limits.generalSamplerIndexing = 1;
	limits.limits.generalVariableIndexing = 1;
	limits.limits.generalConstantMatrixVectorIndexing = 1;

	return limits;
}

static TBuiltInResource s_glslangLimits = getShaderLimits();

////////////////////////////////////////////////////////////////////////////////

static void tokenize(const GfString& line, GfVector<GfString>& tokens) 
{
	tokens.clear();
	std::stringstream stream(line);
	GfString tmpStr;
	while(std::getline(stream, tmpStr, ' '))
	{
		tokens.push_back(tmpStr);
	}
}

static void addUniqueIncludeHeader(const GfString& filename, GfString& outSrc) 
{
	u64 hash = GfHash::compute(filename);
	GfString hashStr = std::to_string(hash);
	outSrc += "#ifndef GF_INCLUDE_" + hashStr + "\n#define GF_INCL_" + hashStr + "\n";
}

////////////////////////////////////////////////////////////////////////////////

GfShaderCompiler::GfShaderCompiler()
{
	glslang::InitializeProcess();
}

GfShaderCompiler::~GfShaderCompiler()
{
	glslang::FinalizeProcess();
}


void GfShaderCompiler::init(const GfString& globalShaderPath, const GfString& baseShaderPath)
{
	m_baseShaderDirPath = baseShaderPath;
	m_engineShaderDirPath = globalShaderPath;
}

GfUniquePtr<GfShaderSerializer> GfShaderCompiler::compileShader(const GfString& filename, const GfString& src, GfShaderCache& filter, GfString& outErrors)
{
	GfUniquePtr<GfShaderSerializer> shaderSerializer = std::make_unique<GfShaderSerializer>();

	GfString parsedSrc;
	if (!parse(filename, src, *shaderSerializer.get(), parsedSrc, outErrors)) 
	{		
		return nullptr;
	}

	// Create an absolute hash
	u64 srcHash = GfHash::compute(parsedSrc);

	// Filter based on hash (likely to check against hash in cache)
	if (filter.skipCompilation(filename, srcHash)) 
	{
		return nullptr;
	}

	// Uberize based on mutators
	GfVector<GfShaderSerializer::MutatorHash> mutatorHashes = uberize(*shaderSerializer.get());

	// TODO: Distribute variants compilation
	GfVector<GfShaderSerializer::ShaderBytecode> bytecodeCache;
	GfVector<u32> bytecodeSizes;
	GfUMap<u64, s32> bytecodeHashToIdx;
	// Reserve being optimistic (as if every mutator had just one shader stage). To save a few resizes
	bytecodeCache.reserve(mutatorHashes.size());
	bytecodeHashToIdx.reserve(mutatorHashes.size());

	GfString compilationErrors("");
	GfString compilationWarnings("");

	// Compile variants
	bool success = true;
	for (GfShaderSerializer::MutatorHash hash : mutatorHashes) 
	{
		success = success && compileVariant(
			parsedSrc, 
			*shaderSerializer.get(), 
			hash,
			shaderSerializer->getVariant(hash), 
			&bytecodeCache,
			&bytecodeSizes,
			&bytecodeHashToIdx,
			compilationErrors,
			compilationWarnings);
	}

	if (!success) 
	{
		outErrors = compilationErrors;
		return nullptr;
	}

	// Add bytecodes to the cache of the serializer
	for (u32 i=0; i<static_cast<u32>(bytecodeCache.size()); ++i) 
	{
		shaderSerializer->addShaderBytecode(std::move(bytecodeCache[i]), bytecodeSizes[i]);
	}

	// Reflection
	for (GfShaderSerializer::MutatorHash hash : mutatorHashes) 
	{
		success = success && reflectVariant(
			*shaderSerializer.get(), 
			shaderSerializer->getVariant(hash), 
			compilationErrors);
	}

	if (!success) 
	{
		outErrors = compilationErrors;
		return nullptr;
	}

	// Register this shader in the cache
	filter.registerShaderBlob(filename, srcHash, *shaderSerializer);

	return shaderSerializer;
}

bool GfShaderCompiler::parse(const GfString& filename, const GfString& src, GfShaderSerializer& outSerializer, GfString& outSrc, GfString& outErrors)
{
	outSrc.reserve(src.size());
	GfString line("");
	std::istringstream tokenStream(src);
	GfVector<GfString> tokens;
	bool pragmaOnceUsed(false);
	while (std::getline(tokenStream, line, '\n'))
	{
		tokenize(line, tokens);
		GfVector<GfString>::const_iterator tokenIter = tokens.begin();

		if (tokens.size() == 0) 
		{
			continue;
		}

		if (*tokenIter == "#") 
		{
			tokenIter++;
		}

		bool pendingTokens = tokenIter != tokens.end();
		bool isInclude = pendingTokens && (*tokenIter).find("include") != GfString::npos;
		bool isPragma = pendingTokens && (*tokenIter).find("pragma") != GfString::npos;
		bool isVersion = pendingTokens && (*tokenIter).find("version") != GfString::npos;
		if (isInclude) 
		{
			outErrors = "[PARSE]: Include statement not implemented/n";
			return false;
		}
		else if (isPragma) 
		{
			tokenIter++;
			if ((*tokenIter).find("once") != GfString::npos) 
			{
				if (!pragmaOnceUsed) 
				{
					pragmaOnceUsed = true;
					addUniqueIncludeHeader(filename, outSrc);
				}
			}
			else if ((*tokenIter).find("mutator") != GfString::npos)
			{
				tokenIter++;
				if (tokenIter == tokens.end()) 
				{
					outErrors = "[PARSE]: Invalid syntax for mutator. Use syntax '#pragma mutator <NAME>'/n";
					return false;
				}
				outSerializer.addMutator(*tokenIter);
			}
			else 
			{
				auto entry = s_hashToShaderStage.find(GfHash::compute(*tokenIter)); 
				if (entry != s_hashToShaderStage.end()) 
				{
					outSerializer.enableStage(entry->second);
				}
			}
		}
		else if (!isVersion)
		{
			outSrc += line + "\n";
		}
	}

	if (pragmaOnceUsed) 
	{
		outSrc += "\n#endif // Include guard";
	}
	return true;
}

GfVector<GfShaderSerializer::MutatorHash> GfShaderCompiler::uberize(const GfShaderSerializer& serializer)
{
	GfVector<GfShaderSerializer::MutatorHash> hashes;
	const u32 hashCount = 1 << serializer.m_mutatorCount;
	hashes.reserve(hashCount);
	// Assuming all mutators are boolean so we do base-2 combinatorial
	// N-bit set to one means the mutator keyword is active, o means disabled
	for (GfShaderSerializer::MutatorHash hash = 0; hash < hashCount; ++hash) 
	{
		hashes.push_back(hash);
	}
	return std::move(hashes);
}

bool GfShaderCompiler::compileVariant(
	const GfString& parsedSrc, 
	const GfShaderSerializer& serializer, 
	const GfShaderSerializer::MutatorHash hash,
	GfShaderSerializer::ShaderVariant* variant, 
	GfVector<GfShaderSerializer::ShaderBytecode>* bytecodeCache, 
	GfVector<u32>* bytecodeSizes,
	GfUMap<u64, s32>* hashToIdx,
	GfString& outErrors,
	GfString& outWarnings)
{
	// This is the final version of the source before pre-processing
	GfString src = serializer.mutatorHashToDefines(hash) + parsedSrc;
	const EShMessages messages = EShMessages(EShMsgSpvRules | EShMsgVulkanRules);
	const glslang::EShTargetLanguageVersion langVersion = glslang::EShTargetSpv_1_3;

	for (u32 i = 0; i < EShaderStage::Count; ++i) 
	{
		EShaderStage::Type shaderStage = static_cast<EShaderStage::Type>(i);
		if (serializer.getStageEnabled(shaderStage)) 
		{
			GfString stageSrc = addShaderStageHeader(src, shaderStage);

			glslang::TShader shader(s_shaderStageToGLSLangStage[shaderStage]);
			shader.setEnvTarget(glslang::EShTargetSpv, langVersion);
			const char* srcArray[] = {stageSrc.c_str()};
			shader.setStrings(srcArray, 1);		
			
			// Parse (preprocess)
			if(!shader.parse(&s_glslangLimits, 100, false, messages))
			{
				outErrors += GfString("[PARSE ERROR] ") + shader.getInfoLog();
				return false;
			}

			// Gen intermediate code
			glslang::TProgram program;
			program.addShader(&shader);

			if(!program.link(messages))
			{
				outErrors += GfString("[LINK ERROR] ") + program.getInfoLog();
				return false;
			}

			// Gen the Spir-V
			glslang::SpvOptions spvOptions;
			spvOptions.optimizeSize = true;
			spvOptions.disableOptimizer = false;
			GfVector<u32> glslangSpirv;
			glslang::GlslangToSpv(*program.getIntermediate(s_shaderStageToGLSLangStage[shaderStage]), glslangSpirv, &spvOptions);

			GfUniquePtr<u32[]> spirv = GfUniquePtr<u32[]>(new u32 [glslangSpirv.size()]);
			memcpy(spirv.get(), &glslangSpirv[0], glslangSpirv.size() * sizeof(u32));

			u32 spirvSize = static_cast<u32>(glslangSpirv.size() * sizeof(u32));
			u32 hash = GfHash::compute(spirv.get(), spirvSize);
			auto entry = hashToIdx->find(hash);
			if (entry != hashToIdx->end()) 
			{
				variant->setVariantShaderBytecode(shaderStage, entry->second);
			}
			else 
			{
				// Bytecodes will be added to the serializer's cache in the same order so byteCodeIdx obtained here is the final valid index
				s32 byteCodeIdx = static_cast<s32>(bytecodeCache->size());
				variant->setVariantShaderBytecode(shaderStage, byteCodeIdx);
				hashToIdx->emplace(hash, byteCodeIdx);
				bytecodeSizes->push_back(spirvSize);
				bytecodeCache->push_back(std::move(spirv));
			}
		}
	}
	return true;
}

bool GfShaderCompiler::reflectVariant(GfShaderSerializer& serializer, GfShaderSerializer::ShaderVariant* variant, GfString& outErrors)
{
	// TODO
	GfArray<GfArray<GfDescriptorBindingSlot, s_MAX_BINDINGS_PER_SET>, s_MAX_DESCRIPTOR_SETS> uniforms;
	GfArray<u16, s_MAX_DESCRIPTOR_SETS> usedBindings;
	usedBindings.fill(0);
	for (u32 set=0; set<s_MAX_DESCRIPTOR_SETS; ++set) 
	{
		for (u32 binding=0; binding<s_MAX_BINDINGS_PER_SET; ++binding) 
		{
			uniforms[set][binding].m_descriptorType = EParamaterSlotType::Invalid;
			uniforms[set][binding].m_stageFlags = 0;
			uniforms[set][binding].m_arraySize = 0;
		}
	}

	auto addDescriptorsOfType = [&](EShaderStage::Type stage, EParamaterSlotType::Type type, 
		const spirv_cross::Compiler& compiler, const spirv_cross::SmallVector<spirv_cross::Resource>& resources) -> bool
	{
		for (auto &resource : resources)
		{
			u32 set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
			u32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			
			const spirv_cross::SPIRType& typeInfo = compiler.get_type(resource.type_id);
			u32 arraySize = 1;
			if(typeInfo.array.size() != 0)
			{
				arraySize = typeInfo.array[0];
				if (typeInfo.array.size() != 1) 
				{
					outErrors += GfString("[REFLECTION ERROR] Only 1D Arrays supported.\n");
					return false;
				}
			}

			// Check if this binding is in-use
			if (set >= s_MAX_DESCRIPTOR_SETS) 
			{
				outErrors += GfString("[REFLECTION ERROR] Invalid descriptor set.\n");
				return false;
			}

			u32 bindIdx = 0;
			for (bindIdx=0; bindIdx<s_MAX_BINDINGS_PER_SET; ++bindIdx) 
			{
				if (usedBindings[set] == bindIdx || uniforms[set][bindIdx].m_bindingSlot == binding) 
				{
					break;
				}
			}

			GfDescriptorBindingSlot& bindEntry = uniforms[set][bindIdx];
			if (bindIdx < s_MAX_DESCRIPTOR_SETS) 
			{
				if ( usedBindings[set] == bindIdx) 
				{
					bindEntry.m_descriptorType = type;
					bindEntry.m_bindingSlot = binding;
					bindEntry.m_arraySize = arraySize;
					bindEntry.m_stageFlags = (1<<stage);
					usedBindings[set]++;
				}
				else if (bindEntry.m_descriptorType == type) 
				{
					bindEntry.m_stageFlags |= (1<<stage);
				}
				else 
				{
					outErrors += GfString("[REFLECTION ERROR] Trying to bind multiple resources of different type to same binding slot.\n");
					return false;
				}
			}
			else 
			{
				outErrors += GfString("[REFLECTION ERROR] Exceding the number of bindings per set.\n");
				return false;
			}
		}
		return true;
	};

	for (u32 i = 0; i < EShaderStage::Count; ++i)
	{
		s32 idx = variant->getBytecodeIndexForStage(static_cast<EShaderStage::Type>(i));
		if (idx >= 0) 
		{
			const u32* bytecode = serializer.m_bytecodeCache[idx].get();
			const u32 sizeInBytes = serializer.m_bytecodeSizes[idx];
			spirv_cross::Compiler compiler(bytecode, sizeInBytes / (sizeof(u32)));
			spirv_cross::ShaderResources resources = compiler.get_shader_resources();
			if(!addDescriptorsOfType(static_cast<EShaderStage::Type>(i), EParamaterSlotType::Sampler, compiler, resources.separate_samplers)) return false;
			if(!addDescriptorsOfType(static_cast<EShaderStage::Type>(i), EParamaterSlotType::UniformBuffer, compiler, resources.uniform_buffers)) return false;
			if(!addDescriptorsOfType(static_cast<EShaderStage::Type>(i), EParamaterSlotType::Texture, compiler, resources.separate_images)) return false;
			if(!addDescriptorsOfType(static_cast<EShaderStage::Type>(i), EParamaterSlotType::SamplerTexture, compiler, resources.sampled_images)) return false;
			if(!addDescriptorsOfType(static_cast<EShaderStage::Type>(i), EParamaterSlotType::StorageBuffer, compiler, resources.storage_buffers)) return false;
			if(!addDescriptorsOfType(static_cast<EShaderStage::Type>(i), EParamaterSlotType::StorageImage, compiler, resources.storage_images)) return false;
		}
	}

	// Add data to serializer and set offsets to variant
	for (u32 set = 0; set < s_MAX_DESCRIPTOR_SETS; ++set) 
	{
		if (usedBindings[set] > 0)
		{
			std::sort(&uniforms[set][0], (&uniforms[set][0]) + usedBindings[set], [=](const GfDescriptorBindingSlot& l, const GfDescriptorBindingSlot& r)
			{
				return l.m_bindingSlot < r.m_bindingSlot;
			});

			s16 offset = static_cast<s16>(serializer.addBindingsArray(&uniforms[set][0], usedBindings[set]));
			s16 count = static_cast<s16>(usedBindings[set]);
			variant->setDescriptorSetLayoutRange(set, offset, count);
		}
	}

	return true;
}

void GfShaderDeserializer::deserialize(GfUniquePtr<u8[]> blob)
{
}

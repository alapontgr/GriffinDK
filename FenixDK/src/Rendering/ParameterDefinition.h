#pragma once
#include "Utilities\types.h"
#include "Utilities\Name.h"
#include "BufferUtils.h"

#include <vector>

namespace fdk
{
namespace Rendering
{

	enum EShaderParameterType : u32
	{
		Type_ConstantBuffer = 0,
		Type_Texture2D
	};

	enum EShaderStageFlag : u32
	{
		Vertex = 1 << 0,
		TesellationControl = 1 << 1,
		TesellationEvaluation = 1 << 2,
		Geometry = 1 << 3,
		Fragment = 1 << 4,
		Compute = 1 << 5,
		Graphics = Vertex |
							 TesellationEvaluation |
							 TesellationEvaluation |
							 Geometry |
							 Fragment,
    All = Graphics | Compute,
    StageCount = 6
	};

  using ShaderStageMask = u32;

	struct ParameterDefinition
	{
		Utilities::Name m_paramName;
		EShaderParameterType m_paramType;
    ShaderStageMask m_stages;
		u32 m_bindingSlot;
		u32 m_offset;
		u32 m_size;
		u32 m_alignment;
	};

  struct ParameterSlot 
  {
    u32 m_offset;
    u32 m_size;
  };

	using ParameterCollection = std::vector<ParameterDefinition>;
}
}
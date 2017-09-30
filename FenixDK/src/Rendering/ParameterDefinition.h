#pragma once
#include "Utilities\types.h"
#include "Utilities\Name.h"
#include "BufferUtils.h"

namespace fdk
{
  namespace Rendering 
  {

    enum EShaderParameterType : u32 
    {
      Type_ConstantBuffer = 0,
      Type_Texture2D
    };

    struct ParameterDefinition 
    {
      Utilities::Name m_paramName;
      EShaderParameterType m_paramType;
      u32 m_bindingSlot;
      u32 m_offset;
      u32 m_size;
      u32 m_alignment;
    };
  }
}
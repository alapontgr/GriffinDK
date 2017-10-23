#pragma once
#include "Utilities\types.h"

namespace fdk
{
  namespace Rendering 
  {
    class MaterialParameterSet;

    class MaterialBase
    {
    public:

      static constexpr u32 kMaxParameterSetLayouts = 3;

      MaterialBase();

    protected:
    
      u32 m_setLayoutCount;
      const MaterialParameterSet* m_parameterSetLayouts[kMaxParameterSetLayouts];
    };

  }
}
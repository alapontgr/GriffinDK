#pragma once
#include "ParameterDefinition.h"
#include "..\MaterialUtils.h"

namespace fdk
{
  namespace Rendering 
  {

    
    class MaterialParameterSetBase
    {
    public:

      u32 parameter_count() const { return m_parameters.size(); }

      const ParameterCollection& parameters() const { return m_parameters; }

      u32 set_count() const { m_setCount; }

    protected:

      u32 m_setCount;
      ParameterCollection m_parameters;
    };
  }
}
#pragma once
#include "ParameterDefinition.h"

namespace fdk
{
  namespace Rendering 
  {

    
    class MaterialParameterSetBase
    {
    public:

      u32 parameter_count() const { return m_parameters.size(); }

      const ParameterCollection& parameters() const { return m_parameters; }

    protected:

      ParameterCollection m_parameters;
    };
  }
}
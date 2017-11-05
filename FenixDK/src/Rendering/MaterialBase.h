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

      MaterialBase();

    protected:
    
      const MaterialParameterSet* m_pParameterSetLayout;
    };

  }
}
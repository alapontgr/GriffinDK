#include "MaterialBase.h"

namespace fdk
{
  namespace Rendering 
  {
  
    MaterialBase::MaterialBase()
      : m_setLayoutCount{ 0 }
      , m_parameterSetLayouts{nullptr, nullptr, nullptr}
    {
    }

  }
}


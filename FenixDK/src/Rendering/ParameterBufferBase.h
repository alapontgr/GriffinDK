#pragma once

#include "Memory\memallocator.h"

namespace fdk
{
  namespace Rendering 
  {
    class MaterialParameterSet;

    class ParameterBufferBase 
    {
    public:

    protected:

      Memory::mem_ptr_t m_pParametersData;
      const MaterialParameterSet* m_pParameterSetLayout;
      u32 m_bindingSlot;
    };
  }
}
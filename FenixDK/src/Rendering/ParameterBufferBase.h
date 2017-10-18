#pragma once

#include "Memory\memallocator.h"

namespace fdk
{
  namespace Rendering 
  {
    class MaterialParameterSet;
    class ParameterBufferGroup;

    class ParameterBufferBase 
    {
    public:
      
    protected:

      Memory::mem_ptr_t m_pParametersData;
      const MaterialParameterSet* m_pParameterSetLayout;
      const ParameterBufferGroup* m_pAllocationGroup;
      u32 m_bindingSlot;
    };
  }
}
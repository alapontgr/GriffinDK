#pragma once

#include "Memory\memallocator.h"

namespace fdk
{
  namespace Rendering 
  {
    class MaterialParameterSet;
    class ParameterBufferGroup;
    struct ParameterDefinition;

    class ParameterBufferBase 
    {
    public:
      
    protected:

      Memory::mem_ptr_t get_parameter_data(const ParameterDefinition& rParameter);

      Memory::mem_ptr_t m_pParametersData;
      const MaterialParameterSet* m_pParameterSetLayout;
      const ParameterBufferGroup* m_pAllocationGroup;
      u32 m_bindingSlot;
    };

  }
}
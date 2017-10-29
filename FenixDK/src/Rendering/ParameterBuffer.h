#pragma once

#include "Utilities/platform.h"
#include INCLUDE_IMPLEMENTATION(ParameterBuffer)


namespace fdk
{
  namespace Rendering 
  {
    struct ParameterSlot;
    class ConstantBuffer;

    class ParameterBuffer : public IMPLEMENTATION(ParameterBuffer)
    {
    public:

      using BaseT = IMPLEMENTATION(ParameterBuffer);

      ParameterBuffer();

      void create(
        const MaterialParameterSet& rParameterSetLayout,
        const ParameterBufferGroup& rAllocationGroup,
        Memory::MemAllocator& rAllocator);

      void release();
      
      void update(Memory::MemAllocator& rTmpAllocator);
     
      void set_parameter(const ParameterSlot& rSlot, const ConstantBuffer* pConstantBuffer);

    };

  }
}

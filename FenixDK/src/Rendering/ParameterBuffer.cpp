#include "ParameterBuffer.h"
#include "RenderInterface.h"
#include "MaterialParameterSet.h"
#include "ParameterBufferGroup.h"

namespace fdk
{
  namespace Rendering 
  {
    
    ParameterBuffer::ParameterBuffer()
    {

    }

    void ParameterBuffer::create(
      const MaterialParameterSet& rParameterSetLayout,
      const ParameterBufferGroup& rAllocationGroup,
      Memory::MemAllocator& rAllocator)
    {
      m_pParameterSetLayout = &rParameterSetLayout;
      u32 size = m_pParameterSetLayout->total_size();
      m_pParametersData = rAllocator.allocate(size, 16);
      BaseT::create();
    }

    void ParameterBuffer::update()
    {
      FDASSERT(m_pParameterSetLayout, "Parameter layout has not been set");
      BaseT::update();
    }

    void ParameterBuffer::bind()
    {
      BaseT::bind();
    }

    void ParameterBuffer::release()
    {
      BaseT::release();
    }
  }
}


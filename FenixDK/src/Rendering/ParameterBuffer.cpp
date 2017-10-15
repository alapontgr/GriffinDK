#include "ParameterBuffer.h"
#include "RenderInterface.h"
#include "MaterialParameterSet.h"

namespace fdk
{
  namespace Rendering 
  {
    
    ParameterBuffer::ParameterBuffer()
    {

    }

    void ParameterBuffer::create(
      const MaterialParameterSet& rParameterSetLayout,
      RenderInterface& rRI, 
      Memory::MemAllocator& rAllocator)
    {
      m_pParameterSetLayout = &rParameterSetLayout;
      u32 size = m_pParameterSetLayout->total_size();
      m_pParametersData = rAllocator.allocate(size, 16);
      BaseT::create(rRI, *m_pParameterSetLayout, m_bindingSlot);
    }

    void ParameterBuffer::update()
    {
      FDASSERT(m_pParameterSetLayout, "Parameter layout has not been set");
      BaseT::update(*m_pParameterSetLayout, m_pParametersData);
    }

    void ParameterBuffer::bind()
    {
      BaseT::bind(*m_pParameterSetLayout);
    }

    void ParameterBuffer::release(RenderInterface& rRI)
    {
      BaseT::release(rRI);
    }
  }
}


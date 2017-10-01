#include "ParameterBuffer.h"

namespace fdk
{
  namespace Rendering 
  {
    
    ParameterBuffer::ParameterBuffer()
    {

    }

    void ParameterBuffer::create(RenderInterface& rRI)
    {
      FDASSERT(m_pParameterSetLayout, "Parameter layout has not been set");
    }

    void ParameterBuffer::update()
    {

    }

    void ParameterBuffer::release()
    {

    }

  }
}


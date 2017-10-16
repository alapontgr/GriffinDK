#include "ParameterBufferGroup.h"

namespace fdk
{
  namespace Rendering 
  {   

    ParameterBufferGroup::ParameterBufferGroup()
    {

    }

    void ParameterBufferGroup::create(const u32 maxElements)
    {
      FDASSERT(m_maxElementCount == 0, "This group has already been initialized");
      m_maxElementCount = maxElements;
    }
  }
}


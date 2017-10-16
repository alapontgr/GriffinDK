#include "ParameterBufferGroupBase.h"
#include "ParameterBuffer.h"

namespace fdk
{
  namespace Rendering 
  {
    
    ParameterBufferGroupBase::ParameterBufferGroupBase()
      : m_currentAllocatedSets{ 0 }
      , m_maxElementCount{ 0 } {}

    void ParameterBufferGroupBase::add_element(const ParameterBuffer& rParameterBuffer)
    {
      FDASSERT(!is_full(), "Cannon add an element to a group that is already full");
      m_currentAllocatedSets++;
    }

  }
}


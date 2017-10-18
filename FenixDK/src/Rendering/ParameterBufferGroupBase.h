#pragma once
#include "Utilities\types.h"

namespace fdk
{
  namespace Rendering 
  {
    class ParameterBuffer;
    class ParameterSet;

    class ParameterBufferGroupBase
    {
    public:

      ParameterBufferGroupBase();

      bool is_full() const { return m_currentAllocatedSets >= m_maxElementCount; }

      void add_element(const ParameterBuffer& rParameterBuffer);

    protected:
      
      u32 m_maxElementCount;
      u32 m_currentAllocatedSets;
    };

  }
}
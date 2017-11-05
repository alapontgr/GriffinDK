#pragma once

#include "Memory/memallocator.h"
#include "BufferUtils.h"

namespace fdk
{
  namespace Rendering 
  {
    class BufferBase 
    {    
    public:

      BufferBase();

    protected:

      Memory::MemBlock m_data;
      BufferDesc m_desc;
    };

  }
}
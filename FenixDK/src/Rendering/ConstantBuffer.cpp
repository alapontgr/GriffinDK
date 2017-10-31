#include "ConstantBuffer.h"
#include "Buffer.h"

namespace fdk
{
  namespace Rendering 
  {
    
    ConstantBuffer::ConstantBuffer()
    {

    }

    void ConstantBuffer::initialize(const BufferChunk& rBufferChunk)
    {
      m_gpuData = rBufferChunk;
    }

  }
}


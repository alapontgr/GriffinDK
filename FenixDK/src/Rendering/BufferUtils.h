#pragma once
#include "Utilities\types.h"
#include "Containers\Mask.h"
#include "Memory\MemUtils.h"

namespace fdk
{
  namespace Rendering 
  {
    class Buffer;

    enum EBufferUsage : u32
    {
      InvalidUsage = 0,
      Transfer_Src = 1 << 0,
      Transfer_Dst = 1 << 1,
      Uniform_Texel = 1 << 2,
      Storage_Texel = 1 << 3,
      Uniform_Buffer = 1 << 4,
      Storage_Buffer = 1 << 5,
      Index_Buffer = 1 << 6,
      Vertex_Buffer = 1 << 7,
      Indirect_Buffer = 1 << 8
    };
    using UsageFlags = Containers::Mask<u32>;

    enum EMemoryProperties : u32
    {
      GPU_Local = 1 << 0,
      CPU_Visible = 1 << 1,
      CPU_GPU_Coherent = 1 << 2,
      CPU_Cached = 1 << 3,
      Lazy_Allocated = 1 << 4
    };
    using MemoryProperties = Containers::Mask<u32>;

    struct BufferDesc
    {
      EBufferUsage m_currentUsage;
      UsageFlags m_bufferUsage;
      MemoryProperties m_memoryProperties;
      Memory::mem_size_t m_size;
      Memory::mem_size_t m_alignment;
    };

    struct BufferChunk 
    {
      Buffer* m_pBuffer;
      u32 m_dataOffset;
      u32 m_dataSize;
    };
  }
}
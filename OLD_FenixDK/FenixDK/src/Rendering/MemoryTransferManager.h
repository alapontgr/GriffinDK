#pragma once
#include "Utilities\SingletonHolder.h"
#include "BufferUtils.h"

namespace fdk
{
  namespace Rendering 
  {
    class MemoryTransferManager 
    {
    public:

      MemoryTransferManager();

      Rendering::BufferChunk allocate_vertex_data(const f32* pVertices, const u32 vertexCount, const u32 vertexSize);

      Rendering::BufferChunk allocate_index_data(const u16* pIndices, const u32 indexCount, const u32 indexSize);

    private:
    };

    using GMemoryTransgerManager = Utilities::SingletonHolder<MemoryTransferManager>;
  }
}
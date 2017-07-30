#include "Mesh.h"

namespace fdk
{
namespace Framework
{

  Mesh::Mesh()
  {

  }

  void Mesh::init(
    const f32* pVertices, const u32 vertexCount, 
    const u16* pIndices, const u32 indexCount,
    Memory::MemAllocator& rAllocator)
	{
    if (!m_pVertexBuffer) 
    {
      m_pVertexBuffer = Rendering::Buffer::create(rAllocator);
    }
    if (!m_pIndexBuffer) 
    {
      m_pIndexBuffer = Rendering::Buffer::create(rAllocator);
    }

    Rendering::Buffer::BufferDesc vertexDesc;
    vertexDesc.m_bufferUsage = Rendering::Buffer::UsageFlags(
				Rendering::Buffer::Transfer_Dst |
				Rendering::Buffer::Vertex_Buffer);
    vertexDesc.m_memoryProperties = Rendering::Buffer::MemoryProperties(Rendering::Buffer::GPU_Local);
    vertexDesc.m_size = vertexCount * sizeof(f32);
    vertexDesc.m_alignment = alignof(f32);
    m_pVertexBuffer->init(vertexDesc, rAllocator, (Memory::mem_ptr_t)pVertices);

    Rendering::Buffer::BufferDesc indexDesc;
    indexDesc.m_bufferUsage = Rendering::Buffer::UsageFlags(
      Rendering::Buffer::Transfer_Dst |
      Rendering::Buffer::Index_Buffer);
    indexDesc.m_memoryProperties = Rendering::Buffer::MemoryProperties(Rendering::Buffer::GPU_Local);
    indexDesc.m_size = vertexCount * sizeof(u16);
    indexDesc.m_alignment = alignof(u16);
    m_pIndexBuffer->init(indexDesc, rAllocator, (Memory::mem_ptr_t)pVertices);
  }
}
}

#include "Mesh.h"

namespace fdk
{
namespace Framework
{

  Mesh::Mesh() 
  {

  }

  void Mesh::init(
    const f32* pVertices, const u32 vertexCount, const u32 vertexSize,
    const u16* pIndices, const u32 indexCount, const u32 indexSize,
    const Rendering::BufferChunk& rVertexData,
    const Rendering::BufferChunk& rIndexData)
	{
    m_vertexBufferData = m_indexBufferData;
    m_indexBufferData = m_indexBufferData;
  }
}
}

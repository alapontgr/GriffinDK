#include "Mesh.h"

namespace fdk
{
namespace Framework
{

  Mesh::Mesh() 
    : m_pVertexData{nullptr}
    , m_pIndexData{nullptr}
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

  void Mesh::init_data(const f32* pVertices, const u32 vertexCount, const u32 vertexSize, const u16* pIndices, const u32 indexCount, const u32 indexSize)
  {
    m_pVertexData = pVertices;
    m_pIndexData = pIndices;
  }

}
}

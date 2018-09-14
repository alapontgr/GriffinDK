#pragma once
#include "Rendering\BufferUtils.h"

namespace fdk
{
namespace Framework
{
	class Mesh
	{
	public:
		Mesh();

    void init(
      const f32* pVertices, const u32 vertexCount, const u32 vertexSize,
      const u16* pIndices, const u32 indexCount, const u32 indexSize,
      const Rendering::BufferChunk& rVertexData,
      const Rendering::BufferChunk& rIndexData);

    void init_data(
      const f32* pVertices, const u32 vertexCount, const u32 vertexSize,
      const u16* pIndices, const u32 indexCount, const u32 indexSize);

    Rendering::BufferChunk vertex_chunk() const { return m_vertexBufferData; }

    Rendering::BufferChunk index_chunk() const { return m_indexBufferData; }

    const f32* vertex_data() const { return m_pVertexData; }

    const u16* index_data() const { return m_pIndexData; }

	private:

    Rendering::BufferChunk m_vertexBufferData; // Range of a buffer where the vertex data of the mesh is
    Rendering::BufferChunk m_indexBufferData; // Range of a buffer where the index data is for the mesh
    const f32* m_pVertexData;
    const u16* m_pIndexData;
  };

}
}
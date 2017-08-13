#pragma once
#include "Rendering\Buffer.h"

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
      Memory::MemAllocator& rAllocator);

		Rendering::Buffer* vertex_buffer() { return m_pVertexBuffer; }

		Rendering::Buffer* index_buffer() { return m_pIndexBuffer; }

	private:
		Rendering::Buffer* m_pVertexBuffer;
		Rendering::Buffer* m_pIndexBuffer;
	};

}
}
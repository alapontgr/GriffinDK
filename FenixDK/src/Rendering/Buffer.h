#pragma once

#include "Memory/memallocator.h"
#include "Containers/Mask.h"

#include "Utilities/platform.h"
#ifdef FENIX_VK_IMPL
#include "VK_Buffer.h"
#endif

namespace fdk
{
namespace Rendering
{
  struct BufferRange
  {
    u32 m_offset;
    u32 m_size;
  };

	class Buffer : public IMPLEMENTATION(Buffer)
	{
	public:

    using BaseT = IMPLEMENTATION(Buffer);

		enum EBufferUsage : u32
		{
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
      UsageFlags m_bufferUsage;
      MemoryProperties m_memoryProperties;
      Memory::mem_size_t m_size;
      Memory::mem_size_t m_alignment;
    };

    Buffer();

		static Buffer* create(Memory::MemAllocator& rAllocator);

		void init(
				const BufferDesc& rDesc,
				Memory::MemAllocator& rAllocator,
				Memory::mem_ptr_t pInitialData = nullptr);

		void release(Memory::MemAllocator& rAllocator);

		Memory::mem_ptr_t data_mutable();

		template <typename T>
		T* data_mutable_as()
		{
			FDASSERT(Memory::is_aligned(m_data.memory(), alignof(T)), "The data is not aligned for this type");
			return reinterpret_cast<T*>(m_data.memory());
		}

    Memory::mem_size_t size() const { return m_desc.m_size; };

		UsageFlags usage() const { return m_desc.m_bufferUsage; }

		MemoryProperties mem_properties() const { return m_desc.m_memoryProperties; }

	private:
		Buffer& operator=(const Buffer&) = delete;
		Buffer(const Buffer&) = delete;

	protected:

		Memory::MemBlock m_data;
    BufferDesc m_desc;
	};
}
}
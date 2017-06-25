#pragma once

#include "Memory/memallocator.h"
#include "Containers/Mask.h"

namespace fdk
{
namespace Rendering
{
	class Buffer
	{
	public:
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

		enum EMemoryProperties
		{
			GPU_Local = 1 << 0,
			CPU_Visible = 1 << 1,
			CPU_GPU_Coherent = 1 << 2,
			CPU_Cached = 1 << 3,
			Lazy_Allocated = 1 << 4
		};
		using MemoryProperties = Containers::Mask<u32>;

		static Buffer* create(Memory::MemAllocator& rAllocator);

		void init(
				const UsageFlags& bufferUsage,
				const MemoryProperties& memoryProperties,
				Memory::MemAllocator& rAllocator,
				Memory::mem_size_t size,
				Memory::mem_size_t alignment,
				Memory::mem_ptr_t pInitialData = nullptr);

		void release(Memory::MemAllocator& rAllocator);

		Memory::mem_ptr_t data_mutable();

		template <typename T>
		T* data_mutable_as()
		{
			FDK_ASSERT(Memory::is_aligned(m_data.memory(), alignof(T)), "The data is not aligned for this type");
			return reinterpret_cast<T*>(m_data.memory());
		}

		Memory::mem_size_t size() const { return m_data.size(); };

		UsageFlags usage() const { return m_usage; }

    MemoryProperties mem_properties() const { return m_memProperties; }

	private:
		Buffer& operator=(const Buffer&) = delete;
		Buffer(const Buffer&) = delete;

	protected:
		Buffer();

		Memory::MemBlock m_data;
		UsageFlags m_usage;
    MemoryProperties m_memProperties;
	};
}
}
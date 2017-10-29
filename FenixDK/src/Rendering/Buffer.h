#pragma once

#include "Memory/memallocator.h"
#include "BufferUtils.h"

#include "Utilities/platform.h"
#include INCLUDE_IMPLEMENTATION(Buffer)

namespace fdk
{
namespace Rendering
{
  class Buffer : public IMPLEMENTATION(Buffer)
	{
	public:

    using BaseT = IMPLEMENTATION(Buffer);

    Buffer();

		static Buffer* create(Memory::MemAllocator& rAllocator);

		void init(
				const BufferDesc& rDesc,
				Memory::MemAllocator& rAllocator,
				Memory::mem_ptr_t pInitialData = nullptr);

		void release(Memory::MemAllocator& rAllocator);

    void copy_range_from(const Buffer& rFrom, const u32 fromOffset, const u32 offset, const u32 size, CommandBuffer& rCmdBuffer);

		Memory::mem_ptr_t data_mutable();

		template <typename T>
		T* data_mutable_as()
		{
			FDASSERT(Memory::is_aligned(m_data.memory(), alignof(T)), "The data is not aligned for this type");
			return reinterpret_cast<T*>(m_data.memory());
		}

    Memory::mem_size_t size() const { return m_desc.m_size; };

    EBufferUsage type() const { m_desc.m_currentUsage; }

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
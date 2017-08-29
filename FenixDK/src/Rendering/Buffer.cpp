#include "Buffer.h"

namespace fdk
{
namespace Rendering
{

  Buffer::Buffer() : m_desc{ BufferDesc{InvalidUsage,0,0,0,1} }
	{
	}

	void Buffer::init(
			const BufferDesc& rDesc,
			Memory::MemAllocator& rAllocator,
			Memory::mem_ptr_t pInitialData)
	{
		FDASSERT(this->size() == 0, "A buffer must be freed before allocating a new chunk of memory");
		m_desc = rDesc;
		if (pInitialData)
		{
			m_data.allocate(rAllocator, rDesc.m_size, rDesc.m_alignment);
			memcpy(m_data.memory(), pInitialData, rDesc.m_size);
		}
	}

	void Buffer::release(Memory::MemAllocator& rAllocator)
	{
		if (m_data.size() > 0)
		{
			m_data.release(rAllocator);
		}
	}

  void Buffer::copy_range_from(
    const Buffer& rFrom, 
    const u32 fromOffset, 
    const u32 offset, 
    const u32 size, 
    CommandBuffer& rCmdBuffer)
  {
    BaseT::copy_buffer_range(rFrom, m_desc, fromOffset, offset, size, rCmdBuffer);
  }

  Memory::mem_ptr_t Buffer::data_mutable()
	{
		return m_data.memory();
	}

	Buffer* Buffer::create(Memory::MemAllocator& rAllocator)
	{
		return rAllocator.create<Buffer>();
	}
}
}

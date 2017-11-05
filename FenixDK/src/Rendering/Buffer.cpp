#include "Buffer.h"

namespace fdk
{
namespace Rendering
{

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
    BaseT::copy_buffer_range(rFrom, fromOffset, offset, size, rCmdBuffer);
  }

  void Buffer::update_region(const BufferRange& rRange, const Memory::mem_ptr_t pData, CommandBuffer& rCmdBuffer)
  {
    BaseT::update_region(rRange, pData, rCmdBuffer);
  }

  Memory::mem_ptr_t Buffer::data_mutable()
	{
		return m_data.memory();
	}

  Buffer::Buffer()
  {
  }

  Buffer* Buffer::create(Memory::MemAllocator& rAllocator)
	{
		return rAllocator.create<Buffer>();
	}

}
}

#pragma once
#include "MemAllocator.h"

namespace fdk
{
  namespace Memory 
  {
    class StackAllocator : public MemAllocator
    {
    public:
      StackAllocator() {}

      virtual ~StackAllocator() {}

      void init(MemAllocator& rAllocator, const mem_size_t size, const mem_align_t alignment) 
      {
        FDK_ASSERT((m_block.memory() == nullptr), "Stack has already been initialise");
        m_block.allocate(rAllocator, size, alignment);
        reset();
      }

      void free_memory(MemAllocator& rAllocator) 
      {
        m_block.release(rAllocator);
      }

      virtual mem_ptr_t allocate(const mem_size_t size, const mem_align_t alignment) 
      {
        mem_size_t offset = get_alignment_offset(m_pivot, alignment);
        FDK_ASSERT(((offset + size) <= space_left()), "Not enough memory to allocate that resource");
        m_pivot += offset;
        mem_ptr_t pData = m_pivot;
        m_pivot += size;
        return pData;
      }

      virtual void release(mem_ptr_t pMemory) 
      {
        UNUSED(pMemory);
      }

      void reset() 
      {
        m_pivot = m_block.memory();
      }

      mem_size_t space_left() const 
      {
        mem_ptr_t pEnd = m_block.c_memory() + m_block.size();
        return mem_distance(m_pivot, pEnd);
      }

      mem_size_t size() const { return mem_distance(m_block.c_memory(), m_pivot); }

      mem_size_t capacity() const { return m_block.size(); }

    private:
      MemBlock m_block;
      mem_ptr_t m_pivot;
    };
  }
}
#pragma once
#include "Utilities\types.h"
#include "MemUtils.h"

namespace fdk
{
  namespace Memory 
  {
    class MemAllocator 
    {
    public:
      virtual ~MemAllocator() {}
      
      virtual mem_ptr_t allocate(const mem_size_t size, const mem_align_t alignment) = 0;

      virtual void release(mem_ptr_t pMemory) = 0;

      template <typename T, typename... Args>
      T* create(Args&&... args) 
      {
        auto pData = allocate(sizeof(T), alignof(T));
        FDASSERT(pData, "Failed to allocate memory");
        FDASSERT(is_aligned(pData, alignof(T)), "Bad alignment");
        return new (pData) T(args...);
      }

      template <typename T>
      void destroy(T* pObj) 
      {
        pObj::~T();
        release(reinterpret_cast<mem_ptr_t>(pObj));
      }

      // TODO: Implement unit test
      template <typename T, typename... Args>
      T* create_array(const u32 elementCount, Args&&... args)
      {
        auto pData = allocate(elementCount * sizeof(T), alignof(T));
        FDASSERT(pData, "Failed to allocate memory");
        FDASSERT(is_aligned(pData, alignof(T)), "Bad alignment");
        auto pTData = reinterpret_cast<T*>(pData);
        auto pAux = pTData;
        for (u32 i=0; i<elementCount; ++i) 
        {
          new (pAux) T(args...);
          pAux++;
        }
        return pTData;
      }

      // TODO: Implement unit test
      template <typename T>
      void destroy_array(T* pObj, const u32 elementCount)
      {
        auto pAux = pObj;
        for (u32 i = 0; i < elementCount; ++i)
        {
          pAux::~T();
          pAux++;
        }
        release(reinterpret_cast<mem_ptr_t>(pObj));
      }

    private:
    };

    class MemBlock 
    {
    public:      
      MemBlock(mem_ptr_t data = nullptr, const mem_size_t size = 0, const mem_align_t alignment = 16) :
        m_data{ data }, m_size(size), m_alignment{alignment} {}

      void allocate(MemAllocator& rAllocator, const mem_size_t size, const mem_align_t alignment) 
      {
        FDASSERT(m_data == nullptr, "Trying to allocate without releasing the previous memory");
        m_data = rAllocator.allocate(size, alignment);
        m_size = size;
        m_alignment = alignment;
      }

      void release(MemAllocator& rAllocator) 
      {
        rAllocator.release(m_data);
        m_size = 0;
        m_data = nullptr;
      }

      mem_ptr_t memory() { return m_data; }
 
      const mem_ptr_t c_memory() const { return m_data; }

      mem_size_t size() const { return m_size; }

      mem_align_t alignment() const { return m_size; }

    private:
      mem_ptr_t m_data;
      mem_size_t m_size;
      mem_align_t m_alignment;
    };
  }
}
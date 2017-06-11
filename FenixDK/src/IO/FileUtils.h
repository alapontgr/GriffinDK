#pragma once
#include "Utilities\types.h"
#include "Utilities\platform.h"
#include "Memory\MemUtils.h"

namespace fdk
{
  namespace IO 
  {
    class File 
    {
    public:
      File() {}

      ~File(){}

      Memory::mem_ptr_t load(PATH path, const u32 padding, const u32 alignment);
      
      void release();
      
      Memory::mem_ptr_t data() { return m_data; }

      Memory::mem_size_t size() const { return m_size; }
    
    private:

      Memory::mem_ptr_t m_data = nullptr;
      Memory::mem_size_t m_size = 0;
    };

  }
}
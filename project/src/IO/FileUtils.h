#pragma once
#include "Utilities\types.h"
#include "Utilities\platform.h"

namespace fdk
{
  namespace IO 
  {
    class File 
    {
    public:
      using mem_t = char;
      using size_t = u32;

      File() {}

      ~File(){}

      mem_t* load(PATH path, const u32 padding, const u32 alignment);
      
      void release();
      
      mem_t* data() { return m_data; }

      size_t size() const { return m_size; }
    
    private:

      mem_t* m_data = nullptr;
      size_t m_size = 0;
    };

  }
}
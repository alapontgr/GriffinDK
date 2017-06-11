#include "FileUtils.h"

#include <stdlib.h>
#include <fstream>
#include "Memory\MemUtils.h"

namespace fdk
{
  namespace IO 
  {
    Memory::mem_ptr_t File::load(PATH path, const u32 padding, const u32 alignment)
    {
      std::ifstream fs;
      FDK_ASSERT(path, "Trying to load invalid path");

      fs.open(path, std::ios::binary);
      if (fs.good())
      {
        fs.seekg(0, std::ios::end);
        m_size = static_cast<Memory::mem_size_t>(fs.tellg());
        fs.seekg(0, std::ios::beg);

        // TODO: Replace by allocators
        m_data = Memory::allocate_memory(m_size + padding, alignment);

        fs.read(m_data, m_size);
        if (padding > 0)
        {
          memset(m_data + m_size, 0, padding);
        }

        fs.close();
      }
      return m_data;
    }

    void File::release()
    {
      // TODO: remove once replaced by allocators
      if (m_data) 
      {
        Memory::release_memory(m_data);
        m_data = nullptr;
      }
    }
  }
}


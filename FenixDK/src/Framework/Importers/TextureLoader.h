#pragma once
#include "Utilities\types.h"
#include "Memory\MemAllocator.h"

namespace fdk
{
  namespace Framework 
  {
    namespace Importers 
    {
      Memory::mem_ptr_t load_image(
        Memory::MemAllocator& rAllocator, 
        const char* textureFileName, 
        s32& rOutWidth, 
        s32& rOutHeight, 
        s32& rOutComponentCount,
        s32 desiredChannels = 0);
    }
  }
}
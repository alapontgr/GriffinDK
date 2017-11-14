#include "TextureLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../../../External/stb/stb_image.h"

namespace fdk
{
  namespace Framework 
  {
    namespace Importers 
    {
      Memory::mem_ptr_t load_image(Memory::MemAllocator& rAllocator, const char* textureFileName, s32& rOutWidth, s32& rOutHeight, s32& rOutComponentCount, s32 desiredChannels /*= 0*/)
      {
        s32 w, h, n;
        unsigned char* pData = stbi_load(textureFileName, &w, &h, &n, desiredChannels);

        Memory::mem_size_t totalSize = static_cast<Memory::mem_size_t>(w * h * n);
        auto pOutData = rAllocator.allocate(totalSize, alignof(Memory::mem_t));
        memcpy(pOutData, pData, totalSize);

        stbi_image_free(pData);

        rOutWidth = w;
        rOutHeight = h;
        rOutComponentCount = n;
        return pOutData;
      }

    }
  }
}
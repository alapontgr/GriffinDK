#pragma once

#include "Utilities/platform.h"
#ifdef FENIX_VK_IMPL
#include "Vulkan/VK_Fence.h"
#endif

namespace fdk
{
  namespace Rendering 
  {
    class Fence : public IMPLEMENTATION(Fence)
    {
    public:

      using BaseT = IMPLEMENTATION(Fence);

      Fence() {}

      void wait();
    };

  }
}
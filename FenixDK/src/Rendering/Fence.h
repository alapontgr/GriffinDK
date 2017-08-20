#pragma once

#include "Utilities/platform.h"
#ifdef FENIX_VK_IMPL
#include "VK_Fence.h"
#endif

namespace fdk
{
  namespace Rendering 
  {
    class Fence : public IMPLEMENTATION(Fence)
    {
    public:
      Fence() {}
    };
  }
}
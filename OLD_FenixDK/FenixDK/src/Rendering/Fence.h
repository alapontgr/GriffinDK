#pragma once

#include "Utilities/platform.h"
#include INCLUDE_IMPLEMENTATION(Fence)

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
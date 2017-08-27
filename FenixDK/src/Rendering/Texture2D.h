#pragma once

#include "Utilities/platform.h"
#ifdef FENIX_VK_IMPL
#include "VK_Texture2D.h"
#endif

namespace fdk
{
  namespace Rendering 
  {
    class Texture2D : public IMPLEMENTATION(Texture2D)
    {
    public:

      Texture2D() {}

    };
  }
}
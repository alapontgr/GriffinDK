#include "RenderInterface.h"
#include "RenderDefines.h"

// Implementations
#include "VK_RenderInterface.h"

namespace fdk
{
  namespace Rendering 
  { 
    RenderInterface::RenderInterface() {}
    
    void RenderInterface::test()
    {
      IMPLEMENTATION(RenderInterface, this)->test();
    }    
  }
}


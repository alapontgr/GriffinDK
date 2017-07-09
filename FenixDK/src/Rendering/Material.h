#pragma once
#include "Utilities\types.h"
#include "Containers\Mask.h"

namespace fdk
{
  namespace Rendering 
  {
    class Material 
    {
    
    public:

      Material();

      ~Material();



    private:
      Containers::Mask<u32> m_flags;
    };

  }
}
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

      ~Material();

      static Material* create();

    private:
      
      Material();
      
      Containers::Mask<u32> m_flags;
    };

  }
}
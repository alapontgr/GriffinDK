#pragma once
#include "Memory\MemAllocator.h"

#include "Utilities/platform.h"
#include INCLUDE_IMPLEMENTATION(Material)

namespace fdk
{
  namespace Rendering 
  {
    class RenderInterface;

    class Material : public IMPLEMENTATION(Material)
    {
    
    public:

      using BaseT = IMPLEMENTATION(Material);

      Material();

      ~Material();

      void init(const MaterialDesc& rDesc, const MaterialParameterSet& rSetLayout);

      void create_material();
    };

  }
}
#pragma once
#include "Memory\MemAllocator.h"

#include "Utilities/platform.h"
#ifdef FENIX_VK_IMPL
#include "Vulkan/VK_Material.h"
#endif

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

      void init(const MaterialDesc& rDesc, const MaterialParameterSet* pSetArray, const u32 setArrayCount);

      void create_material();
    };

  }
}
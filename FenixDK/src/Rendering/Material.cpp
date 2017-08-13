#include "Material.h"

#include "VK_Material.h"

#include "RenderDefines.h"

namespace fdk
{
  namespace Rendering
  {

    Material::Material()
    {

    }

    Material::~Material()
    {

    }

    Material* Material::create(Memory::MemAllocator& rAllocator)
    {
      return rAllocator.create<IMPL_NAME(Material)>();
    }

    void Material::init(const MaterialDesc& rDesc)
    {
      m_desc = rDesc;
    }

    void Material::create_material(RenderInterface& rRI)
    {
      IMPLEMENTATION(Material, this)->create_material(rRI);
    }

  }
}


#include "Material.h"

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
      return rAllocator.create<Material>();
    }

    void Material::init(const MaterialDesc& rDesc)
    {
      BaseT::init(rDesc);
    }

    void Material::create_material(RenderInterface& rRI)
    {
      BaseT::create_material(rRI);
    }

  }
}


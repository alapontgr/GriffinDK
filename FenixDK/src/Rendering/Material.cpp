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

    void Material::init(const MaterialDesc& rDesc)
    {
      m_desc = rDesc;
    }

  }
}


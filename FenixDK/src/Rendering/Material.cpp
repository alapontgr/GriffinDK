#include "Material.h"
#include "MaterialParameterSet.h"

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

	void Material::init(const MaterialDesc& rDesc, const MaterialParameterSet* pSetArray, const u32 setArrayCount)
	{
    m_setLayoutCount = setArrayCount;
    for (u32 i=0; i<setArrayCount; ++i)
    {
      m_parameterSetLayouts[i] = &pSetArray[i];
    }
		BaseT::init(rDesc);
	}

	void Material::create_material()
	{
		BaseT::create_material();
	}
}
}

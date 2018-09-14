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

	void Material::init(const MaterialDesc& rDesc, const MaterialParameterSet& rSetLayout)
	{
    m_pParameterSetLayout = &rSetLayout;
		BaseT::init(rDesc);
	}

	void Material::create_material()
	{
		BaseT::create_material();
	}
}
}

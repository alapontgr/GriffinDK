#pragma once

#include "Utilities/platform.h"
#ifdef FENIX_VK_IMPL
#include "Vulkan/VK_MaterialParameterSet.h"
#endif

namespace fdk
{
namespace Rendering
{
	class MaterialParameterSet : public IMPLEMENTATION(MaterialParameterSet)
	{

	public:
		using BaseT = IMPLEMENTATION(MaterialParameterSet);

		MaterialParameterSet();

		void clear();

		void add_parameter(
				const u32 bindingSlot,
				const Utilities::Name& rParamName,
				const EShaderParameterType paramType,
				const ShaderStageMask stages);

    ParameterSlot get_parameter(const Utilities::Name& rParamName) const;

		u32 total_size() const;

		void create();
	};
}
}
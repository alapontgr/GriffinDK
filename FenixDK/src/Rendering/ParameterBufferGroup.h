#pragma once

#include "Utilities/platform.h"
#ifdef FENIX_VK_IMPL
#include "Vulkan/VK_ParameterBufferGroup.h"
#endif

namespace fdk
{
namespace Rendering
{
	class ParameterBufferGroup : public IMPLEMENTATION(ParameterBufferGroup)
	{
	public:

    using BaseT = IMPLEMENTATION(ParameterBufferGroup);

		ParameterBufferGroup();

		void create(const u32 maxElements);
  };
}
}
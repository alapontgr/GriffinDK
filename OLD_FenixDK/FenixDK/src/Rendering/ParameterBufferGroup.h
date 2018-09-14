#pragma once

#include "Utilities/platform.h"
#include INCLUDE_IMPLEMENTATION(ParameterBufferGroup)

namespace fdk
{
namespace Rendering
{
	class ParameterBufferGroup : public IMPLEMENTATION(ParameterBufferGroup)
	{
	public:

    using BaseT = IMPLEMENTATION(ParameterBufferGroup);

		ParameterBufferGroup();

		void create();
  };
}
}
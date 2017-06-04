#pragma once

#include "RenderInterface.h"
#include "Utilities/VulkanInclude.h"

namespace fdk
{
namespace Rendering
{
	class VKRenderInterface : public RenderInterface
	{
	public:
		VKRenderInterface();
		~VKRenderInterface();

    void test();
	private:
	};

}
}
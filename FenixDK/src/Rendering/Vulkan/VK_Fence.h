#pragma once

#include "Utilities/VulkanInclude.h"

namespace fdk
{
namespace Rendering
{
	class RenderInterface;

	namespace Vulkan
	{
		class VK_Fence
		{
		public:
			VK_Fence();

			void wait(const RenderInterface& rRi);

			VkFence m_fence;
		};
	}
}
}
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

			void wait();

			VkFence m_fence;
		};
	}
}
}
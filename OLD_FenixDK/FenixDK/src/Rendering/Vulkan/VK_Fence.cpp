#include "VK_Fence.h"
#include "../RenderInterface.h"

namespace fdk
{
namespace Rendering
{

	namespace Vulkan
	{
		VK_Fence::VK_Fence()
				: m_fence{VK_NULL_HANDLE}
		{
		}

		void VK_Fence::wait()
{
			auto result = vkWaitForFences(RenderInterface::s_device, 1, &m_fence, VK_FALSE, 1000000000);
			VK_CHECK(result, "Waited to long for fences");
			result = vkResetFences(RenderInterface::s_device, 1, &m_fence);
			VK_CHECK(result, "Failed to reset the fences");
		}
	}
}
}

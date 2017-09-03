#include "VK_CommandBuffer.h"

#include "Utilities\platform.h"

namespace fdk
{
namespace Rendering
{

	namespace Vulkan
	{
		VK_CommandBuffer::VK_CommandBuffer()
				: m_commandBuffer{VK_NULL_HANDLE}
		{
		}

		void VK_CommandBuffer::start_recording()
		{
			// TODO: Refactor to be possible to use it as a secondary command buffer (Inheritance info)
			VkCommandBufferBeginInfo info;
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			info.pNext = nullptr;
			info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			info.pInheritanceInfo = nullptr;

			// begin command buffer
			auto result = vkBeginCommandBuffer(m_commandBuffer, &info);
			VK_CHECK(result, "Failed to begin cmd buffer");
		}

		void VK_CommandBuffer::end_recording()
		{
			vkEndCommandBuffer(m_commandBuffer);
		}
	}
}
}

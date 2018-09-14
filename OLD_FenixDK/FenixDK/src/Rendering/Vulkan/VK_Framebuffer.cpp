#include "VK_Framebuffer.h"

#include "../RenderPass.h"

#include "Framework\app.h"
#include "Utilities\platform.h"
#include "../RenderSurface.h"
#include "../Texture2D.h"
#include "../RenderInterface.h"

namespace fdk
{
namespace Rendering
{

	namespace Vulkan
	{
		VK_Framebuffer::VK_Framebuffer()
				: m_handle{VK_NULL_HANDLE}
		{
		}

		VK_Framebuffer::~VK_Framebuffer() {}

		void VK_Framebuffer::create(
				const RenderPass& rRenderPass,
				const RenderSurface& rRenderSurface)
		{
			VkFramebufferCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			info.pNext = nullptr;
			info.flags = 0;
			info.renderPass = rRenderPass.handle();
			info.attachmentCount = 1;
			info.pAttachments = &rRenderSurface.m_pColorTexture->m_view;
			info.width = rRenderSurface.m_width;
			info.height = rRenderSurface.m_height;
			info.layers = 1;
			auto result = vkCreateFramebuffer(RenderInterface::s_device, &info, nullptr, &m_handle);
			VK_CHECK(result, "Failed to create framebuffer");
		}

		void VK_Framebuffer::destroy()
{
			if (m_handle != VK_NULL_HANDLE)
			{
				vkDestroyFramebuffer(RenderInterface::s_device, m_handle, nullptr);
				m_handle = VK_NULL_HANDLE;
			}
		}
	}
}
}

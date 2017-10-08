#pragma once

#include "Utilities/VulkanInclude.h"

namespace fdk
{
namespace Rendering
{

	class RenderInterface;
	class RenderPass;
	class RenderSurface;

	namespace Vulkan
	{
		class VK_Framebuffer
		{
		public:
			VK_Framebuffer();
			~VK_Framebuffer();

			void create(
					const RenderPass& rRenderPass,
					const RenderSurface& rRenderSurface);

			void destroy(const RenderInterface& rRI);

			VkFramebuffer handle() { return m_handle; }

		private:
			VkFramebuffer m_handle;
		};
	}
}
}
#pragma once

#include "Utilities/VulkanInclude.h"
#include "Utilities/types.h"
#include "../BufferUtils.h"

namespace fdk
{
namespace Rendering
{
	class Buffer;
	class CommandBuffer;

	namespace Vulkan
	{
		class VK_Buffer
		{
		public:
			VK_Buffer();

			VkBuffer m_pBuffer;
			VkDeviceMemory m_pMemory;

		protected:
			void copy_buffer_range(
					const Buffer& rFrom,
					const BufferDesc& rMyDesc,
					const u32 fromOffset,
					const u32 offset,
					const u32 size,
					CommandBuffer& rCmdBuffer);
		};
	}
}
}
#pragma once

#include "../BufferBase.h"
#include "Utilities/types.h"

#include "Utilities/VulkanInclude.h"

namespace fdk
{
namespace Rendering
{
	class Buffer;
	class CommandBuffer;

	namespace Vulkan
	{
    class VK_Buffer : public BufferBase
		{
		public:
			VK_Buffer();

			VkBuffer m_pBuffer;
			VkDeviceMemory m_pMemory;

		protected:

			void copy_buffer_range(
					const Buffer& rFrom,
					const u32 fromOffset,
					const u32 offset,
					const u32 size,
					CommandBuffer& rCmdBuffer);

      void update_region(
        const BufferRange& rRange,
        const void* pData, 
        CommandBuffer& rCmdBuffer);
		};
	}
}
}
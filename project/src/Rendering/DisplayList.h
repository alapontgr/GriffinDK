#pragma once

#include "Utilities/VulkanInclude.h"

namespace dk {
namespace Rendering {
struct DisplayList {
  VkCommandBuffer m_cmdBuffer;
  VkFence m_isReadyFence;
};
}
}
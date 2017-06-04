#pragma once

#include "Utilities/VulkanInclude.h"

namespace fdk {
namespace Rendering {
struct DisplayList {
  VkCommandBuffer m_cmdBuffer;
  VkFence m_isReadyFence;
};
}
}
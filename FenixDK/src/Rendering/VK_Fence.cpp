#include "VK_Fence.h"
#include "RenderInterface.h"

namespace fdk
{
  namespace Rendering 
  {
    
    VK_Fence::VK_Fence() : m_fence{ VK_NULL_HANDLE } {}

    void VK_Fence::wait(const RenderInterface& rRi)
    {
      auto result = vkWaitForFences(rRi.m_device, 1, &m_fence, VK_FALSE, 1000000000);
      VK_CHECK(result, "Waited to long for fences");
      result = vkResetFences(rRi.m_device, 1, &m_fence);
      VK_CHECK(result, "Failed to reset the fences");
    }
  }
}


#pragma once

#include "Utilities/VulkanInclude.h"

#include "Utilities\types.h"
#include "Memory/MemUtils.h"
#include "../ParameterBufferBase.h"

namespace fdk
{
  namespace Rendering 
  {
    class RenderInterface;
    class MaterialParameterSet;

    namespace Vulkan 
    {
      class VK_ParameterBuffer : public ParameterBufferBase
      {
      public:

        VK_ParameterBuffer();

      protected:

         void create();

        void update();

        void release();

        void bind();

      private:

        VkDescriptorSet m_descriptorSet;
      };

    }
  }
}
#pragma once

#include "Utilities/VulkanInclude.h"

#include "Utilities\types.h"
#include "Memory/MemUtils.h"

namespace fdk
{
  namespace Rendering 
  {
    class RenderInterface;
    class MaterialParameterSet;

    namespace Vulkan 
    {
      class VK_ParameterBuffer 
      {
      public:

        VK_ParameterBuffer();

      protected:

         void create(
           RenderInterface& rRI,
           const MaterialParameterSet& rParameterSet,
           const u32 bindingSlot);

        void update(const MaterialParameterSet& rParameterSet, Memory::mem_ptr_t pData);

        void release(RenderInterface& rRI);

        void bind(const MaterialParameterSet& rParameterSet);

      private:

        VkDescriptorSet m_descriptorSet;
      };

    }
  }
}
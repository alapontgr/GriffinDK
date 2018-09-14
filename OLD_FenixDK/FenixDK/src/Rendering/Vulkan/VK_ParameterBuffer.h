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

        VkDescriptorSet m_descriptorSet;

      protected:

         void create();

        void update(Memory::MemAllocator& rTmpAllocator);

        void release();

      private:

        void fill_write_data_for_param(
          const ParameterDefinition& rParam, 
          VkWriteDescriptorSet* pParamWrite, 
          Memory::MemAllocator& rTmpAllocator);

        void fill_constant_buffer_write(
          const ParameterDefinition& rParam,
          VkWriteDescriptorSet* pParamWrite,
          Memory::MemAllocator& rTmpAllocator);
      };

    }
  }
}
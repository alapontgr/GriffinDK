#include "VK_ParameterBuffer.h"

#include "..\RenderInterface.h"
#include "..\MaterialParameterSet.h"
#include "..\ParameterBufferGroup.h"
#include "..\ConstantBuffer.h"
#include "..\Buffer.h"

namespace fdk
{
namespace Rendering
{
	namespace Vulkan
	{

		VK_ParameterBuffer::VK_ParameterBuffer()
		{
		}

		void VK_ParameterBuffer::create()
		{
			VkDescriptorSetAllocateInfo descSetAllocInfo{};
			descSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descSetAllocInfo.pNext = nullptr;
			descSetAllocInfo.descriptorPool = m_pAllocationGroup->m_descriptorPool;
			descSetAllocInfo.descriptorSetCount = 1;
			descSetAllocInfo.pSetLayouts = &m_pParameterSetLayout->m_pLayout;
			auto result = vkAllocateDescriptorSets(RenderInterface::s_device, &descSetAllocInfo, &m_descriptorSet);
			VK_CHECK(result, "Failed to allocate the descriptor set");
		}

		void VK_ParameterBuffer::update(Memory::MemAllocator& rTmpAllocator)
		{
			FDASSERT(m_pAllocationGroup, "Invalid allocation group");
			FDASSERT(m_pParameterSetLayout, "Invalid parameter set layout");
			u32 paramCount = m_pParameterSetLayout->parameter_count();
			VkWriteDescriptorSet* pWriteCollection = rTmpAllocator.create_array<VkWriteDescriptorSet>(paramCount);
			auto pWriteIter = pWriteCollection;
			for (const auto& rParam : m_pParameterSetLayout->parameters())
			{
				fill_write_data_for_param(rParam, pWriteIter, rTmpAllocator);
				pWriteIter++;
			}

			vkUpdateDescriptorSets(RenderInterface::s_device, paramCount, pWriteCollection, 0, nullptr);
		}

		void VK_ParameterBuffer::release()
		{
		}

    void VK_ParameterBuffer::fill_write_data_for_param(const ParameterDefinition& rParam, VkWriteDescriptorSet* pParamWrite, Memory::MemAllocator& rTmpAllocator)
    {
      // TODO: Refactor that. Use the type as an index to a table of pointers to functions instead
      switch (rParam.m_paramType)
      {
      case Type_ConstantBuffer:
      {
        fill_constant_buffer_write(rParam, pParamWrite, rTmpAllocator);
        break;
      }
      default:
      {
        FDASSERT(false, "Invalid parameter type");
        break;
      }
      }
    }

    void VK_ParameterBuffer::fill_constant_buffer_write(const ParameterDefinition& rParam, VkWriteDescriptorSet* pParamWrite, Memory::MemAllocator& rTmpAllocator)
    {
      auto pParamData = get_parameter_data(rParam);
      FDASSERT(pParamData, "Invalid parameter");
      ConstantBuffer* pConstantBuffer = *(reinterpret_cast<ConstantBuffer**>(pParamData));
      auto bufferChunk = pConstantBuffer->buffer_chunk();
      FDASSERT(bufferChunk.m_pBuffer, "Invalid buffer");
      VkDescriptorBufferInfo* pBufferInfo = rTmpAllocator.create<VkDescriptorBufferInfo>();
      pBufferInfo->buffer = bufferChunk.m_pBuffer->m_pBuffer;
      pBufferInfo->offset = bufferChunk.m_dataOffset;
      pBufferInfo->range = bufferChunk.m_dataSize;

      pParamWrite->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      pParamWrite->pNext = nullptr;
      pParamWrite->dstSet = m_descriptorSet;
      pParamWrite->dstBinding = rParam.m_bindingSlot; 
      pParamWrite->dstArrayElement = 0; // TODO: Extend to support array
      pParamWrite->descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      pParamWrite->descriptorCount = 1;
      pParamWrite->pBufferInfo = pBufferInfo;
      pParamWrite->pImageInfo = nullptr;
      pParamWrite->pTexelBufferView = nullptr;
    }

  }
}
}

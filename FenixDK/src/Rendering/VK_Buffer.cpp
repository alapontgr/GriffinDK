#include "VK_Buffer.h"

#include "CommandBuffer.h"
#include "Buffer.h"
#include <unordered_map>

namespace fdk
{
namespace Rendering
{

  struct StageAccessConfig
  {
    VkPipelineStageFlagBits m_stage;
    VkAccessFlagBits m_access;
  };

  static const std::unordered_map<EBufferUsage, StageAccessConfig> g_vulkanStageAccessFlagsMap
  {
    { Transfer_Dst  , { VK_PIPELINE_STAGE_TRANSFER_BIT,  VK_ACCESS_TRANSFER_READ_BIT }},
    { Transfer_Dst  , { VK_PIPELINE_STAGE_TRANSFER_BIT,  VK_ACCESS_TRANSFER_WRITE_BIT }},
    { Index_Buffer  , { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,  VK_ACCESS_INDEX_READ_BIT}},
    { Vertex_Buffer , { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,  VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT } }
  };

  static StageAccessConfig get_access_and_stage(EBufferUsage bufferUsage) 
  {
    return g_vulkanStageAccessFlagsMap.at(bufferUsage);
  }

	VK_Buffer::VK_Buffer()
			: m_pBuffer{VK_NULL_HANDLE}
			, m_pMemory{VK_NULL_HANDLE}
	{
	}

  void VK_Buffer::copy_buffer_range(
    const Buffer& rFrom,
    const BufferDesc& rMyDesc,
    const u32 fromOffset,
    const u32 offset,
    const u32 size,
    CommandBuffer& rCmdBuffer)
  {
    auto fromCurrentToTransfer = get_access_and_stage(rMyDesc.m_currentUsage);
    auto fromTransferToCurrent = get_access_and_stage(EBufferUsage::Transfer_Dst);

    VkBufferMemoryBarrier barrier;
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    barrier.pNext = nullptr;
    barrier.buffer = m_pBuffer;
    barrier.offset = offset;
    barrier.size = size;
    barrier.srcAccessMask = fromCurrentToTransfer.m_access;
    barrier.dstAccessMask = fromTransferToCurrent.m_access;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    vkCmdPipelineBarrier(
      rCmdBuffer.m_commandBuffer,
      fromCurrentToTransfer.m_stage,
      fromTransferToCurrent.m_stage,
      0,
      0, nullptr,
      1, &barrier,
      0, nullptr);

    VkBufferCopy region;
    region.srcOffset = fromOffset;
    region.dstOffset = offset;
    region.size = size;
    vkCmdCopyBuffer(rCmdBuffer.m_commandBuffer, rFrom.m_pBuffer, m_pBuffer, 1, &region);

    barrier.srcAccessMask = fromTransferToCurrent.m_access;
    barrier.dstAccessMask = fromCurrentToTransfer.m_access;
    vkCmdPipelineBarrier(
      rCmdBuffer.m_commandBuffer,
      fromTransferToCurrent.m_stage,
      fromCurrentToTransfer.m_stage,
      0,
      0, nullptr,
      1, &barrier,
      0, nullptr);
  }

}
}

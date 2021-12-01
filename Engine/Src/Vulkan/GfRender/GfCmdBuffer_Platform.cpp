////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/16
//	File: 	GFCMDBUFFER_PLATFORM.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfRender/GfCmdBuffer.h"
#include "Common/GfRender/GfRenderContext.h"
#include "Common/GfRender/GfWindow.h"
#include "Common/GfRender/GfRenderPass.h"
#include "Common/GfRender/GfShaderPipeline.h"
#include "Common/GfRender/GfCmdBuffer.h"
#include "Common/GfRender/GraphicResources/GfGraphicResources.h"

////////////////////////////////////////////////////////////////////////////////

#define GF_INFINITE_TIMEOUT 1000000000

////////////////////////////////////////////////////////////////////////////////

static void getStagesAndAccessForBufferUsage(
	const BufferUsageFlags::Mask bufferUsage,
	VkAccessFlags& access, 
	VkPipelineStageFlags& stages) 
{
	access = 0;
	stages = 0;

	if ((bufferUsage & BufferUsageFlags::SrcTransfer) != 0) 
	{
		access |= VK_ACCESS_TRANSFER_READ_BIT;
		stages |= VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	if ((bufferUsage & BufferUsageFlags::DstTransfer) != 0) 
	{
		access |= VK_ACCESS_TRANSFER_WRITE_BIT;
		stages |= VK_PIPELINE_STAGE_TRANSFER_BIT;
	}

	if ((bufferUsage & BufferUsageFlags::VertexBuffer) != 0) 
	{
		access |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
		stages |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
	}
	if ((bufferUsage & BufferUsageFlags::IndexBuffer) != 0) 
	{
		access |= VK_ACCESS_INDEX_READ_BIT;
		stages |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
	}
	if ((bufferUsage & BufferUsageFlags::AllUniform) != 0) 
	{
		access |= VK_ACCESS_UNIFORM_READ_BIT;
		stages |= ((bufferUsage & BufferUsageFlags::UniformVertex) != 0) ? VK_PIPELINE_STAGE_VERTEX_SHADER_BIT : 0;
		stages |= ((bufferUsage & BufferUsageFlags::UniformFragment) != 0) ? VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT : 0;
		stages |= ((bufferUsage & BufferUsageFlags::UniformCompute) != 0) ? VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT : 0;
	}
	if ((bufferUsage & BufferUsageFlags::AllStorage) != 0) 
	{
		access |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
		stages |= ((bufferUsage & BufferUsageFlags::StorageVertex) != 0) ? VK_PIPELINE_STAGE_VERTEX_SHADER_BIT : 0;
		stages |= ((bufferUsage & BufferUsageFlags::StorageFragment) != 0) ? VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT : 0;
		stages |= ((bufferUsage & BufferUsageFlags::StorageCompute) != 0) ? VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT : 0;
	}
	if ((bufferUsage & BufferUsageFlags::Indirect) != 0) 
	{
		access |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
		stages |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT ;
	}
}

static void getStagesAccessAspectAndLayoutForTextureUsage(
	const TextureUsageFlags::Mask textureUsage, bool isNewLayout,
	VkAccessFlags& access, 
	VkPipelineStageFlags& stages, 
	VkImageAspectFlags& aspect, 
	VkImageLayout& layout) 
{
	access = 0;
	stages = 0;
	aspect = 0;
	layout = VK_IMAGE_LAYOUT_UNDEFINED;

	// Special cases
	if ((textureUsage & TextureUsageFlags::Undefined) != 0) 
	{
		layout = VK_IMAGE_LAYOUT_UNDEFINED;
		return;
	}
	if ((textureUsage & TextureUsageFlags::Present) != 0) 
	{
		layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		stages |= isNewLayout ? VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT : 0;
		return;
	}

	// Normal cases
	if ((textureUsage & TextureUsageFlags::SRVAll) != 0) 
	{
		layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		aspect = VK_IMAGE_ASPECT_COLOR_BIT;
		access |= VK_ACCESS_SHADER_READ_BIT;
		stages |= ((textureUsage & TextureUsageFlags::SRVVertex) != 0) ? VK_PIPELINE_STAGE_VERTEX_SHADER_BIT : 0;
		stages |= ((textureUsage & TextureUsageFlags::SRVFragment) != 0) ? VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT : 0;
		stages |= ((textureUsage & TextureUsageFlags::SRVCompute) != 0) ? VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT : 0;
	}

	if ((textureUsage & TextureUsageFlags::UAVAll) != 0) 
	{
		GF_ASSERT(layout == VK_IMAGE_LAYOUT_UNDEFINED, "trying to use the resource for multiple incompatible usages here");
		layout = VK_IMAGE_LAYOUT_GENERAL;
		aspect = VK_IMAGE_ASPECT_COLOR_BIT;
		access |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
		stages |= ((textureUsage & TextureUsageFlags::UAVVertex) != 0) ? VK_PIPELINE_STAGE_VERTEX_SHADER_BIT : 0;
		stages |= ((textureUsage & TextureUsageFlags::UAVFragment) != 0) ? VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT : 0;
		stages |= ((textureUsage & TextureUsageFlags::UAVCompute) != 0) ? VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT : 0;
	}

	if ((textureUsage & TextureUsageFlags::SrcTransfer) != 0) 
	{
		GF_ASSERT(layout == VK_IMAGE_LAYOUT_UNDEFINED, "trying to use the resource for multiple incompatible usages here");
		layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		aspect = 0;
		access |= VK_ACCESS_TRANSFER_READ_BIT;
		stages |= VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	if ((textureUsage & TextureUsageFlags::DstTransfer) != 0) 
	{
		GF_ASSERT(layout == VK_IMAGE_LAYOUT_UNDEFINED, "trying to use the resource for multiple incompatible usages here");
		layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		aspect = 0;
		access |= VK_ACCESS_TRANSFER_WRITE_BIT;
		stages |= VK_PIPELINE_STAGE_TRANSFER_BIT;
	}

	if ((textureUsage & TextureUsageFlags::ColorAttachment) != 0) 
	{
		GF_ASSERT(layout == VK_IMAGE_LAYOUT_UNDEFINED, "trying to use the resource for multiple incompatible usages here");
		layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		aspect = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
		access |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		stages |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	if ((textureUsage & TextureUsageFlags::InputAttachment) != 0) 
	{
		GF_ASSERT(layout == VK_IMAGE_LAYOUT_UNDEFINED, "trying to use the resource for multiple incompatible usages here");
		layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ;
		aspect = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;
		access |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		stages |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	if ((textureUsage & TextureUsageFlags::DepthStencil) != 0) 
	{
		GF_ASSERT(layout == VK_IMAGE_LAYOUT_UNDEFINED, "trying to use the resource for multiple incompatible usages here");
		layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		aspect = VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT | VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT;
		access |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		stages |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
}

////////////////////////////////////////////////////////////////////////////////

GfCmdBufferFactory_Platform::GfCmdBufferFactory_Platform()
	: m_pool(VK_NULL_HANDLE)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBufferFactory_Platform::initRHI(const GfRenderContext& kCtx, GfRenderContextFamilies::Type queueType)
{
	VkCommandPoolCreateInfo kInfo{};
	kInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	kInfo.pNext = nullptr;
	kInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	kInfo.queueFamilyIndex = kCtx.GetFamilyIdx(queueType);
	VkResult eResult = vkCreateCommandPool(kCtx.Plat().m_pDevice, &kInfo, nullptr, &m_pool);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to create command pool");
}

void GfCmdBufferFactory_Platform::shutdown(const GfRenderContext& ctx)
{
	if (m_pool) 
	{
		vkDestroyCommandPool(ctx.Plat().m_pDevice, m_pool, nullptr);
		m_pool = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////

GF_DEFINE_PLATFORM_CTOR(GfCmdBuffer)
	, m_cmdBuffer(nullptr)
	, m_fence(0)
	, m_curBoundVariant(nullptr)
	, m_curBoundSet(VK_NULL_HANDLE)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::initRHI (const GfRenderContext& ctx, const GfCmdBufferFactory& factory, u32 type)
{
	GF_ASSERT(!m_cmdBuffer && !m_fence, "CMDBuffer already initialised");
	bool bIsPrimary = (type == GfCmdBufferType::Primary);
	VkCommandBufferAllocateInfo kinfo;
	kinfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	kinfo.pNext = nullptr;
	kinfo.commandPool = factory.Plat().getPool();
	kinfo.level = bIsPrimary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	kinfo.commandBufferCount = 1;

	VkResult eResult = vkAllocateCommandBuffers(ctx.Plat().m_pDevice, &kinfo, &m_cmdBuffer);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to allocate command buffers");

	// Create the fences for the command buffers
	VkFenceCreateInfo kFenceInfo;
	kFenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	kFenceInfo.pNext = nullptr;
	kFenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Signaled by default

	eResult = vkCreateFence(ctx.Plat().m_pDevice, &kFenceInfo, nullptr, &m_fence);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to create a fence");
}

void GfCmdBuffer_Platform::reset() 
{
	// Everything could be cleaned with a single memset. This is for clarity.
	for (u32 i = 0; i < s_MAX_DESCRIPTOR_SETS; ++i) 
	{
		for (u32 j = 0; j < s_MAX_BINDINGS_PER_SET; ++j) 
		{
			m_bindings[i][j].m_arrayCount = 0;
			m_bindings[i][j].m_array = nullptr;
		}
	}
	m_curBoundVariant = nullptr;

	vkResetCommandBuffer(m_cmdBuffer, 0);
}

void GfCmdBuffer_Platform::shutdown(const GfRenderContext& ctx) 
{
	GF_ASSERT_ALWAYS("TODO: Implement me");
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::waitForReadyRHI(const GfRenderContext& kCtx)
{
	VkResult eResult = vkWaitForFences(kCtx.Plat().m_pDevice, 1, &m_fence, VK_FALSE, GF_INFINITE_TIMEOUT);
	GF_ASSERT(eResult == VK_SUCCESS, "Waited to long for fences");
	eResult = vkResetFences(kCtx.Plat().m_pDevice, 1, &m_fence);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to reset the fences");
}

bool GfCmdBuffer_Platform::isReady(const GfRenderContext& kCtx)
{
	VkResult res = vkGetFenceStatus(kCtx.Plat().m_pDevice, m_fence);
	return res == VK_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::submitRHI(
	const GfRenderContext& ctx,
	GfRenderContextFamilies::Type eQueueType,
	const GfSemaphore* waitSemaphorePtr,
	const GfSemaphore* signalSemaphorePtr)
{
	VkPipelineStageFlags uiFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo kInfo{};
	kInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	kInfo.pNext = nullptr;
	
	VkSemaphore waitSemaphore = VK_NULL_HANDLE;
	VkSemaphore signalSemaphore = VK_NULL_HANDLE;

	if (waitSemaphorePtr) 
	{
		waitSemaphore = waitSemaphorePtr->Plat().getHandle();
		kInfo.waitSemaphoreCount = 1;
		kInfo.pWaitSemaphores = &waitSemaphore;
	}
	if (signalSemaphorePtr) 
	{
		signalSemaphore = signalSemaphorePtr->Plat().getHandle();
		kInfo.signalSemaphoreCount = 1;
		kInfo.pSignalSemaphores = &signalSemaphore;
	}

	kInfo.commandBufferCount = 1;
	kInfo.pCommandBuffers = &m_cmdBuffer;

	VkResult eResult = vkQueueSubmit(ctx.Plat().GetQueue(eQueueType), 1, &kInfo, m_fence);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to submit cmd block");
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::beginRecordingRHI(const GfRenderContext& kCtx)
{
	VkCommandBufferBeginInfo kInfo;
	kInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	kInfo.pNext = nullptr;
	kInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	kInfo.pInheritanceInfo = nullptr;

	VkImageSubresourceRange range;
	range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	range.baseMipLevel = 0;
	range.levelCount = 1;
	range.baseArrayLayer = 0;
	range.layerCount = 1;

	// begin command buffer
	VkResult eResult = vkBeginCommandBuffer(m_cmdBuffer, &kInfo);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to begin cmd buffer");
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::endRecordingRHI(const GfRenderContext& kCtx)
{
	VkResult eResult = vkEndCommandBuffer(m_cmdBuffer);
	GF_ASSERT(eResult == VK_SUCCESS, "Failed to end command buffer");
}

void GfCmdBuffer_Platform::beginRenderPass(const GfRenderContext& ctx, GfRenderPass* renderPass)
{
	renderPass->Plat().getOrCreateRenderPass(ctx);
	VkRenderPass renderPassVK = renderPass->Plat().getRenderPass();
	VkFramebuffer frameBufferVK = renderPass->Plat().getFramebuffer();

	bool usesDepth = renderPass->usesDepthAttachment();
	u32 attachmentCount = renderPass->getAttachmentCount() + (usesDepth ? 1 : 0);

	StackMemBlock clearColorsMem(static_cast<u32>(sizeof(VkClearValue)) * attachmentCount);
	VkClearValue* clearColors = reinterpret_cast<VkClearValue*>(clearColorsMem.get());
	u32 idx = 0;
	if (usesDepth) 
	{
		VkClearValue& depthClear = clearColors[idx++];
		depthClear.depthStencil.depth = renderPass->getDepthClear();
		depthClear.depthStencil.stencil = renderPass->getStencilClear();
	}

	v4 clearColor = renderPass->getClearColor();
	for (; idx < attachmentCount; ++idx) 
	{
		VkClearValue& clear = clearColors[idx];
		clear.color.float32[0] = clearColor[0];
		clear.color.float32[1] = clearColor[1];
		clear.color.float32[2] = clearColor[2];
		clear.color.float32[3] = clearColor[3];
	}

	VkRenderPassBeginInfo passBeginInfo{};
	passBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	passBeginInfo.pNext = nullptr;
	passBeginInfo.renderPass = renderPassVK;
	passBeginInfo.framebuffer = frameBufferVK;
	passBeginInfo.renderArea.extent.width = renderPass->getWidth();
	passBeginInfo.renderArea.extent.height = renderPass->getHeight();
	passBeginInfo.renderArea.offset.x = renderPass->getOffsetX();
	passBeginInfo.renderArea.offset.y = renderPass->getOffsetY();
	passBeginInfo.clearValueCount = attachmentCount;
	passBeginInfo.pClearValues = clearColors;
	vkCmdBeginRenderPass(getCmdBuffer(), &passBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void GfCmdBuffer_Platform::endRenderPass()
{
	vkCmdEndRenderPass(getCmdBuffer());
}

void GfCmdBuffer_Platform::drawcallCommonGraphics(GfRenderPipelineState* state)
{
	// Retrieve pipeline and layout
	if ((state->getFlags() & GfRenderStateFlags::BindMaterial) != 0) 
	{
		GfShaderPipeline* pipeline = state->getActivePipeline();
		GF_ASSERT(pipeline->isCompute(), "Trying to render graphics with a Compute Shader");
		m_curBoundVariant = pipeline->Plat().getOrCreateGraphicsPipeline(
			*m_kBase.m_ctx, 
			state->getHash(), 
			state->getActiveVariantData(),
			&state->getConfig(), 
			state->getVertexFormat(), 
			state->getCurRenderPass());
		GF_ASSERT(m_curBoundVariant->m_pipeline != VK_NULL_HANDLE, "Failed to obtain valid pipeline");
		vkCmdBindPipeline(getCmdBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_curBoundVariant->m_pipeline);
		state->disableFlag(GfRenderStateFlags::BindMaterial);
	}

	// Get descriptor set (create, update if needed)
	const GfShaderVariantData* variantData = state->getActiveVariantData();
	GfShaderPipeline* pipeline = state->getActivePipeline();
	GF_ASSERT(m_curBoundVariant != nullptr, "A variant was npot bound");
	for (u32 i=0; i<s_MAX_DESCRIPTOR_SETS; ++i) 
	{
		// If the set is used, get a Descriptor set and bind it
		if (variantData->m_setsLayoutHash[i] != 0) 
		{
			const GfShaderDeserializer& deserializer = pipeline->getDeserializer();
			u64 layoutHash(0);
			const GfWeakArray<GfDescriptorBindingSlot>& setBindings = deserializer.getDescriptorBindings(variantData, i, layoutHash);
			const GfWeakArray<GfResourceBindingExt> resourceBindings(m_bindings[i].data(), s_MAX_BINDINGS_PER_SET);
			GfDescriptorSetCache* setCache = m_kBase.m_ctx->Plat().getDescSetFactory()->getOrCreateDescriptorSetLayoutCache(layoutHash, setBindings);
			m_curBoundSet = setCache->getOrCreateDescSet(*m_kBase.m_ctx, m_kBase.m_linearAllocator, setBindings, resourceBindings);
			
			vkCmdBindDescriptorSets(getCmdBuffer(), 
				VK_PIPELINE_BIND_POINT_GRAPHICS, 
				m_curBoundVariant->m_layoutData->m_layout,
				i, 1,
				&m_curBoundSet, 
				0, nullptr);
		}
	}
}

void GfCmdBuffer_Platform::setViewport(const GfRenderContext& ctx, const GfViewport& viewport)
{
	VkViewport viewportVK;
	viewportVK.x = viewport.m_fOffsetX;
	viewportVK.y = viewport.m_fOffsetY;
	viewportVK.minDepth = viewport.m_fMinDepth;
	viewportVK.maxDepth = viewport.m_fMaxDepth;
	viewportVK.width = viewport.m_fWidth;
	viewportVK.height = viewport.m_fHeight;
	vkCmdSetViewport(getCmdBuffer(), 0, 1, &viewportVK);
}

void GfCmdBuffer_Platform::setScissors(const GfRenderContext& ctx, const GfScissor& scissor)
{
	VkRect2D scissorVK;
	scissorVK.offset.x = scissor.m_siOffsetX;
	scissorVK.offset.y = scissor.m_siOffsetY;
	scissorVK.extent.width = scissor.m_siWidth;
	scissorVK.extent.height = scissor.m_siHeight;
	vkCmdSetScissor(getCmdBuffer(), 0, 1, &scissorVK);
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::drawIndexedRHI(GfRenderPipelineState* state, u32 uiIdxCount, u32 uiInstanceCount, u32 uiIdxOffset /*= 0*/, u32 uiVertexOffset /*= 0*/, u32 uiFirstInstanceId /*= 0*/)
{
	drawcallCommonGraphics(state);
	vkCmdDrawIndexed(m_cmdBuffer, uiIdxCount, uiInstanceCount, uiIdxOffset, uiVertexOffset, uiFirstInstanceId);
}

////////////////////////////////////////////////////////////////////////////////

void GfCmdBuffer_Platform::drawRHI(GfRenderPipelineState* state, u32 uiVertexCount, u32 uiInstanceCount, u32 uiFirstVertex /*= 0*/, u32 uiFirstInstance /*= 0*/)
{
	drawcallCommonGraphics(state);
	vkCmdDraw(m_cmdBuffer, uiVertexCount, uiInstanceCount, uiFirstVertex, uiFirstInstance);
}

void GfCmdBuffer_Platform::bindVertexBuffersRHI(GfBuffer** vertexBuffers, u32* vertexBufferOffsets, u32 vertexBufferCount)
{
	StackMemBlock offsetsMem(static_cast<u32>(sizeof(VkDeviceSize) * vertexBufferCount));
	StackMemBlock buffersMem(static_cast<u32>(sizeof(VkBuffer) * vertexBufferCount));
	VkDeviceSize* offsets(reinterpret_cast<VkDeviceSize*>(offsetsMem.get()));
	VkBuffer* buffers(reinterpret_cast<VkBuffer*>(buffersMem.get()));
	for (u32 i = 0; i < vertexBufferCount; ++i) 
	{
		offsets[i] = static_cast<VkDeviceSize>(vertexBufferOffsets[i]);
		buffers[i] = vertexBuffers[i]->Plat().getHandle();
	}
	vkCmdBindVertexBuffers(m_cmdBuffer, 0, vertexBufferCount, buffers, offsets);
}

void GfCmdBuffer_Platform::bindIndexBufferRHI(const GfBuffer& buffer, u32 offset, bool useShort)
{
	vkCmdBindIndexBuffer(m_cmdBuffer, 
		buffer.Plat().getHandle(), 
		static_cast<VkDeviceSize>(offset), 
		useShort ? VkIndexType::VK_INDEX_TYPE_UINT16 : VkIndexType::VK_INDEX_TYPE_UINT32);
}

GfResourceBindingEntry& GfCmdBuffer_Platform::getEntryForBinding(const u32 setIdx, const u32 bindingIdx)
{
	GF_ASSERT(setIdx < s_MAX_DESCRIPTOR_SETS, "Invalid index for Desc set");
	GF_ASSERT(bindingIdx < s_MAX_BINDINGS_PER_SET, "Invalid index for binding");

	GfLinearAllocator& allocator = m_kBase.m_linearAllocator;
	GfResourceBindingExt& entryExt = m_bindings[setIdx][bindingIdx];
	if (bindingIdx == 0 && entryExt.m_arrayCount <= 1)
	{
		entryExt.m_arrayCount = 1;
		return entryExt.m_single;
	}
	else if (bindingIdx < entryExt.m_arrayCount)
	{
		return entryExt.m_array[bindingIdx];
	}

	// Grow. At this point bindingIdx >0
	GfResourceBindingEntry entryZero{};
	entryZero.m_type = GfParameterSlotType::Invalid;
	if (entryExt.m_arrayCount == 1)
	{
		entryZero = entryExt.m_single;
	}

	u32 newEntryCount = glm::max(entryExt.m_arrayCount * 2, bindingIdx + 1);
	u32 sizeOfEntry = static_cast<u32>(sizeof(GfResourceBindingEntry));
	u32 newArraySize = newEntryCount * sizeOfEntry;
	
	GfResourceBindingEntry* newArray = reinterpret_cast<GfResourceBindingEntry*>(allocator.allocRaw(newArraySize));

	u32 elementsToSkip(0);
	if (entryExt.m_arrayCount == 1)
	{
		newArray[0] = entryExt.m_single;
		elementsToSkip = 1;
	}
	else if(entryExt.m_arrayCount > 1)
	{
		memcpy(newArray, entryExt.m_array, entryExt.m_arrayCount * sizeOfEntry);
		elementsToSkip = entryExt.m_arrayCount;
	}
	
	// Invalidate new entries
	GfResourceBindingEntry* tail = newArray + elementsToSkip;
	for (u32 i=0; i<(newEntryCount-elementsToSkip); ++i) 
	{
		tail[i].m_type = GfParameterSlotType::Invalid;
	}

	entryExt.m_arrayCount = newEntryCount;
	entryExt.m_array = newArray;

	return entryExt.m_array[bindingIdx];
}

void GfCmdBuffer_Platform::bindUniformBuffer(const u32 setIdx, const u32 bindIdx, const GfBuffer& buffer, const u32 offset, const u32 size) 
{
	GfResourceBindingEntry& entry = getEntryForBinding(setIdx, bindIdx);
	entry.m_type = GfParameterSlotType::UniformBuffer;
	entry.m_bufferBind.m_buffer = buffer.Plat().getHandle();
	entry.m_bufferBind.m_offset = offset;
	entry.m_bufferBind.m_size = size;
	GF_ASSERT(entry.m_bufferBind.m_buffer != VK_NULL_HANDLE, "Resource has not been created");
}

void GfCmdBuffer_Platform::bindSampledTexture(const u32 setIdx, const u32 bindIdx, GfTextureView& texture, const u32 arrayIdx) 
{
	GfResourceBindingEntry& entry = getEntryForBinding(setIdx, bindIdx);
	entry.m_type = GfParameterSlotType::SampledImage;
	entry.m_sampledTextureBind.m_view = reinterpret_cast<VkImageView>(texture.getViewID(*m_kBase.m_ctx));
	entry.m_sampledTextureBind.m_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	GF_ASSERT(entry.m_sampledTextureBind.m_view != VK_NULL_HANDLE, "Resource has not been created");
}

void GfCmdBuffer_Platform::bindStorageImage(const u32 setIdx, const u32 bindIdx, GfTextureView& texture, const u32 arrayIdx) 
{
	GfResourceBindingEntry& entry = getEntryForBinding(setIdx, bindIdx);
	entry.m_type = GfParameterSlotType::StorageImage;
	entry.m_imageBind.m_view = reinterpret_cast<VkImageView>(texture.getViewID(*m_kBase.m_ctx));
	entry.m_imageBind.m_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	GF_ASSERT(entry.m_imageBind.m_view != VK_NULL_HANDLE, "Resource has not been created");
}

void GfCmdBuffer_Platform::bindSampler(const u32 setIdx, const u32 bindIdx, const GfSampler& sampler) 
{
	GfResourceBindingEntry& entry = getEntryForBinding(setIdx, bindIdx);
	entry.m_type = GfParameterSlotType::Sampler;
	entry.m_samplerBind.m_sampler = sampler.Plat().GetSampler();
	GF_ASSERT(entry.m_samplerBind.m_sampler != VK_NULL_HANDLE, "Resource has not been created");
}

void GfCmdBuffer_Platform::flushBarriers(const GfWeakArray<GfTextureBarrier>& textureBarriers, const GfWeakArray<GfBufferBarrier>& bufferBarriers)
{
	VkPipelineStageFlags srcStages(0);
	VkPipelineStageFlags dstStages(0);

	GfLinearAllocator::Block imageBarriersMem = m_kBase.m_linearAllocator.allocBlock(static_cast<u32>(sizeof(VkImageMemoryBarrier) * textureBarriers.size()));
	VkImageMemoryBarrier* imageB = reinterpret_cast<VkImageMemoryBarrier*>(imageBarriersMem.get());
	for (u32 i=0; i<textureBarriers.size(); ++i) 
	{
		const GfTextureBarrier& barrier = textureBarriers[i];
		imageB[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageB[i].pNext = nullptr;
		imageB[i].srcQueueFamilyIndex = m_kBase.m_ctx->GetFamilyIdx(GfRenderContextFamilies::Graphics); // TODO: Deal with different queues
		imageB[i].dstQueueFamilyIndex = m_kBase.m_ctx->GetFamilyIdx(GfRenderContextFamilies::Graphics); // TODO: Deal with different queues
		imageB[i].image = barrier.m_texture->Plat().getImage();
		imageB[i].subresourceRange.baseArrayLayer = barrier.m_viewConfig.m_firstSlice;
		imageB[i].subresourceRange.layerCount = barrier.m_viewConfig.m_sliceCount;
		imageB[i].subresourceRange.baseMipLevel = barrier.m_viewConfig.m_firstMipLevel;
		imageB[i].subresourceRange.levelCount = barrier.m_viewConfig.m_mipLevelCount;
		VkPipelineStageFlags oldStages(0); VkPipelineStageFlags newStages(0);
		VkImageAspectFlags oldAspect(0);
		getStagesAccessAspectAndLayoutForTextureUsage(barrier.m_oldUsage, false, imageB[i].srcAccessMask, oldStages, oldAspect, imageB[i].oldLayout);
		getStagesAccessAspectAndLayoutForTextureUsage(barrier.m_newUsage, true, imageB[i].dstAccessMask, newStages, imageB[i].subresourceRange.aspectMask, imageB[i].newLayout);

		// Unefficient for Mali
		srcStages |= oldStages;
		dstStages |= newStages;
	}

	GfLinearAllocator::Block bufferBarriersMem = m_kBase.m_linearAllocator.allocBlock(static_cast<u32>(sizeof(VkBufferMemoryBarrier) * bufferBarriers.size()));
	VkBufferMemoryBarrier* bufferB = reinterpret_cast<VkBufferMemoryBarrier*>(bufferBarriersMem.get());
	for (u32 i=0; i<bufferBarriers.size(); ++i) 
	{
		const GfBufferBarrier& barrier = bufferBarriers[i];
		bufferB[i].sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		bufferB[i].pNext = nullptr;
		bufferB[i].offset = barrier.m_offset;
		bufferB[i].size = barrier.m_size;
		bufferB[i].buffer = barrier.m_buffer->Plat().getHandle();
		bufferB[i].srcQueueFamilyIndex = m_kBase.m_ctx->GetFamilyIdx(GfRenderContextFamilies::Graphics); // TODO: Deal with different queues
		bufferB[i].dstQueueFamilyIndex = m_kBase.m_ctx->GetFamilyIdx(GfRenderContextFamilies::Graphics); // TODO: Deal with different queues
		VkPipelineStageFlags oldStages(0); VkPipelineStageFlags newStages(0);
		getStagesAndAccessForBufferUsage(barrier.m_oldUsage, bufferB[i].srcAccessMask, oldStages);
		getStagesAndAccessForBufferUsage(barrier.m_newUsage, bufferB[i].dstAccessMask, newStages);
		
		// Unefficient for Mali
		srcStages |= oldStages;
		dstStages |= newStages;
	}

	vkCmdPipelineBarrier(
		m_cmdBuffer,
		srcStages,
		dstStages,
		0, // TODO: Enable Per Region dependencies
		0, nullptr,
		bufferBarriers.size(), bufferB,
		textureBarriers.size(), imageB);
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/09/29
//	File: 	GfBuffer_Platform.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFBUFFER_PLATFORM_H__
#define __GFBUFFER_PLATFORM_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfRender/GfGraphicsSDK.h"

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;
class GfCmdBuffer;


////////////////////////////////////////////////////////////////////////////////

class GfBuffer_Platform
{
	GF_DECLARE_PLATFORM_MEMBERS(GfBuffer);
public:

	bool CreateRHI(const GfRenderContext& kCtxt);

	void DestroyRHI(const GfRenderContext& kCtxt);

	VkBuffer GetHandle() const;

	VkDeviceMemory GetMemory() const;

	void* MapRHI(const GfRenderContext& kCtxt, u32 uiOffset, u32 uiSize);

	void unMapRHI(const GfRenderContext& kCtxt);

	void flushAndUnMapRHI(const GfRenderContext& kCtxt, u32 uiOffset, u32 uiSize);

	////////////////////////////////////////////////////////////////////////////////
	// Buffer commands

	void copyRangeFromRHI(const GfCmdBuffer& kCmdBuffer, const GfBuffer& kFrom, u32 uiFromOffset, u32 uiToOffset, u32 uiSize);

	void updateRangeRHI(const GfCmdBuffer& kCmdBuffer, u32 uiOffset, u32 uiSize, void* pData);

	////////////////////////////////////////////////////////////////////////////////

private:

	VkBuffer			 m_pBuffer;
	VkDeviceMemory		 m_pMemory;
	VkAccessFlags		 m_currAccess;
	VkPipelineStageFlags m_currStage;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE VkBuffer GfBuffer_Platform::GetHandle() const
{
	return m_pBuffer;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE VkDeviceMemory GfBuffer_Platform::GetMemory() const
{
	return m_pMemory;
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFBUFFER_PLATFORM_H__
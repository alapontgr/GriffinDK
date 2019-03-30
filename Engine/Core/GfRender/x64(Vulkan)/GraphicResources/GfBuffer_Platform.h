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

#include "GfRender/Common/GfGraphicsSDK.h"

////////////////////////////////////////////////////////////////////////////////

class GfRenderContext;
class GfCmdBuffer;

////////////////////////////////////////////////////////////////////////////////

struct GfStageAccessConfig
{
	VkPipelineStageFlagBits m_eStage;
	VkAccessFlagBits		m_eAccess;
};

////////////////////////////////////////////////////////////////////////////////

class GfBuffer_Platform
{
	GF_DECLARE_PLATFORM_MEMBERS(GfBuffer);
public:

	static GfStageAccessConfig GetTransitionSettingsForType(u32 uiType);

	bool InitRHI(const GfRenderContext& kCtxt);

	void DestroyRHI(const GfRenderContext& kCtxt);

	VkBuffer GetHandle() const;

	VkDeviceMemory GetMemory() const;

	GfStageAccessConfig GetTransitionSettings() const;

	void* MapRHI(const GfRenderContext& kCtxt, u32 uiOffset, u32 uiSize);

	void UnMapRHI(const GfRenderContext& kCtxt);

	////////////////////////////////////////////////////////////////////////////////
	// Buffer commands

	void CopyRangeRHI(const GfCmdBuffer& kCmdBuffer, const GfBuffer& kFrom, const GfBuffer& kTo, u32 uiFromOffset, u32 uiToOffset, u32 uiSize);

	void UpdateRangeRHI(const GfCmdBuffer& kCmdBuffer, const GfBuffer& kBuffer, u32 uiOffset, u32 uiSize, void* pData);

	////////////////////////////////////////////////////////////////////////////////

private:

	VkBuffer		m_pBuffer;
	VkDeviceMemory	m_pMemory;
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
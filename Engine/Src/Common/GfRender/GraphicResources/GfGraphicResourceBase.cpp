////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/02/06
//	File: 	GfGraphicResourceBase.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfRender/GraphicResources/GfGraphicResourceBase.h"

////////////////////////////////////////////////////////////////////////////////

GfGraphicsResourceBase::GfGraphicsResourceBase()
	: m_eResourceType(ParamaterSlotType::Invalid)
{
}

////////////////////////////////////////////////////////////////////////////////

void GfGraphicsResourceBase::MarkAsDestroyed()
{
	m_uiGraphicResFlags.Disable(EGraphicResFlags::Initialised | EGraphicResFlags::GPUReady);
}

////////////////////////////////////////////////////////////////////////////////

void GfGraphicsResourceBase::MarkAsGPUReady()
{
	m_uiGraphicResFlags.Enable(EGraphicResFlags::GPUReady);
}

////////////////////////////////////////////////////////////////////////////////

void GfGraphicsResourceBase::MarkAsInitialised()
{
	m_uiGraphicResFlags.Enable(EGraphicResFlags::Initialised);
}

////////////////////////////////////////////////////////////////////////////////

void GfGraphicsResourceBase::MarkAsExternallyInitiailized()
{
	m_uiGraphicResFlags.Enable(EGraphicResFlags::ExternallyInitialised);
}

////////////////////////////////////////////////////////////////////////////////

GfBufferedResource::GfBufferedResource()
{
	m_uiGraphicResFlags.Enable(EGraphicResFlags::BufferedResource);
}

////////////////////////////////////////////////////////////////////////////////

void GfBufferedResource::BindBuffer(const GfBuffer::GfRange& kRange)
{
	if (!m_uiGraphicResFlags.IsEnable(EBUfferedResFlags::BufferBound))
	{
		m_kBufferRange = kRange;
		m_uiGraphicResFlags |= (EBUfferedResFlags::BufferBound);
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF

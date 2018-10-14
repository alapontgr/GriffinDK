////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/10/14
//	File: 	GfStackAllocator.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFSTACKALLOCATOR_H__
#define __GFSTACKALLOCATOR_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfCore/Common/GfCoreMinimal.h"

////////////////////////////////////////////////////////////////////////////////

struct GfDataChunk
{
	u64				m_ulAllocatedSize;
	void*			m_pData;
	GfDataChunk*	m_pNext;
};

////////////////////////////////////////////////////////////////////////////////

struct GfDataMarker
{
	GfDataChunk*	m_pChunk;		// Current back chunk at the point of creation
	u64				m_ulUsedSize;	// Size used by the chunk at the point of creation
	u32				m_uiMarkerIdx;
};

////////////////////////////////////////////////////////////////////////////////

class GfStackAllocator 
{
public:

	friend class GfScopedStackMemMarker;

	GfStackAllocator();

	void* Alloc(u32 uiSize, u32 uiAlign = 16);

	void FreeChunks();

private:
	
	GfDataMarker PushMemMarker();

	void FreeUntilMarker(const GfDataMarker& kMarker);

	bool Fits(u32 uiSize);

	void AllocateNewChunk();

	GfDataChunk*	m_pHead;
	GfDataChunk*	m_pBack;
	u32				m_uiActiveMemMarkers;
};

////////////////////////////////////////////////////////////////////////////////

class GfScopedStackMemMarker 
{
public:

	GfScopedStackMemMarker(GfStackAllocator* pAllocator);

	~GfScopedStackMemMarker();

private:

	GfStackAllocator*	m_pAllocator;
	GfDataMarker		m_kMarker;
	u32					m_uiMarkerIdx;
};

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE GfStackAllocator::GfStackAllocator()
	: m_pHead(nullptr)
	, m_pBack(nullptr)
	, m_uiActiveMemMarkers(0)
{
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void* GfStackAllocator::Alloc(u32 uiSize, u32 uiAlign /*= 16*/)
{
	GF_ASSERT_ALWAYS("Implement me!!!");
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfStackAllocator::FreeChunks()
{
	GF_ASSERT_ALWAYS("Implement me!!!");
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE GfDataMarker GfStackAllocator::PushMemMarker()
{
	GfDataMarker kMarker;
	kMarker.m_pChunk = m_pBack;
	kMarker.m_ulUsedSize = m_pBack->m_ulAllocatedSize;
	kMarker.m_uiMarkerIdx = m_uiActiveMemMarkers++;
	return kMarker;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfStackAllocator::FreeUntilMarker(const GfDataMarker& kMarker)
{
	m_uiActiveMemMarkers--;
	GF_ASSERT(kMarker.m_uiMarkerIdx == m_uiActiveMemMarkers, "Data markers must be freed in the inverse order as they were pushed");
	GF_ASSERT_ALWAYS("Implement me!!!");
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE bool GfStackAllocator::Fits(u32 uiSize)
{
	GF_ASSERT_ALWAYS("Implement me!!!");
	return false;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfStackAllocator::AllocateNewChunk()
{
	GF_ASSERT_ALWAYS("Implement me!!!");
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE GfScopedStackMemMarker::GfScopedStackMemMarker(GfStackAllocator* pAllocator)
{
	GF_ASSERT(pAllocator, "Invalid allocator");
	m_kMarker = pAllocator->PushMemMarker();
	m_pAllocator = pAllocator;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE GfScopedStackMemMarker::~GfScopedStackMemMarker()
{
	m_pAllocator->FreeUntilMarker(m_kMarker);
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFSTACKALLOCATOR_H__
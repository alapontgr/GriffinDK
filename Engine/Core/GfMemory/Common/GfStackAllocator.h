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
#include "GfMemory/Common/GfMemoryShared.h"

////////////////////////////////////////////////////////////////////////////////

struct GF_ALIGNAS(16) GfDataChunk
{
	void*			m_pData;
	GfDataChunk*	m_pNext;
	size_t			m_uiAvalSize;
};

////////////////////////////////////////////////////////////////////////////////

struct GfDataMarker
{
	GfDataChunk*	m_pChunk;		// Current back chunk at the point of creation
	size_t			m_uiAvalSize;	// Size used by the chunk at the point of creation
	u32				m_uiMarkerIdx;
};

////////////////////////////////////////////////////////////////////////////////

class GfStackAllocator 
{
public:

	friend class GfScopedStackMemMarker;

	GfStackAllocator();

	void* Alloc(size_t uiSize, size_t uiAlign = 16);

	void FreeChunks();

	// Size for each chunk of data
	static constexpr size_t ms_uiChunkSize = 64 * 1024;

private:
	
	GfDataMarker PushMemMarker();

	void FreeUntilMarker(const GfDataMarker& kMarker);

	bool Fits(size_t uiSize);

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

void* GfStackAllocator::Alloc(size_t uiSize, size_t uiAlign /*= 16*/)
{
	GF_ASSERT(uiSize <= ms_uiChunkSize, "Size is bigger than the size of an actual chunk");
	// Check for a valid chunk or if there is enough data in the chunk for the requested size plus the alignment
	if (!m_pBack || !Fits(uiSize + uiAlign)) 
	{
		AllocateNewChunk();
	}
	u8* pData = GfAlign((u8*)m_pBack->m_pData, uiAlign);
	m_pBack->m_pData = ((u8*)m_pBack->m_pData + uiSize);
	m_pBack->m_uiAvalSize -= uiSize;
	return pData;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE GfDataMarker GfStackAllocator::PushMemMarker()
{
	GfDataMarker kMarker;
	kMarker.m_pChunk = m_pBack;
	kMarker.m_uiAvalSize = m_pBack->m_uiAvalSize;
	kMarker.m_uiMarkerIdx = m_uiActiveMemMarkers++;
	return kMarker;
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfStackAllocator::FreeUntilMarker(const GfDataMarker& kMarker)
{
	m_uiActiveMemMarkers--;
	GF_ASSERT(kMarker.m_uiMarkerIdx == m_uiActiveMemMarkers, "Data markers must be freed in the inverse order as they were pushed");
	if (kMarker.m_uiMarkerIdx == m_uiActiveMemMarkers)
	{
		kMarker.m_pChunk->m_uiAvalSize = kMarker.m_uiAvalSize;
		kMarker.m_pChunk->m_pData = (reinterpret_cast<u8*>(kMarker.m_pChunk + 1) + (ms_uiChunkSize - kMarker.m_uiAvalSize));

		// Reset the chunks allocated after the marker's one
		GfDataChunk* pDataChunk(reinterpret_cast<GfDataChunk*>(kMarker.m_pChunk->m_pNext));
		while (pDataChunk)
		{
			// Reset the chunk
			pDataChunk->m_uiAvalSize = (ms_uiChunkSize - sizeof(GfDataChunk));
			pDataChunk->m_pData = (pDataChunk + 1);
			pDataChunk = pDataChunk->m_pNext;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

bool GfStackAllocator::Fits(size_t uiSize)
{
	return m_pBack->m_uiAvalSize >= uiSize;
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
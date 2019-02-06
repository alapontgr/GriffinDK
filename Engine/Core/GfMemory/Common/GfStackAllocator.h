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
#include "GfCore/Common/GfSingleton.h"
#include "GfMemory/Common/GfMemoryBase.h"

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

template <typename AllocT = GfDefaultAllocator, u32 CHUNKSIZE = GF_KB(64)>
class GfStackAllocator 
{
public:

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

	GfStackAllocator();

	void* AllocRaw(size_t uiSize, size_t uiAlign = 16);

	void FreeChunks();

	// Use this to reset the allocator without deallocating the memory
	void Reset();

	template <typename T>
	T* Alloc(u32 uiElementCount = 1)
	{
		return static_cast<T*>(AllocRaw(uiElementCount * sizeof(T), alignof(T)));
	}

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

// Singleton StackAllocator with a separate instance per thread
template <typename AllocT = GfDefaultAllocator, u32 CHUNKSIZE = GF_KB(64)>
class GfPerThreadStackAllocator 
	: public GfPerThreadSingleton<GfPerThreadStackAllocator<AllocT, CHUNKSIZE>>
	, public GfStackAllocator<AllocT, CHUNKSIZE>
{
public:

	// Not necessary but it's cleaner
	typedef GfStackAllocator<AllocT, CHUNKSIZE>::GfScopedStackMemMarker GfMemScope;

	GfPerThreadStackAllocator();
};

////////////////////////////////////////////////////////////////////////////////

#include "GfMemory/Common/GfStackAllocator.inl"

////////////////////////////////////////////////////////////////////////////////
#endif // __GFSTACKALLOCATOR_H__
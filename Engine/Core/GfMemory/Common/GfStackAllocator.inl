////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/01/13
//	File: 	GfStackAllocator.inl
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFSTACKALLOCATOR_INL__
#define __GFSTACKALLOCATOR_INL__
////////////////////////////////////////////////////////////////////////////////

template <typename AllocT = GfDefaultAllocator, u32 CHUNKSIZE = GF_KB(64)>
GfStackAllocator::GfStackAllocator()
	: m_pHead(nullptr)
	, m_pBack(nullptr)
	, m_uiActiveMemMarkers(0)
{
}

////////////////////////////////////////////////////////////////////////////////

template <typename AllocT = GfDefaultAllocator, u32 CHUNKSIZE = GF_KB(64)>
void* GfStackAllocator::Alloc(size_t uiSize, size_t uiAlign /*= 16*/)
{
	GF_ASSERT(uiSize <= CHUNKSIZE, "Size is bigger than the size of an actual chunk");
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

template <typename AllocT = GfDefaultAllocator, u32 CHUNKSIZE = GF_KB(64)>
void GfStackAllocator::FreeUntilMarker(const GfDataMarker& kMarker)
{
	m_uiActiveMemMarkers--;
	GF_ASSERT(kMarker.m_uiMarkerIdx == m_uiActiveMemMarkers, "Data markers must be freed in the inverse order as they were pushed");
	if (kMarker.m_uiMarkerIdx == m_uiActiveMemMarkers)
	{
		kMarker.m_pChunk->m_uiAvalSize = kMarker.m_uiAvalSize;
		kMarker.m_pChunk->m_pData = (reinterpret_cast<u8*>(kMarker.m_pChunk + 1) + (CHUNKSIZE - kMarker.m_uiAvalSize));

		// Reset the chunks allocated after the marker's one
		GfDataChunk* pDataChunk(reinterpret_cast<GfDataChunk*>(kMarker.m_pChunk->m_pNext));
		while (pDataChunk)
		{
			// Reset the chunk
			pDataChunk->m_uiAvalSize = (CHUNKSIZE - sizeof(GfDataChunk));
			pDataChunk->m_pData = (pDataChunk + 1);
			pDataChunk = pDataChunk->m_pNext;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

template <typename AllocT = GfDefaultAllocator, u32 CHUNKSIZE = GF_KB(64)>
bool GfStackAllocator::Fits(size_t uiSize)
{
	return m_pBack->m_uiAvalSize >= uiSize;
}

////////////////////////////////////////////////////////////////////////////////

template <typename AllocT = GfDefaultAllocator, u32 CHUNKSIZE = GF_KB(64)>
void GfStackAllocator::AllocateNewChunk()
{
	GfDataChunk* pDataChunk(nullptr);
	// If already initialized
	if (m_pBack)
	{
		pDataChunk = m_pBack->m_pNext;
		if (!pDataChunk)
		{
			// If needed allocate a new blob/chunk of memory
			void* pChunk(AllocT::Alloc(CHUNKSIZE, alignof(GfDataChunk)));
			pDataChunk = reinterpret_cast<GfDataChunk*>(pChunk);
			pDataChunk->m_pNext = nullptr;
			m_pBack->m_pNext = pDataChunk;
		}
		m_pBack = pDataChunk;
	}
	else
	{
		// Initialization
		void* pChunk(AllocT::Alloc(CHUNKSIZE, alignof(GfDataChunk)));
		pDataChunk = reinterpret_cast<GfDataChunk*>(pChunk);
		pDataChunk->m_pNext = nullptr;
		m_pHead = pDataChunk;
		m_pBack = m_pHead;
	}

	// Reset the chunk just in case
	pDataChunk->m_pData = (pDataChunk + 1);
	pDataChunk->m_uiAvalSize = (CHUNKSIZE - sizeof(GfDataChunk));
}

////////////////////////////////////////////////////////////////////////////////

template <typename AllocT = GfDefaultAllocator, u32 CHUNKSIZE = GF_KB(64)>
void GfStackAllocator::FreeChunks()
{
	GF_ASSERT(m_uiActiveMemMarkers == 0, "Trying to free the chunks before the markes have been freed");
	GfDataChunk* pChunk(m_pHead);
	while (pChunk)
	{
		GfDataChunk* pTmp(pChunk);
		pChunk = pChunk->m_pNext;
		AllocT::Free(pTmp);
	}
}

////////////////////////////////////////////////////////////////////////////////

template <typename AllocT = GfDefaultAllocator, u32 CHUNKSIZE = GF_KB(64)>
void GfStackAllocator::Reset()
{
	m_uiActiveMemMarkers = 0;
	GfDataChunk* pChunk(m_pHead);
	while (pChunk)
	{
		pChunk->m_pData = (pChunk + 1);
		pChunk->m_uiAvalSize = (CHUNKSIZE - sizeof(GfDataChunk));
		pChunk = pChunk->m_pNext;
	}
	m_pBack = m_pHead;
}

////////////////////////////////////////////////////////////////////////////////

template <typename AllocT = GfDefaultAllocator, u32 CHUNKSIZE = GF_KB(64)>
GfDataMarker GfStackAllocator::PushMemMarker()
{
	GfDataMarker kMarker;
	kMarker.m_pChunk = m_pBack;
	kMarker.m_uiAvalSize = m_pBack->m_uiAvalSize;
	kMarker.m_uiMarkerIdx = m_uiActiveMemMarkers++;
	return kMarker;
}

////////////////////////////////////////////////////////////////////////////////

template <typename StackAllocT = GfStackAllocator<GfDefaultAllocator, GF_KB(64)>>
GfScopedStackMemMarker::GfScopedStackMemMarker(GfStackAllocator* pAllocator)
{
	GF_ASSERT(pAllocator, "Invalid allocator");
	m_kMarker = pAllocator->PushMemMarker();
	m_pAllocator = pAllocator;
}

////////////////////////////////////////////////////////////////////////////////

template <typename StackAllocT = GfStackAllocator<GfDefaultAllocator, GF_KB(64)>>
GfScopedStackMemMarker::~GfScopedStackMemMarker()
{
	m_pAllocator->FreeUntilMarker(m_kMarker);
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFSTACKALLOCATOR_INL__
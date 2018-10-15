////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/10/14
//	File: 	GfStackAllocator.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "GfMemory/Common/GfStackAllocator.h"

////////////////////////////////////////////////////////////////////////////////

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
			void* pChunk(GfMemory::Alloc(ms_uiChunkSize, alignof(GfDataChunk)));
			pDataChunk = reinterpret_cast<GfDataChunk*>(pChunk);
			pDataChunk->m_pNext = nullptr;
			m_pBack->m_pNext = pDataChunk;
		}
		m_pBack = pDataChunk;
	}
	else 
	{
		// Initialization
		void* pChunk(GfMemory::Alloc(ms_uiChunkSize, alignof(GfDataChunk)));
		pDataChunk = reinterpret_cast<GfDataChunk*>(pChunk);
		pDataChunk->m_pNext = nullptr;
		m_pHead = pDataChunk;
		m_pBack = m_pHead;
	}
	// Reset the chunk just in case
	pDataChunk->m_pData = (pDataChunk + 1);
	pDataChunk->m_uiAvalSize = (ms_uiChunkSize - sizeof(GfDataChunk));
}

////////////////////////////////////////////////////////////////////////////////

void GfStackAllocator::FreeChunks()
{
	GF_ASSERT(m_uiActiveMemMarkers == 0, "Trying to free the chunks before the markes have been freed");
	GfDataChunk* pChunk(m_pHead);
	while (pChunk) 
	{
		GfDataChunk* pTmp(pChunk);
		pChunk = pChunk->m_pNext;
		GfMemory::Free(pTmp);
	}
}

////////////////////////////////////////////////////////////////////////////////
// EOF
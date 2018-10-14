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
	void* pChunk(GfMemory::Alloc(ms_uiChunkSize));
	GfDataChunk* pDataChunk(reinterpret_cast<GfDataChunk*>(pChunk));
	if (m_pHead) 
	{
		m_pBack->m_pNext = pDataChunk;
		m_pBack = pDataChunk;
	}
	else 
	{		
		m_pHead = pDataChunk;
		m_pBack = m_pHead;
	}
	pDataChunk->m_pNext = nullptr;
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
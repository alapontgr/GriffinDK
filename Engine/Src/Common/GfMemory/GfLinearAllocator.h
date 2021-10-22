#pragma once
////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	File: 	GfLinearAllocator.h
//
//	Copyright (c) 2021 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFLINEARALLOCATOR_H__
#define __GFLINEARALLOCATOR_H__

#include "Common/GfCore/GfStl.h"
#include "Common/GfMemory/GfRAII.h"

class GfLinearAllocator 
{
public:

	GfLinearAllocator()
		: m_activeMem(nullptr)
		, m_curSize(0)
		, m_pivot(0)
	{
		allocateNewBuffer(s_defaultSize);
	}

	GfLinearAllocator(u32 defaultSize)
		: m_activeMem(nullptr)
		, m_curSize(0)
		, m_pivot(0)
	{
		allocateNewBuffer(defaultSize);
	}

	void* allocRaw(u32 size) 
	{
		size = GfAlignUint(size, 16);
		if ((m_pivot + size) > m_curSize) 
		{
			u32 reqSize = m_curSize*2;
			while (reqSize < size) { reqSize *= 2; }
			allocateNewBuffer(reqSize);
		}
		char* ptr = m_activeMem.get() + m_pivot;
		m_pivot += size;
		return ptr;
	}

	void reset() 
	{
		m_pivot = 0;
		m_toRelease.clear();
	}

	void shutdown() 
	{
		reset();
		m_activeMem = nullptr;
		m_curSize = 0;
	}

private:

	static constexpr u32 s_defaultSize = 1 * 1024 * 1024; // 1MB

	void allocateNewBuffer(u32 reqMinSize) 
	{
		if (m_activeMem) 
		{
			m_toRelease.push_back(std::move(m_activeMem));
		}
		m_activeMem = GfUniquePtr<char[]>(new char[reqMinSize]);
		GF_ASSERT(m_activeMem.get(), "Failed to allocate memory");
		m_pivot = 0;
		m_curSize = reqMinSize;
	}
	
	GfVector<GfUniquePtr<char[]>> m_toRelease;
	GfUniquePtr<char[]> m_activeMem;
	u32 m_curSize;
	u32 m_pivot;
};

#endif
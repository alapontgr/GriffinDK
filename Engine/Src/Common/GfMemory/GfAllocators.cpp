////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	File: 	GfAllocators.cpp
//
//	Copyright (c) 2021 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////

#include "Common/GfCore/GfCoreMinimal.h"
#include "Common/GfMemory/GfAllocators.h"
#include "Common/GfMemory/GfMemoryBase.h"

////////////////////////////////////////////////////////////////////////////////

GfLinearAllocator::GfLinearAllocator()
	: m_activeMem(nullptr)
	, m_curSize(0)
	, m_pivot(0)
{
	allocateNewBuffer(s_defaultSize);
}

GfLinearAllocator::GfLinearAllocator(u32 defaultSize)
	: m_activeMem(nullptr)
	, m_curSize(0)
	, m_pivot(0)
{
	allocateNewBuffer(defaultSize);
}

void* GfLinearAllocator::allocRaw(u32 size)
{
	if (size != 0) 
	{
		size = GfAlignUint(size, 16);
		if ((m_pivot + size) > m_curSize)
		{
			u32 reqSize = m_curSize * 2;
			while (reqSize < size) { reqSize *= 2; }
			allocateNewBuffer(reqSize);
		}
		char* ptr = m_activeMem.get() + m_pivot;
		m_pivot += size;
		return ptr;
	}
	return nullptr;
}

GfLinearAllocator::Block GfLinearAllocator::allocBlock(const u32 size) 
{
	if (size != 0) 
	{
		u32 reqSize = GfAlignUint(size, 16);
		return std::move(Block(this, allocRaw(reqSize), reqSize));
	}
	return Block();
}

void GfLinearAllocator::release(void* ptr, const u32 size) 
{
	if (m_pivot >= size) 
	{
		u32 prevPivot = m_pivot - size;
		char* estimatedPtr = m_activeMem.get() + prevPivot;
		// If this was the last allocation, return memory
		if (ptr == estimatedPtr) 
		{
			m_pivot = prevPivot;
		}
	}
}

void GfLinearAllocator::reset()
{
	m_pivot = 0;
	m_toRelease.clear();
}

void GfLinearAllocator::shutdown()
{
	reset();
	m_activeMem = nullptr;
	m_curSize = 0;
}

void GfLinearAllocator::allocateNewBuffer(u32 reqMinSize)
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

GfLinearAllocator::Block::Block() 
	: m_alloc(nullptr)
	, m_ptr(nullptr)
	, m_blockSize(0)
{
}

GfLinearAllocator::Block::Block(GfLinearAllocator* alloc, void* ptr, u32 size)
	: m_alloc(alloc)
	, m_ptr(ptr)
	, m_blockSize(size)
{	
}

GfLinearAllocator::Block::Block(Block&& other)
{
	if (m_ptr) 
	{
		release();
	}
	m_alloc = other.m_alloc;
	m_ptr = other.m_ptr;
	m_blockSize = other.m_blockSize;
	other.m_alloc = nullptr;
	other.m_blockSize = m_blockSize;
	other.m_ptr = nullptr;
}

GfLinearAllocator::Block::~Block()
{
	if (m_ptr) 
	{
		release();
	}
}

void GfLinearAllocator::Block::release()
{
	if (m_alloc) 
	{
		m_alloc->release(m_ptr, m_blockSize);
		m_alloc = nullptr;
		m_blockSize = m_blockSize;
		m_ptr = nullptr;
	}
}

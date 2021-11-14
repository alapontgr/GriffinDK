#pragma once
////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	File: 	GfAllocators.h
//
//	Copyright (c) 2021 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFALLOCATORS_H__
#define __GFALLOCATORS_H__

#include "Common/GfCore/GfTypes.h"
#include "Common/GfCore/GfStl.h"
#include "Common/GfMemory/GfRAII.h"

class GfLinearAllocator 
{
public:

	class Block 
	{
	public:
		Block();
		Block(GfLinearAllocator* alloc, void* ptr, u32 size);
		Block(Block&& other);
		~Block();

		u8* get() const { return reinterpret_cast<u8*>(m_ptr); }
		u8* operator->() const { return get(); }

	private:
		Block& operator=(const Block&);
		Block(const Block&);

		void release();

		GfLinearAllocator* m_alloc;
		void* m_ptr;
		u32 m_blockSize;
	};

	GfLinearAllocator();

	GfLinearAllocator(u32 defaultSize);

	void* allocRaw(u32 size);

	Block allocBlock(const u32 size);

	void release(void* ptr, const u32 size);

	void reset();

	void shutdown();

private:

	static constexpr u32 s_defaultSize = 1 * 1024 * 1024; // 1MB

	void allocateNewBuffer(u32 reqMinSize);
	
	GfVector<GfUniquePtr<char[]>> m_toRelease;
	GfUniquePtr<char[]> m_activeMem;
	u32 m_curSize;
	u32 m_pivot;
};

#endif
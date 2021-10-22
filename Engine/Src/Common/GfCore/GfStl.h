////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/10/07
//	File: 	GfStl.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFSTL_H__
#define __GFSTL_H__
////////////////////////////////////////////////////////////////////////////////
// Includes

#include <vector>
#include <stack>
#include <queue>
#include <map>
#include <unordered_map>
#include <array>

#include "Common/GfMemory/GfRAII.h"

////////////////////////////////////////////////////////////////////////////////

// Vector
template <typename... Ts>
using GfVector = std::vector<Ts...>;

// Stack
template <typename... Ts>
using GfStack = std::stack<Ts...>;

// Queue
template <typename... Ts>
using GfQueue = std::queue<Ts...>;

// Map
template <typename... Ts>
using GfMap = std::map<Ts...>;

// Unordered Map
template <typename... Ts>
using GfUMap = std::unordered_map<Ts...>;

template <typename T, size_t Count>
using GfArray = std::array<T, Count>;

////////////////////////////////////////////////////////////////////////////////

template <typename T>
class GfWeakArray 
{
public:

	GfWeakArray() 
		: m_ptr(nullptr)
		, m_count(0) {}

	GfWeakArray(const T* ptr, const u32 count) 
		: m_ptr(ptr)
		, m_count(count) {}

	void set(const T* ptr, const u32 count) 
	{
		m_ptr = ptr;
		m_count = count;
	}

	const T& operator[] (u32 i) const { GF_ASSERT(i >= 0 && i < m_count, "Invalid index"); return m_ptr[i]; }

	const u32 size() const { return m_count; }

	const T* data() const { return m_ptr; }

private:

	const T* m_ptr;
	u32 m_count;
};

template <typename T, u32 ChunkSize>
class GfPool 
{
public:
	GfPool() {}

	T* pop() 
	{
		if (!m_aval.size()) 
		{
			allocateChunk();
		}
		T* entry = m_aval.front();
		m_aval.pop();
		return new (entry) T();
	}

	void push(T* entry) 
	{
		entry->~T();
		m_aval.push_back(entry);
	}

private:

	void allocateChunk() 
	{
		GfUniquePtr<T[]> ptr = GfUniquePtr<T[]>(new T[ChunkSize]);
		m_chunks.push_back(std::move(ptr));
		T* data = m_chunks.back().get();
		for (u32 i = 0; i < ChunkSize; ++i)
		{
			m_aval.push(data+i);
		}
	}

	GfQueue<T*> m_aval;
	GfVector<GfUniquePtr<T[]>> m_chunks;
};

////////////////////////////////////////////////////////////////////////////////
#endif // __GFSTL_H__
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

////////////////////////////////////////////////////////////////////////////////
#endif // __GFSTL_H__
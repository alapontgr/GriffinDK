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
#include <list>
#include <stack>
#include <queue>
#include <map>
#include <unordered_map>
#include <array>
#include <type_traits>

#include "Common/GfMemory/GfRAII.h"

////////////////////////////////////////////////////////////////////////////////

// Vector
template <typename... Ts>
using GfVector = std::vector<Ts...>;

template <typename... Ts>
using GfList = std::list<Ts...>;

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
	GfPool(GfPool&& other);

	T* pop();
	void push(T* entry);
	void clear();

private:

	void allocateChunk();

	GfQueue<T*> m_aval;
	GfVector<GfUniquePtr<T[]>> m_chunks;
};

////////////////////////////////////////////////////////////////////////////////

template <typename T, u32 PoolChunkSize>
class GfFactory 
{
public:

	using Pool = GfPool<T, PoolChunkSize>;

	class FactoryItem 
	{
		public:
			T* newInstance();
			void release();
		//private:
			FactoryItem() {}
	};

	GfFactory() {}

	static T* newInstance();
	void deleteInstance(T* entry);

private:

	static GfPool<T, PoolChunkSize> ms_pool;
};

////////////////////////////////////////////////////////////////////////////////

// Double linked list that works with externally allocated objects
template <typename T>
class GfIntrusiveList 
{
public:

	class GfListEntry 
	{
	public:
		friend class GfIntrusiveList;

		GfListEntry();
		GfListEntry* getPrev() const { return m_prev; }
		GfListEntry* getNext() const { return m_prev; }

	private:
		GfListEntry* m_prev;
		GfListEntry* m_next;
	};
	
	class Iterator 
	{
	public:
		Iterator(GfListEntry* entry);
		Iterator();

		GfListEntry* getVal() const { return m_valPtr; }
		Iterator& operator++();
		GfListEntry* operator->() const { return getVal(); }
		GfListEntry& operator*() const { return *getVal(); }
		bool operator==(const Iterator& other) const;
		bool operator!=(const Iterator& other) const;

	private:
		GfListEntry* m_valPtr;
	};

	GfIntrusiveList()
		: m_head(nullptr)
		, m_back(nullptr)
		, m_size(0)
	{}

	GfIntrusiveList(GfIntrusiveList&& other)
		: m_head(other.m_head)
		, m_back(other.m_back)
		, m_size(other.m_size)
	{}

	T* front();
	const T* front() const;
	T* back();
	const T* back() const;
	bool empty() const;
	u32 size() const;
	void clear();
	void insert(const u32 pos, T* val);
	void erase(const u32 pos);
	void erase(T* val);
	void pushFront(T* val);
	void pushBack(T* val);
	void popFront();
	void popBack();

	Iterator begin() const;
	Iterator end() const;

	const T* operator[](const u32 idx) const;
	T* operator[](const u32 idx);

private:

	static constexpr u32 s_PoolBatchSize = 128;

	GfListEntry* m_head;
	GfListEntry* m_back;
	u32 m_size;
};

#include "Common/GfCore/GfStl.inl"

////////////////////////////////////////////////////////////////////////////////
#endif // __GFSTL_H__
#include "GfStl.h"
#pragma once
////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	File: 	GfStl.inl
//
//	Copyright (c) 2021 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFSTL_INL__
#define __GFSTL_INL__

////////////////////////////////////////////////////////////////////////////////
// GfPool

template<typename T, u32 ChunkSize>
GF_FORCEINLINE GfPool<T, ChunkSize>::GfPool(GfPool&& other)
	: m_aval(std::move(other.m_aval))
	, m_chunks(std::move(other.m_chunks))
{
}

template<typename T, u32 ChunkSize>
GF_FORCEINLINE T* GfPool<T, ChunkSize>::pop()
{
	if (!m_aval.size())
	{
		allocateChunk();
	}
	T* entry = m_aval.front();
	m_aval.pop();
	return new (entry) T();
}

template<typename T, u32 ChunkSize>
GF_FORCEINLINE void GfPool<T, ChunkSize>::push(T* entry)
{
	entry->~T();
	m_aval.push(entry);
}

template<typename T, u32 ChunkSize>
GF_FORCEINLINE void GfPool<T, ChunkSize>::clear()
{
	for (const GfUniquePtr<T[]>& chunk : m_chunks)
	{
		T* data = chunk.get();
		for (u32 i = 0; i < ChunkSize; ++i)
		{
			(data + i)->~T();
			m_aval.push(data + i);
		}
	}
}

template<typename T, u32 ChunkSize>
GF_FORCEINLINE void GfPool<T, ChunkSize>::allocateChunk()
{
	GfUniquePtr<T[]> ptr = GfUniquePtr<T[]>(new T[ChunkSize]);
	m_chunks.push_back(std::move(ptr));
	T* data = m_chunks.back().get();
	for (u32 i = 0; i < ChunkSize; ++i)
	{
		m_aval.push(data + i);
	}
}

////////////////////////////////////////////////////////////////////////////////
// GfFactory

template <typename T, u32 PoolChunkSize> GfPool<T, PoolChunkSize> GfFactory<T, PoolChunkSize>::ms_pool;

template<typename T, u32 PoolChunkSize>
GF_FORCEINLINE T* GfFactory<T, PoolChunkSize>::newInstance()
{
	return ms_pool.pop();
}

template<typename T, u32 PoolChunkSize>
GF_FORCEINLINE void GfFactory<T, PoolChunkSize>::deleteInstance(T* entry)
{
	ms_pool.push(entry);
}

template<typename T, u32 PoolChunkSize>
GF_FORCEINLINE T* GfFactory<T, PoolChunkSize>::FactoryItem::newInstance()
{
	return GfFactory<T, PoolChunkSize>::newInstance();
}

template<typename T, u32 PoolChunkSize>
GF_FORCEINLINE void GfFactory<T, PoolChunkSize>::FactoryItem::release()
{
	GfFactory<T, PoolChunkSize>::deleteInstance(this);
}


////////////////////////////////////////////////////////////////////////////////
// GfIntrusiveList

template<typename T>
GF_FORCEINLINE GfIntrusiveList<T>::GfListEntry::GfListEntry()
	: m_prev(nullptr)
	, m_next(nullptr)
{
}

template<typename T>
GF_FORCEINLINE GfIntrusiveList<T>::Iterator::Iterator(GfListEntry* entry)
	: m_valPtr(entry)
{
}

template<typename T>
GF_FORCEINLINE GfIntrusiveList<T>::Iterator::Iterator()
	: m_valPtr(nullptr)
{
}

template<typename T>
GF_FORCEINLINE typename GfIntrusiveList<T>::Iterator& GfIntrusiveList<T>::Iterator::operator++()
{
	if (m_valPtr) 
	{
		m_valPtr = m_valPtr->getNext();
	}
	return *this;
}

template<typename T>
GF_FORCEINLINE bool GfIntrusiveList<T>::Iterator::operator==(const Iterator& other) const
{
	return m_valPtr == other.m_valPtr;
}

template<typename T>
GF_FORCEINLINE bool GfIntrusiveList<T>::Iterator::operator!=(const Iterator& other) const
{
	return !(*this == other);
}

template<typename T>
GF_FORCEINLINE T* GfIntrusiveList<T>::front()
{
	return static_cast<T*>(m_head);
}

template<typename T>
GF_FORCEINLINE const T* GfIntrusiveList<T>::front() const
{
	return static_cast<T*>(m_head);
}

template<typename T>
GF_FORCEINLINE T* GfIntrusiveList<T>::back()
{
	return static_cast<T*>(m_back);
}

template<typename T>
GF_FORCEINLINE const T* GfIntrusiveList<T>::back() const
{
	return static_cast<T*>(m_back);
}

template<typename T>
GF_FORCEINLINE bool GfIntrusiveList<T>::empty() const
{
	return m_size == 0;
}

template<typename T>
GF_FORCEINLINE u32 GfIntrusiveList<T>::size() const
{
	return m_size;
}

template<typename T>
GF_FORCEINLINE void GfIntrusiveList<T>::clear()
{
	m_size = 0;
	m_head = nullptr;
	m_back = nullptr;
}

template<typename T>
GF_FORCEINLINE void GfIntrusiveList<T>::insert(const u32 pos, T* val)
{
	GF_ASSERT(pos <= m_size, "Invalid position");
	if (pos == 0) 
	{
		GfListEntry* entry = m_head;
		m_head = val;
		m_head->m_next = entry;
		m_head->m_prev = nullptr;
		if (entry) 
		{
			entry->m_prev = m_head;
		}
	}
	else if (pos == m_size) 
	{
		GfListEntry* entry = m_back;
		m_back = val;
		m_back->m_next = nullptr;
		m_head->m_prev = entry;
		if (entry) 
		{
			entry->m_next = m_back;
		}
	}
	else 
	{
		GfListEntry* entry = m_head;
		for (u32 i=1; i<pos; ++i) 
		{
			entry = entry->m_next;
		}

		GfListEntry* nextEntry = m_head->m_next;
		entry->m_next = val;
		val->m_prev = entry;
		val->m_next = nextEntry;
		nextEntry->m_prev = val;
	}
	m_size++;
}

template<typename T>
GF_FORCEINLINE void GfIntrusiveList<T>::erase(const u32 pos)
{
	GF_ASSERT(pos < m_size, "Invalid position");
	if (pos == 0)
	{
		GfListEntry* entry = m_head;
		m_head = entry->m_next;
		if (m_head) 
		{
			m_head->m_prev = nullptr;
		}
		entry->m_next = nullptr;
		entry->m_prev = nullptr;
	}
	else if (pos == (m_size - 1)) 
	{
		GfListEntry* entry = m_back;
		m_back = entry->m_prev;
		if (m_back) 
		{
			m_back->m_next = nullptr;
		}
		entry->m_next = nullptr;
		entry->m_prev = nullptr;
	}
	else 
	{
		GfListEntry* entry = m_head;
		for (u32 i=0; i<pos; ++i) 
		{
			entry = entry->m_next;
		}
		entry->m_prev->m_next = entry->m_next;
		entry->m_next->m_prev = entry->m_prev;
		entry->m_next = nullptr;
		entry->m_prev = nullptr;
	}
	m_size--;
}

template<typename T>
GF_FORCEINLINE void GfIntrusiveList<T>::erase(T* val)
{
	if (val == m_head) 
	{
		m_head = m_head->m_next;
	} 
	else if (val == m_back) 
	{
		m_back = m_back->m_prev;
	}
	else 
	{
		if (val->m_next) 
		{
			val->m_next->m_prev = val->m_prev;
		}
		if (val->m_prev) 
		{
			val->m_prev->m_next = val->m_next;
		}
		val->m_prev = nullptr;
		val->m_next = nullptr;
	}
	m_size--;
}

template<typename T>
GF_FORCEINLINE void GfIntrusiveList<T>::pushFront(T* val)
{
	insert(0, val);
}

template<typename T>
GF_FORCEINLINE void GfIntrusiveList<T>::pushBack(T* val)
{
	insert(m_size, val);
}

template<typename T>
GF_FORCEINLINE void GfIntrusiveList<T>::popFront()
{
	erase(0);
}

template<typename T>
GF_FORCEINLINE void GfIntrusiveList<T>::popBack()
{
	erase(m_size - 1);
}

template<typename T>
GF_FORCEINLINE typename GfIntrusiveList<T>::Iterator GfIntrusiveList<T>::begin() const
{
	return Iterator(m_head); 
}

template<typename T>
GF_FORCEINLINE typename GfIntrusiveList<T>::Iterator GfIntrusiveList<T>::end() const
{
	return Iterator(nullptr); 
}

template<typename T>
GF_FORCEINLINE const T* GfIntrusiveList<T>::operator[](const u32 idx) const
{
	GF_ASSERT(idx < m_size, "Invalid index");
	GfListEntry* entry = m_head;
	for (u32 i = 0; i < idx; ++i) 
	{
		entry = entry->m_next;
	}
	return entry;
}

template<typename T>
GF_FORCEINLINE T* GfIntrusiveList<T>::operator[](const u32 idx)
{
	GF_ASSERT(idx < m_size, "Invalid index");
	GfListEntry* entry = m_head;
	for (u32 i = 0; i < idx; ++i) 
	{
		entry = entry->m_next;
	}
	return entry;
}


#endif
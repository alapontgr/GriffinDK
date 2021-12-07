////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/10/14
//	File: 	GfMemoryBase.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFMEMORYBASE_H__
#define __GFMEMORYBASE_H__
////////////////////////////////////////////////////////////////////////////////

#include <new.h>

////////////////////////////////////////////////////////////////////////////////

template <typename T, typename AlignT>
GF_FORCEINLINE T* GfAlign(T* pBase, AlignT uiAlign = 16)
{
	u64 ulBase((u64)pBase);
	u64 ulOffset = ulBase & (uiAlign - 1);
	return reinterpret_cast<T*>(ulBase + (ulOffset != 0 ? (uiAlign - ulOffset) : 0));
}

GF_FORCEINLINE u32 GfAlignUint(u32 val, u32 alignment = 16)
{
	u32 ulOffset = val & (alignment - 1);
	return val + (ulOffset != 0 ? (alignment - ulOffset) : 0);
}

////////////////////////////////////////////////////////////////////////////////

template <typename T>
inline bool GfIsAligned(const T* pData, u32 uiAlign = 16)
{
	return (reinterpret_cast<u64>(pData) & (uiAlign - 1)) == 0;
}

////////////////////////////////////////////////////////////////////////////////

class GfDefaultAllocator
{
public:

	static void* Alloc(size_t uiSize, size_t uiAlign = 16);

	static void Free(void* pAddr);

	template<typename T, typename... Args>
	static T* New(Args... args);

	template<typename T>
	static void Delete(T* pObj);
};

////////////////////////////////////////////////////////////////////////////////

template<typename T, typename... Args>
T* GfDefaultAllocator::New(Args... args)
{
	void* pMem(GfDefaultAllocator::Alloc(sizeof(T), alignof(T)));
	GF_ASSERT(pMem, "Failed to allocate memory");
	return new (pMem) T(args...);
}

////////////////////////////////////////////////////////////////////////////////

template<typename T>
void GfDefaultAllocator::Delete(T* pObj)
{
	if (pObj) 
	{
		pObj->~T();
		Free(pObj);
	}
}

////////////////////////////////////////////////////////////////////////////////

// Use this function to allocate from the stack (i.e tmp arrays of data)
void* GfAlloca(u32 size);
void GfFreea(void* data);

// Utility class
class StackMemBlock 
{
public:
	StackMemBlock(u32 size) : m_memBlock (GfAlloca(size)) 
	{
		GF_VERIFY(m_memBlock || size == 0, "Stack Overflow!!!");
	}
	~StackMemBlock() 
	{
		GfFreea(m_memBlock);
		m_memBlock = nullptr;
	}

	StackMemBlock(StackMemBlock&& other) 
		: m_memBlock(other.m_memBlock)
	{
		other.m_memBlock = nullptr;
	}

	void* get() const { return m_memBlock; }

private:

	StackMemBlock(const StackMemBlock&);
	StackMemBlock& operator=(const StackMemBlock&);

	void* m_memBlock;
};

////////////////////////////////////////////////////////////////////////////////

#include GF_SOLVE_PLATFORM_PATH(GfMemory/GfMemoryBase_Platform.inl)

////////////////////////////////////////////////////////////////////////////////
#endif // __GFMEMORY_H__
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

template <typename T>
GF_FORCEINLINE T* GfAlign(T* pBase, u32 uiAlign = 16)
{
	u64 ulBase((u64)pBase);
	u64 ulOffset = ulBase & (uiAlign - 1);
	return reinterpret_cast<T*>(ulBase + (ulOffset != 0 ? (uiAlign - ulOffset) : 0));
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
};

////////////////////////////////////////////////////////////////////////////////

template<typename T, typename... Args>
T* GfDefaultAllocator::New(Args... args)
{
	void* pMem(GfDefaultAllocator::Alloc(sizeof(T), alignof(T));
	GF_ASSERT(pMem, "Failed to allocate memory");
	return new (pMem) T(args);
}

////////////////////////////////////////////////////////////////////////////////

#include GF_SOLVE_PLATFORM_INLINE(GfMemoryBase)

////////////////////////////////////////////////////////////////////////////////
#endif // __GFMEMORY_H__
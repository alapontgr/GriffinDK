////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2018/10/14
//	File: 	GfMemoryShared.inl
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFMEMORYSHARED_INL__
#define __GFMEMORYSHARED_INL__
////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void* GfDefaultAllocator::Alloc(size_t uiSize, size_t uiAlign)
{
	return _aligned_malloc(uiSize, uiAlign);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void GfDefaultAllocator::Free(void* pAddr)
{
	_aligned_free(pAddr);
}

////////////////////////////////////////////////////////////////////////////////

GF_FORCEINLINE void* GfAlloca(u32 size) 
{
	return (size != 0) ? _malloca(static_cast<size_t>(size)) : nullptr;
}

GF_FORCEINLINE void GfFreea(void* data) 
{
	_freea(data);
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFMEMORYSHARED_H__
////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/03/28
//	File: 	GfPlatform.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFPLATFORM_H__
#define __GFPLATFORM_H__
////////////////////////////////////////////////////////////////////////////////

// Helper macro used for some platform abstraction
#define GF_DECLARE_PLATFORM_INTERFACE(TYPE)					\
	public:													\
	friend class TYPE##_Platform;							\
	using PlatformT = TYPE##_Platform;						\
	using PlatformRef = TYPE##_Platform&;					\
	using PlatformPtr = TYPE##_Platform*;					\
	PlatformT& Plat(){return m_kPlatform;}					\
	const PlatformT& Plat() const { return m_kPlatform; }	\
	PlatformT* PlatPtr() { return &m_kPlatform; }			\
	private:												\
	PlatformT	m_kPlatform;

////////////////////////////////////////////////////////////////////////////////
#endif // __GFPLATFORM_H__
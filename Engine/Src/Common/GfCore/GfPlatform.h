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
	using PlatformCRef = const TYPE##_Platform&;			\
	using PlatformPtr = TYPE##_Platform*;					\
	PlatformT& Plat(){return m_kPlatform;}					\
	const PlatformT& Plat() const { return m_kPlatform; }	\
	PlatformT* PlatPtr() { return &m_kPlatform; }			\
	protected:												\
	PlatformT	m_kPlatform;

/*
	Usage:
		GF_DEFINE_BASE_CTOR(MyType)
		, m_uiMemberInt(0)
		{}
*/
#define GF_DEFINE_BASE_CTOR(TYPE)	\
	TYPE::TYPE() : m_kPlatform(*this)

#define GF_DEFINE_BASE_CTOR_EXT(TYPE, ...)	\
	TYPE::TYPE(__VA_ARGS__) : m_kPlatform(*this)

////////////////////////////////////////////////////////////////////////////////

/*
	Declares members of the platform class to connect it with the base class
*/
#define  GF_DECLARE_PLATFORM_MEMBERS(TYPE)	\
	public:									\
	friend class TYPE;						\
	TYPE##_Platform(TYPE& kBase);			\
	private:								\
	TYPE& m_kBase;							

/*
	Usage:
		GF_DEFINE_PLATFORM_CTOR(MyType)
			, m_uiMemberInt(0)
		{}
*/
#define GF_DEFINE_PLATFORM_CTOR(TYPE)	\
	TYPE##_Platform::TYPE##_Platform(TYPE& kBase) : m_kBase(kBase)

////////////////////////////////////////////////////////////////////////////////
/*
	Usage:

	If the platform file uses a base class reference, "GF_DEFINE_BASE_CTOR" must be use to define the base class's constructor.
	Then, the platform class must use "GF_DECLARE_PLATFORM_MEMBERS" and "GF_DEFINE_PLATFORM_CTOR"

	Example:

	///////////////////////////////////////////////////////
	# MyType_Platform.h:

		class MyType_Platform
		{
			GF_DECLARE_PLATFORM_MEMBERS(MyType);
		public:
			...
			int m_uiA;
		};

	///////////////////////////////////////////////////////
	# MyType.h:

		#include GF_SOLVE_PLATFORM_HEADER(MyType)

		class MyType
		{
			GF_DECLARE_PLATFORM_INTERFACE(MyType);
		public:
			...
		};

	///////////////////////////////////////////////////////
	# MyType.cpp:

	GF_DEFINE_BASE_CTOR(MyType)
	{
	}

	///////////////////////////////////////////////////////
	# MyType_Platform.cpp:

		#include ".../MyType.h"

		GF_DEFINE_PLATFORM_CTOR(MyType)
			, m_uiA(10)
		{
		}
*/

////////////////////////////////////////////////////////////////////////////////
#endif // __GFPLATFORM_H__
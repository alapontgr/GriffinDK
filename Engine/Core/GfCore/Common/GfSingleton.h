////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/01/13
//	File: 	GfSingleton.h
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __GFSINGLETON_H__
#define __GFSINGLETON_H__
////////////////////////////////////////////////////////////////////////////////

template <typename T, typename AllocT = GfDefaultAllocator>
class GfPerThreadSingleton 
{
public:

	// Singleton access. Allow custom creation of the singleton instance
	static T* Get();

protected:

	GfPerThreadSingleton();
	GfPerThreadSingleton(const GfPerThreadSingleton&);
	GfPerThreadSingleton& operator=(const GfPerThreadSingleton&);
};

////////////////////////////////////////////////////////////////////////////////

template <typename T, typename AllocT = GfDefaultAllocator>
class GfSingleton
{
public:

	// Singleton access. Allow custom creation of the singleton instance
	static T* Get(T* pUserInstance = nullptr);

protected:

	GfSingleton();
	GfSingleton(const GfSingleton&);
	GfSingleton& operator=(const GfSingleton&);

	static T* ms_pInstance;
};

////////////////////////////////////////////////////////////////////////////////

template <typename T, typename AllocT = GfDefaultAllocator>
GfPerThreadSingleton<T, AllocT>::GfPerThreadSingleton() {}

////////////////////////////////////////////////////////////////////////////////

template <typename T, typename AllocT = GfDefaultAllocator>
T* GfPerThreadSingleton<T, AllocT>::Get()
{
	thread_local static T ms_kInstance;
	return &ms_kInstance;
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, typename AllocT = GfDefaultAllocator>
GfSingleton<T, AllocT>::GfSingleton()
	: m_pInstance(nullptr)
{}

////////////////////////////////////////////////////////////////////////////////

template <typename T, typename AllocT = GfDefaultAllocator>
T* GfSingleton<T, AllocT>::Get(T* pUserInstance)
{
	if (!ms_pInstance)
	{
		// TODO: Replace this with a proper allocator. Fallback to block allocator in the future
		if (pUserInstance)
		{
			ms_pInstance = pUserInstance;
		}
		else
		{
			ms_pInstance = new T();
		}
	}
	return ms_pInstance;
}

////////////////////////////////////////////////////////////////////////////////
#endif // __GFSINGLETON_H__
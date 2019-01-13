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
	T* Get();

private:

	GfPerThreadSingleton();
	GfPerThreadSingleton(const GfPerThreadSingleton&);
	GfPerThreadSingleton& operator=(const GfPerThreadSingleton&);

	static thread_local T* ms_pInstance;
};

////////////////////////////////////////////////////////////////////////////////

template <typename T, typename AllocT = GfDefaultAllocator>
class GfSingleton
{
public:

	// Singleton access. Allow custom creation of the singleton instance
	T* Get(T* pUserInstance = nullptr);

private:

	GfSingleton();
	GfSingleton(const GfSingleton&);
	GfSingleton& operator=(const GfSingleton&);

	static T* ms_pInstance;
};

////////////////////////////////////////////////////////////////////////////////

template <typename T, typename AllocT = GfDefaultAllocator>
GfPerThreadSingleton::GfPerThreadSingleton() 
	: m_pInstance(nullptr)
{}

////////////////////////////////////////////////////////////////////////////////

template <typename T, typename AllocT = GfDefaultAllocator>
T* GfPerThreadSingleton::Get() 
{
	if (!ms_pInstance) 
	{
		// TODO: Replace this with a proper allocator. Fallback to block allocator in the future
		ms_pInstance = new T();
	}
	return ms_pInstance;
}

////////////////////////////////////////////////////////////////////////////////

template <typename T, typename AllocT = GfDefaultAllocator>
GfSingleton::GfSingleton()
	: m_pInstance(nullptr)
{}

////////////////////////////////////////////////////////////////////////////////

template <typename T, typename AllocT = GfDefaultAllocator>
T* GfSingleton::Get(T* pUserInstance)
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
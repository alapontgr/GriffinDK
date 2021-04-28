////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2020/01/10
//	File: 	GfCommandLine.cpp
//
//	Copyright (c) 2020 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////

#include "Common/GfCore/GfHash.h"

////////////////////////////////////////////////////////////////////////////////

constexpr u64 HASH_M = 0xc6a4a7935bd1e995;
constexpr u64 HASH_R = 47;

static u64 appendHash(const void* buffer, u64 bufferSize, u64 h)
{
	const u64* data = static_cast<const u64*>(buffer);
	const u64* const end = data + (bufferSize / sizeof(u64));

	while(data != end)
	{
		u64 k = *data++;

		k *= HASH_M;
		k ^= k >> HASH_R;
		k *= HASH_M;

		h ^= k;
		h *= HASH_M;
	}

	const u8* data2 = reinterpret_cast<const u8*>(data);

	switch(bufferSize & (sizeof(u64) - 1))
	{
	case 7:
		h ^= u64(data2[6]) << 48;
	case 6:
		h ^= u64(data2[5]) << 40;
	case 5:
		h ^= u64(data2[4]) << 32;
	case 4:
		h ^= u64(data2[3]) << 24;
	case 3:
		h ^= u64(data2[2]) << 16;
	case 2:
		h ^= u64(data2[1]) << 8;
	case 1:
		h ^= u64(data2[0]);
		h *= HASH_M;
	};

	h ^= h >> HASH_R;
	h *= HASH_M;
	h ^= h >> HASH_R;

	GF_ASSERT(h != 0, "Invalid hash");
	return h;
}

static u32 appendHash(const void* buffer, u32 bufferSize, u32 h)
{
	const u32* data = static_cast<const u32*>(buffer);
	const u32* const end = data + (bufferSize / sizeof(u32));

	while(data != end)
	{
		u32 k = *data++;

		k *= HASH_M;
		k ^= k >> HASH_R;
		k *= HASH_M;

		h ^= k;
		h *= HASH_M;
	}

	const u8* data2 = reinterpret_cast<const u8*>(data);

	switch(bufferSize & (sizeof(u32) - 1))
	{
	case 3:
		h ^= u32(data2[2]) << 16;
	case 2:
		h ^= u32(data2[1]) << 8;
	case 1:
		h ^= u32(data2[0]);
		h *= u32(HASH_M);
	};

	h ^= h >> u32(HASH_R);
	h *= u32(HASH_M);
	h ^= h >> u32(HASH_R);

	GF_ASSERT(h != 0, "Invalid hash");
	return h;
}

u64 GfHash::compute(const void* buffer, u64 size, u64 seed)
{
	const u64 h = seed ^ (size * HASH_M);
	return appendHash(buffer, size, h);
}

u64 GfHash::compute(const GfString& str, u64 seed)
{
	const u64 strSize = u64(str.size());
	const u64 h = seed ^ (strSize * HASH_M);
	return appendHash(str.data(), strSize, h);
}

u32 GfHash::compute32(const void* buffer, u32 size, u32 seed)
{
	const u32 h = seed ^ (size * HASH_M);
	return appendHash(buffer, size, h);
}

u32 GfHash::compute32(const GfString& str, u32 seed)
{
	const u32 strSize = u32(str.size());
	const u32 h = seed ^ (strSize * u32(HASH_M));
	return appendHash(str.data(), strSize, h);
}

////////////////////////////////////////////////////////////////////////////////
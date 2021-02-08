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
#include <stack>
#include <queue>
#include <map>
#include <unordered_map>
#include <array>

////////////////////////////////////////////////////////////////////////////////

// Vector
template <typename... Ts>
using GfVector = std::vector<Ts...>;

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
#endif // __GFSTL_H__
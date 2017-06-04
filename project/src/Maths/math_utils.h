#pragma once

#include "Utilities/types.h"

namespace fdk
{
namespace Maths
{
	template <typename T>
	T clamp(T value, T min, T max)
	{
		return value < min ? min : value > max ? max : value;
	}

	template <typename T>
	T lerp(T from, T to, T delta)
	{
		return from + (to - from) * delta;
	}

	template <typename T>
	T wrap(T value, T min, T max)
	{
		T diff = max - min;
		return value < min ? (value + diff) : value >= max ? value - diff : value;
	}

	inline f32 mysign(f32 a) { return a >= 0.0f ? 1.0f : -1.0f; }
}
}
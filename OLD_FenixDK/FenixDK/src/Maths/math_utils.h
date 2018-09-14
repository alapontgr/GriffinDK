#pragma once

#include "Utilities/types.h"

namespace fdk
{
namespace Maths
{

  static constexpr f32 kPI = 3.14159265359f;
  
  static constexpr f32 k2PI = kPI * 2.0f;

  static constexpr f32 kPI2 = kPI * 0.5f;

  static constexpr f32 kPI4 = kPI * 0.25f;

  // Use: X from Deg to Rad -> X * kDegToRad
  static constexpr f32 kDegToRad = kPI / 180.0f;

  // Use: X from Rad to Deg -> X * kRadToDeg
  static constexpr f32 kRadToDeg = 180.0f / kPI;

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

	inline f32 my_sign(f32 a) { return a >= 0.0f ? 1.0f : -1.0f; }
}
}
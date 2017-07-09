#pragma once

#include <glm/glm.hpp>

#include <assert.h>
#include <cinttypes>

namespace fdk
{
#define FDK_ASSERT(expr, msg) assert(expr&& msg)
#define FDK_ABORT(msg) quick_exit(EXIT_SUCCESS)
#define UNUSED(arg) (void)(arg)

typedef std::uint8_t u8;
typedef std::uint16_t u16;
typedef std::uint32_t u32;
typedef std::uint64_t u64;

typedef std::int8_t s8;
typedef std::int16_t s16;
typedef std::int32_t s32;
typedef std::int64_t s64;

typedef float f32;
typedef double f64;

typedef glm::vec2 v2;
typedef glm::vec3 v3;
typedef glm::vec4 v4;
typedef glm::quat quat;
typedef glm::mat3 m3;
typedef glm::mat4 m4;

void debug_msg(const char* format, ...);
}
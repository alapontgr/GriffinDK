#pragma once

#include <stdio.h>

#include "VulkanInclude.h"
#include "types.h"

namespace fdk {
namespace Utilities {

// Windows definitions
#if defined(Win32) || defined(x64)

#define WinPlatform

#endif // end Windows definitions

#if defined(Unix)
#define  UnixPlatform
#endif

#define VK_CHECK(result, failMsg) FDK_ASSERT(result == VkResult::VK_SUCCESS, failMsg)

#define PATH const char*

constexpr char *kEngineName = "FenixEngine";
constexpr u32 kEngineVersion = VK_MAKE_VERSION(0, 0, 1);
}
}
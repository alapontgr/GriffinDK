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

#define VK_CHECK(result, failMsg) FDASSERT(result == VkResult::VK_SUCCESS, failMsg)

#define PATH const char*

constexpr char *kEngineName = "FenixEngine";
constexpr u32 kEngineVersion = VK_MAKE_VERSION(0, 0, 1);


#define IMPLEMENTATION_TYPE(API, ClassName) API##ClassName
#define IMPL_CONCAT(API, ClassName) IMPLEMENTATION_TYPE(API, ClassName)

// Define used to get the base class of the API dependent classes (Material, Buffer, ...)
#define IMPLEMENTATION(ClassName) IMPL_CONCAT(GPUIMPL_ID, ClassName)

#ifdef WinPlatform

#define GPUIMPL_ID Vulkan::VK_

#endif // WinPlatform

}
}

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


#define IMPLEMENTATION_TYPE(API, ClassName) API##ClassName
#define IMPL_CONCAT(API, ClassName) IMPLEMENTATION_TYPE(API, ClassName)
#define CLASS_PTR(ClassName) ClassName##*

// Used to get the name of the class that implements internally the specified class
#define IMPL_NAME(ClassName) IMPL_CONCAT(GPUIMPL_ID, ClassName)
// Get the name of a pointer to the class that implements internally the specified class
#define IMPL_PTR(ClassName) CLASS_PTR(IMPL_NAME(ClassName))

//#define IMPLEMENTATION(ClassName, pObj) reinterpret_cast<IMPL_PTR(ClassName)>(pObj)
#define IMPLEMENTATION(ClassName) IMPL_CONCAT(GPUIMPL_ID, ClassName)


#ifdef WinPlatform

#define FENIX_VK_IMPL // This macro enables the implementation of vulkan
#define GPUIMPL_ID VK_

#endif // WinPlatform

}
}

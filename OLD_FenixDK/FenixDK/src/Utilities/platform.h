#pragma once

#include <stdio.h>

#include "VulkanInclude.h"
#include "types.h"

namespace fdk
{
namespace Utilities
{

// Windows definitions
#if defined(Win32) || defined(x64)

#define WinPlatform

#endif // end Windows definitions

#if defined(Unix)
#define UnixPlatform
#endif

#define VK_CHECK(result, failMsg) FDASSERT(result == VkResult::VK_SUCCESS, failMsg)

#define PATH const char*

	constexpr char* kEngineName = "FenixEngine";
	constexpr u32 kEngineVersion = VK_MAKE_VERSION(0, 0, 1);

#define IMPLEMENTATION_TYPE(API, ClassName) API##ClassName
#define IMPL_CONCAT(API, ClassName) IMPLEMENTATION_TYPE(API, ClassName)

// Define used to get the base class of the API dependent classes (Material, Buffer, ...)
#define IMPLEMENTATION(ClassName) IMPL_CONCAT(GPUIMPL_ID, ClassName)

#define STRINGIFY(ARG) #ARG
#define CONCAT(A, B) A##B

#define CONCAT_INCL_FILE(Prefix, Class) Prefix##Class.h
#define INCL_STRINGIFY_FILE(Path) STRINGIFY(Path)
#define INCL_GET_FILE(Prefix, Class) INCL_STRINGIFY_FILE(CONCAT_INCL_FILE(Prefix, Class))
// Macro used to include the implementation of a class depending on the Renderer
#define INCLUDE_IMPLEMENTATION(ClassName) INCL_GET_FILE(GPUINCL_PREFIX, ClassName)

#ifdef FENIX_VK_IMPL

#define GPUIMPL_ID Vulkan::VK_
#define GPUINCL_PREFIX Vulkan/VK_

#endif // WinPlatform
}
}

#pragma once
#include "Utilities\platform.h"

namespace fdk
{
namespace Rendering
{
#define IMPLEMENTATION_TYPE(API, ClassName) API##ClassName
#define IMPL_CONCAT(API, ClassName) IMPLEMENTATION_TYPE(API, ClassName)
#define CLASS_PTR(ClassName) ClassName##*

  // Used to get the name of the class that implements internally the specified class
#define IMPL_NAME(ClassName) IMPL_CONCAT(GPUIMPL_ID, ClassName)
  // Get the name of a pointer to the class that implements internally the specified class
#define IMPL_PTR(ClassName) CLASS_PTR(IMPL_NAME(ClassName))

#define IMPLEMENTATION(ClassName, pObj) reinterpret_cast<IMPL_PTR(ClassName)>(pObj)


#ifdef WinPlatform
#define GPUIMPL_ID VK
#endif // WinPlatform
}
}
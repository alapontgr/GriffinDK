#include "types.h"

#include <windows.h>

#include <debugapi.h>
#include <stdarg.h>
#include <stdio.h>

namespace fdk {
void debug_msg(const char *format, ...) {
#ifdef _DEBUG
  constexpr u32 kMaxSize = 256;
  char buffer[kMaxSize];
  va_list vl;
  va_start(vl, format);
  vsprintf_s(buffer, kMaxSize, format, vl);
  va_end(vl);
#ifdef _WIN32
  OutputDebugString(buffer); // For Visual studio debug.
  puts(buffer);              // and to console.
#endif                       // _WIN32

#else
  UNUSED(format);
#endif // _DEBUG
}
}

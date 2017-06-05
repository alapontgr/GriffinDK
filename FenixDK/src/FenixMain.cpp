#include <windows.h>
#include "Entry.h"

#ifdef DEAD
fdk::s32 main() 
{
  TestApp app;
  app.run("Test Vulkan Application");

  return 0;
}
#endif // DEAD

int WINAPI WinMain(HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR lpCmdLine,
  int nCmdShow)
{
  return fdk::Framework::__fenix_entry__();
}
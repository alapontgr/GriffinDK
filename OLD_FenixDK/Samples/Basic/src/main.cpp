#include "TestApp.h"

s32 fdk::Framework::__fenix_entry__() 
{
  TestApp app;
  app.run("Test Vulkan Application");
  return 0;
}
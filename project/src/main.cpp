#include <windows.h>

#include "Utilities/types.h" 
#include "Framework/app.h"
#include "Rendering/context.h"

class TestApp : public fdk::Framework::App
{
public:
  TestApp() {}
  virtual ~TestApp() {}

  virtual void on_config_window() override
  {
    s_width = 512;//1280;
    s_height = 512;//720;
  }

  virtual void on_start() override
  {

  }

  virtual void on_update() override
  {

  }

  virtual void on_render() override
  {
    m_context.test_render(total_time_secs());
  }

  virtual void on_release() override
  {

  }
};


// Recommended tutorial
// https://software.intel.com/en-us/articles/api-without-secrets-introduction-to-vulkan-part-1
int WINAPI WinMain(HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR lpCmdLine,
  int nCmdShow)
{
  //ParticleApp app;
  //app.run("GPU Particles example", hInstance, nCmdShow);
  TestApp app;
  app.run("Test Vulkan Application", hInstance, nCmdShow);

  return 0;
}
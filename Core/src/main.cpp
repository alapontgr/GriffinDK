#include <windows.h>

#include "Utilities/types.h" 
#include "Framework/app.h"
#include "Rendering/context.h"
#include "Framework/Time.h"


using namespace fdk;

class TestApp : public fdk::Framework::App
{
public:
  TestApp() {}
  virtual ~TestApp() {}

  virtual void on_pre_init() override
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
    auto time = Framework::Time::total_time_secs();
    m_context.test_render(time);
  }

  virtual void on_release() override
  {

  }
};

int WINAPI WinMain(HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR lpCmdLine,
  int nCmdShow)
{
  TestApp app;
  app.run("Test Vulkan Application");

  return 0;
}
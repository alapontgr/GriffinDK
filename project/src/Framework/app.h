#pragma once

#include "Rendering/context.h"
#include "Rendering/RenderInterface.h"
#include "Utilities/types.h"

namespace fdk {
namespace Framework {

class App {
public:
  App();
  virtual ~App();

  static u32 width() { return s_width; }
  static u32 height() { return s_height; }

  void run(const char *title, HINSTANCE hInstance, int nCmdShow);

  // Used to edit some of the initial parameter of the application
  virtual void on_pre_init() = 0;

  virtual void on_start() = 0;
  
  virtual void on_update() = 0;
  
  virtual void on_render() = 0;
  
  virtual void on_release() = 0;

private:
  void init(const char *title, HINSTANCE hInstance, int nCmdShow);
  void frame();
  void update();
  void render();
  void release();

protected:
  Rendering::RenderInterface* m_pRenderInterface;
  Rendering::Context m_context;
  static u32 s_width;
  static u32 s_height;
  bool m_fullscreen = false;
  bool m_vSync = false;
  v4 m_clearColor = v4(0.5f, 0.5f, 1.0f, 1.0f);
  HWND m_hwnd;
};
}
}
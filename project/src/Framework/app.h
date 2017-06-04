#pragma once

#include "Rendering/context.h"
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
  virtual void on_config_window() = 0;
  virtual void on_start() = 0;
  virtual void on_update() = 0;
  virtual void on_render() = 0;
  virtual void on_release() = 0;

  f64 delta_time_secs();
  f64 delta_time_mili();
  f64 delta_time_micro();

  f64 total_time_secs();
  f64 total_time_mili();
  f64 total_time_micro();

  f64 update_time();
  f64 render_time();

private:
  void init(const char *title, HINSTANCE hInstance, int nCmdShow);
  void frame();
  void update();
  void render();
  void release();

protected:
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
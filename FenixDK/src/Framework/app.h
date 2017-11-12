#pragma once

#include "Utilities/types.h"

namespace fdk {
namespace Framework {

class App {
public:
  App();
  virtual ~App();

  static u32 width() { return s_width; }
  
  static u32 height() { return s_height; }

  static f32 fwidth() { return static_cast<f32>(s_width); }

  static f32 fheight() { return static_cast<f32>(s_height); }

  void run(const char *title);

  // Used to edit some of the initial parameter of the application
  virtual void on_pre_init() = 0;

  virtual void on_start() = 0;
  
  virtual void on_update() = 0;
  
  virtual void on_render() = 0;
  
  virtual void on_release() = 0;

private:
  void init(const char *title);
  void frame();
  void update();
  void render();
  void release();

protected:

  static u32 s_width;
  static u32 s_height;
  bool m_fullscreen = false;
  bool m_vSync = false;
  v4 m_clearColor = v4(0.5f, 0.5f, 1.0f, 1.0f);
};
}
}
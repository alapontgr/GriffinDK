#pragma once
#include "Utilities\types.h"

namespace fdk
{
  namespace Rendering 
  {
    class Camera 
    {
    public:

      Camera();

      void setup_perspective(const f32 radFovy, const f32 aspect, const f32 near, const f32 far);

      void set_view_matrix(const v3& eyePos, const v3& targetPos, const v3& up);

      void update_matrices();

      m4 view() const { return m_view; }

      m4 projection() const { return m_projection; }

      m4 view_projection() const { return m_viewProjection; }

    private:
    
      m4 m_view;
      m4 m_projection;
      m4 m_viewProjection;
    };

  }
}

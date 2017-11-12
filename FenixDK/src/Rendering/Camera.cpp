#include "Camera.h"

namespace fdk
{
namespace Rendering
{

	Camera::Camera()
	{
	}

	void Camera::setup_perspective(const f32 fovy, const f32 aspect, const f32 near, const f32 far)
	{
		m_projection = FDMath::perspective(fovy, aspect, near, far);
	}

	void Camera::set_view_matrix(const v3& eyePos, const v3& targetPos, const v3& up)
	{
    m_view = FDMath::lookAt(eyePos, targetPos, up);
  }

  void Camera::update_matrices()
  {
    m_viewProjection = m_projection * m_view;
  }

}
}

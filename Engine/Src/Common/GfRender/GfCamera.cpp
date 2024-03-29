////////////////////////////////////////////////////////////////////////////////
//
//	Author: Sergio Alapont Granero (seralgrainf@gmail.com)
//	Date: 	2019/04/10
//	File: 	GfCamera.cpp
//
//	Copyright (c) 2018 (See README.md)
//
////////////////////////////////////////////////////////////////////////////////
// Includes

#include "Common/GfRender/GfCamera.h"
#include "Common/GfCore/GfMaths.h"

#include "glm/gtc/matrix_transform.hpp"

////////////////////////////////////////////////////////////////////////////////

GfCamera::GfCamera()
{
}

////////////////////////////////////////////////////////////////////////////////

void GfCamera::UpdatePerspective(f32 fFov, f32 fAspect, f32 fNear, f32 fFar)
{
	m_mProjection = glm::perspective(fFov, fAspect, fNear, fFar);
}

////////////////////////////////////////////////////////////////////////////////

void GfCamera::UpdateViewWithTarget(const v3& vPos, const v3& vTarget, const v3& vUp)
{
	m_mView = glm::lookAt(vPos, vTarget, vUp);
}

////////////////////////////////////////////////////////////////////////////////

void GfCamera::UpdateViewWithForward(const v3& vPos, const v3& vForward, const v3& vUp)
{
	v3 vTarget = vPos + vForward;
	m_mView = glm::lookAt(vPos, vTarget, vUp);
}

////////////////////////////////////////////////////////////////////////////////
// EOF
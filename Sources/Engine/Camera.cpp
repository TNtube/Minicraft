#include "pch.h"
#include "Camera.h"

Camera::Camera(float ar, float fov, float _near, float _far) : m_ar(ar), m_fov(fov), m_near(_near), m_far(_far) {

	UpdateViewMatrix();
	UpdateProjectionMatrix();
}

void Camera::SetPosition(const Vector3& pos)
{
	m_position = pos;
	UpdateViewMatrix();
}

void Camera::SetTarget(const Vector3& target)
{
	m_target = target;
	UpdateViewMatrix();
}

void Camera::SetAspectRatio(float ar)
{
	m_ar = ar;
	UpdateProjectionMatrix();
}

const Matrix& Camera::GetViewMatrix() const
{
	return m_view; 
}

const Matrix& Camera::GetProjectionMatrix() const
{
	return m_projection;
}

void Camera::UpdateViewMatrix()
{
	if (m_position == m_target) {
		// m_target.z += 1.0f; // Adjust target to avoid zero direction vector
	}
	m_view = Matrix::CreateLookAt(m_position, m_target, Vector3::Up);
}

void Camera::UpdateProjectionMatrix()
{
	m_projection = Matrix::CreatePerspectiveFieldOfView(DirectX::XMConvertToRadians(m_fov), m_ar, m_near, m_far);
}

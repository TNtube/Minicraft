#pragma once
using namespace DirectX::SimpleMath;

class Camera
{
public:
	Camera(float ar, float fov, float _near, float _far);

	void SetPosition(const Vector3& pos);
	void SetTarget(const Vector3& target);

	void SetAspectRatio(float ar);

	const Matrix& GetViewMatrix() const;
	const Matrix& GetProjectionMatrix() const;

private:
	void UpdateViewMatrix();
	void UpdateProjectionMatrix();
	
	float m_ar;
	float m_fov;
	float m_near;
	float m_far;

	Vector3 m_position {0, 0, -1};
	Vector3 m_target;

	Matrix m_view;
	Matrix m_projection;
};

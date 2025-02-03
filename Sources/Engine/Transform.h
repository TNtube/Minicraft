#pragma once

class Transform
{
public:
	Transform();
	DirectX::SimpleMath::Vector3	position;
	DirectX::SimpleMath::Quaternion	rotation;
	DirectX::SimpleMath::Vector3	scale;

	void SetEulerAngles(float x, float y, float z);

	DirectX::SimpleMath::Matrix GetTransformMatrix() const;
};

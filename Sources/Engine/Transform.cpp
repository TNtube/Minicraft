#include "pch.h"
#include "Transform.h"

Transform::Transform() : scale(1, 1, 1)
{
	
}

void Transform::SetEulerAngles(float x, float y, float z)
{
	rotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(y, x, z);
}

DirectX::SimpleMath::Matrix Transform::GetTransformMatrix() const
{
	return DirectX::SimpleMath::Matrix::CreateScale(scale)
		 * DirectX::SimpleMath::Matrix::CreateFromQuaternion(rotation)
		 * DirectX::SimpleMath::Matrix::CreateTranslation(position);
}

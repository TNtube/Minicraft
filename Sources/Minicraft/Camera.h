#pragma once

#include "Engine/Buffers.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Camera {
	float fov;
	float nearPlane = 0.01f;
	float farPlane = 1000.0f;
	
	Vector3 camPos = Vector3(0, 0, 0);
	Quaternion camRot = Quaternion::Identity;
	Matrix projection;
	Matrix view;

	int lastMouseX = 0;
	int lastMouseY = 0;

	struct MatrixData {
		Matrix mView;
		Matrix mProjection;
		Vector4 camPos;
	};
	std::unique_ptr<ConstantBuffer<MatrixData>> cbCamera = nullptr;
public:
	Camera(float fov, float aspectRatio);
	~Camera();

	void UpdateAspectRatio(float aspectRatio);
	void Update(float dt, DirectX::Keyboard::State kb, DirectX::Mouse* mouse);

	void SetPosition(Vector3 pos) { camPos = pos; }

	void ApplyCamera(DeviceResources* deviceRes);
};
#include "pch.h"

#include "Camera.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Camera::Camera(float fov, float aspectRatio) : fov(fov), camRot(Quaternion::Identity) {
	Vector3 forward = Vector3::Transform(Vector3::Forward, camRot);
	view = Matrix::CreateLookAt(camPos, camPos + forward, Vector3::Up);

	projection = Matrix::CreatePerspectiveFieldOfView(fov, aspectRatio, nearPlane, farPlane);
}

Camera::~Camera() = default;

void Camera::UpdateAspectRatio(float aspectRatio) {
	projection = Matrix::CreatePerspectiveFieldOfView(fov, aspectRatio, nearPlane, farPlane);
}

void Camera::Update(float dt, Keyboard::State kb, Mouse* mouse) {
	float camSpeedRot = 0.25f;
	float camSpeedMouse = 10.0f;
	float camSpeed = 15.0f;
	if (kb.LeftShift) camSpeed *= 2.0f;

	Mouse::State mstate = mouse->GetState();
	const Matrix im = view.Invert();

	// TP: deplacement par clavier
	if (kb.Z) camPos += Vector3::TransformNormal(Vector3::Forward, im) * camSpeed * dt;
	if (kb.S) camPos += Vector3::TransformNormal(Vector3::Backward, im) * camSpeed * dt;
	if (kb.Q) camPos += Vector3::TransformNormal(Vector3::Left, im) * camSpeed * dt;
	if (kb.D) camPos += Vector3::TransformNormal(Vector3::Right, im) * camSpeed * dt;
	if (kb.A) camPos += Vector3::TransformNormal(Vector3::Down, im) * camSpeed * dt;
	if (kb.E) camPos += Vector3::TransformNormal(Vector3::Up, im) * camSpeed * dt;

	// astuce: Vector3::TransformNormal(vecteur, im); transforme un vecteur de l'espace cameravers l'espace monde

	if (mstate.positionMode == Mouse::MODE_RELATIVE) {
		float dx = mstate.x;
		float dy = mstate.y;

		if (mstate.rightButton) { 
			camPos += Vector3::TransformNormal(Vector3::Right, im) * -dx * camSpeedMouse * dt;
			camPos += Vector3::TransformNormal(Vector3::Up, im) * dy * camSpeedMouse * dt;

		} else if (mstate.leftButton) {
			camRot *= Quaternion::CreateFromAxisAngle(Vector3::Up, -dx * camSpeedRot * dt);
			camRot *= Quaternion::CreateFromAxisAngle(Vector3::Right, -dy * camSpeedRot * dt);
		} else {
			mouse->SetMode(Mouse::MODE_ABSOLUTE);
		}
	} else if (mstate.rightButton || mstate.leftButton) {
		mouse->SetMode(Mouse::MODE_RELATIVE);
	}

	Vector3 forward = Vector3::Transform(Vector3::Forward, camRot);
	view = Matrix::CreateLookAt(camPos, camPos + forward, Vector3::Up);
}

void Camera::ApplyCamera(DeviceResources* deviceRes) {
	if (!cbCamera) {
		cbCamera = std::make_unique<ConstantBuffer<MatrixData>>();
		cbCamera->Create(deviceRes);
	}

	MatrixData md;
	md.mView = view.Transpose();
	md.mProjection = projection.Transpose();
	cbCamera->UpdateSubResource(deviceRes, md);
	cbCamera->Bind(deviceRes, 1);
}
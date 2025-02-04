//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include "Cube.h"
#include "PerlinNoise.hpp"
#include "Engine/Camera.h"
#include "Engine/Shader.h"
#include "Engine/Transform.h"

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;


struct ModelData
{
	Matrix model;
};

struct CameraData
{
	Matrix view;
	Matrix projection;
};

// Global stuff
Shader* basicShader;

ConstantBuffer<ModelData> modelConstantBuffer;

// Game
Game::Game() noexcept(false) {
	m_deviceResources = std::make_unique<DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_D32_FLOAT, 2);
	m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game() {
	delete basicShader;
	g_inputLayouts.clear();
}

std::vector<uint32_t> indices;

void Game::Initialize(HWND window, int width, int height) {
	// Create input devices
	m_gamePad = std::make_unique<GamePad>();
	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);

	// Initialize the Direct3D resources
	m_deviceResources->SetWindow(window, width, height);
	m_deviceResources->CreateDeviceResources();
	m_deviceResources->CreateWindowSizeDependentResources();

	basicShader = new Shader(L"Basic");
	basicShader->Create(m_deviceResources.get());

	GenerateInputLayout<VertexLayout_PositionUV>(m_deviceResources.get(), basicShader);

	m_cube = std::make_unique<Cube>(m_deviceResources.get());

	modelConstantBuffer.Create(m_deviceResources.get());

	m_camera = std::make_unique<Camera>(XMConvertToRadians(80.0f), static_cast<float>(width) / static_cast<float>(height));
}

void Game::Tick() {
	// DX::StepTimer will compute the elapsed time and call Update() for us
	// We pass Update as a callback to Tick() because StepTimer can be set to a "fixed time" step mode, allowing us to call Update multiple time in a row if the framerate is too low (useful for physics stuffs)
	m_timer.Tick([&]() { Update(m_timer); });


	Render(m_timer);
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer) {
	auto const kb = m_keyboard->GetState();
	// auto const ms = m_mouse->GetState();

	
	
	// add kb/mouse interact here
	
	if (kb.Escape)
		ExitGame();


	m_camera->Update(timer.GetElapsedSeconds(), kb, m_mouse.get());

	// constexpr float rotateSpeed = 2.0f;
	// m_camera->SetPosition(Vector3(5.0f * sin(timer.GetTotalSeconds()*rotateSpeed), 2.0f,  5.0f * cos(timer.GetTotalSeconds()*rotateSpeed)));
	// // m_camera->SetPosition(Vector3(0, 2, -5));
	//
	// m_camera->SetTarget(Vector3(0, 0, 0));
	
	auto const pad = m_gamePad->GetState(0);
}

// Draws the scene.
void Game::Render(DX::StepTimer const& timer) {
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
		return;

	auto context = m_deviceResources->GetD3DDeviceContext();
	auto renderTarget = m_deviceResources->GetRenderTargetView();
	auto depthStencil = m_deviceResources->GetDepthStencilView();
	auto const viewport = m_deviceResources->GetScreenViewport();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->RSSetViewports(1, &viewport);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);
	
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ApplyInputLayout<VertexLayout_PositionUV>(m_deviceResources.get());

	basicShader->Apply(m_deviceResources.get());


	m_cube->Draw(m_deviceResources.get());
	ModelData md;
	md.model = m_cube->transform.GetTransformMatrix().Transpose();
	modelConstantBuffer.UpdateSubResource(m_deviceResources.get(), md);
	modelConstantBuffer.Bind(m_deviceResources.get(), 0);

	m_camera->ApplyCamera(m_deviceResources.get());

	context->DrawIndexed(indices.size(), 0, 0);

	// envoie nos commandes au GPU pour etre afficher � l'�cran
	m_deviceResources->Present();

	m_cube->transform.position = Vector3::Forward * 3;
}


#pragma region Message Handlers
void Game::OnActivated() {}

void Game::OnDeactivated() {}

void Game::OnSuspending() {}

void Game::OnResuming() {
	m_timer.ResetElapsedTime();
}

void Game::OnWindowMoved() {
	auto const r = m_deviceResources->GetOutputSize();
	m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange() {
	m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height) {
	if (!m_deviceResources->WindowSizeChanged(width, height))
		return;

	// The windows size has changed:
	// We can realloc here any resources that depends on the target resolution (post processing etc)
	m_camera->UpdateAspectRatio(static_cast<float>(width) / height);
}

void Game::OnDeviceLost() {
	// We have lost the graphics card, we should reset resources [TODO]
}

void Game::OnDeviceRestored() {
	// We have a new graphics card context, we should realloc resources [TODO]
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept {
	width = 800;
	height = 600;
}

#pragma endregion

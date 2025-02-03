//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include "PerlinNoise.hpp"
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

ComPtr<ID3D11Buffer> vertexBuffer;
ComPtr<ID3D11Buffer> indexBuffer;
ComPtr<ID3D11Buffer> modelConstantBuffer;
ComPtr<ID3D11Buffer> cameraConstantBuffer;
ComPtr<ID3D11InputLayout> inputLayout;

Transform front;

Matrix view;
Matrix projection;

// Game
Game::Game() noexcept(false) {
	m_deviceResources = std::make_unique<DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_D32_FLOAT, 2);
	m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game() {
	delete basicShader;
	g_inputLayouts.clear();
}

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

	auto device = m_deviceResources->GetD3DDevice();

	const std::vector<D3D11_INPUT_ELEMENT_DESC> InputElementDescs = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	device->CreateInputLayout(
		InputElementDescs.data(), InputElementDescs.size(),
		basicShader->vsBytecode.data(), basicShader->vsBytecode.size(),
		inputLayout.ReleaseAndGetAddressOf());

	std::vector vertices = {
		-0.5f,  0.5f, 0.0f,    //  1.0f, 0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, 0.0f,    //  0.0f, 1.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,    //  0.0f, 0.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, 0.0f,    //  1.0f, 1.0f, 0.0f, 1.0f
	};

	std::vector indices = {
		0, 1, 2,
		2, 3, 0
	};

	CD3D11_BUFFER_DESC vboDesc(sizeof(float) * vertices.size(), D3D11_BIND_VERTEX_BUFFER);
	D3D11_SUBRESOURCE_DATA vboSubResData = {};
	vboSubResData.pSysMem = vertices.data();
	device->CreateBuffer(&vboDesc, &vboSubResData, vertexBuffer.ReleaseAndGetAddressOf());
	
	CD3D11_BUFFER_DESC iboDesc(sizeof(float) * vertices.size(), D3D11_BIND_INDEX_BUFFER);
	D3D11_SUBRESOURCE_DATA iboSubResData = {};
	iboSubResData.pSysMem = indices.data();
	device->CreateBuffer(&iboDesc, &iboSubResData, indexBuffer.ReleaseAndGetAddressOf());

	CD3D11_BUFFER_DESC cameraDesc(sizeof(CameraData), D3D11_BIND_CONSTANT_BUFFER);
	device->CreateBuffer(&cameraDesc, nullptr, cameraConstantBuffer.ReleaseAndGetAddressOf());


	CD3D11_BUFFER_DESC modelDesc(sizeof(ModelData), D3D11_BIND_CONSTANT_BUFFER);
	device->CreateBuffer(&modelDesc, nullptr, modelConstantBuffer.ReleaseAndGetAddressOf());

	projection = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(80.f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 1000.0f);


	front.position = Vector3(0.0f, 0.0f, -.5f);
	front.position.z += 1;
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
	auto const ms = m_mouse->GetState();
	
	// add kb/mouse interact here
	
	if (kb.Escape)
		ExitGame();

	view = Matrix::CreateLookAt(Vector3(sin(timer.GetTotalSeconds()), 0.0f, cos(timer.GetTotalSeconds())), Vector3::Zero, Vector3::Up);

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
	context->IASetInputLayout(inputLayout.Get());

	basicShader->Apply(m_deviceResources.get());

	ID3D11Buffer* vbs[] = { vertexBuffer.Get() };
	UINT strides[] = { sizeof(float) * 3 };
	UINT offsets[] = { 0 };


	// do math here
	context->IASetVertexBuffers(0, 1, vbs, strides, offsets);
	context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);


	CameraData cd;
	cd.view = view.Transpose();
	// md.mView = Matrix::Identity;
	cd.projection = projection.Transpose();
	// md.mProjection = Matrix::Identity;
	ModelData md;
	md.model = front.GetTransformMatrix().Transpose();
	context->UpdateSubresource(cameraConstantBuffer.Get(), 0, nullptr, &cd, 0, 0);
	context->UpdateSubresource(modelConstantBuffer.Get(), 0, nullptr, &md, 0, 0);

	ID3D11Buffer* cbs[] = {modelConstantBuffer.Get(), cameraConstantBuffer.Get()};
	context->VSSetConstantBuffers(0, 2, cbs);

	context->DrawIndexed(6, 0, 0);

	// envoie nos commandes au GPU pour etre afficher � l'�cran
	m_deviceResources->Present();
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
	projection = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(80.f), static_cast<float>(width) / height, 0.1f, 1000.0f);
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

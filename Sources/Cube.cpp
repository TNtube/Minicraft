#include "pch.h"
#include "Cube.h"

Vector4 ToVec4(Vector3 v)
{
	return {v.x, v.y, v.z, 1.0f};
}

void Cube::AddFace(Vector3 position, Vector3 up, Vector3 right)
{
	auto a = m_vertexBuffer.PushVertex({ToVec4(position),     {0.0f, 0.0f}});
	auto b = m_vertexBuffer.PushVertex({ToVec4(position + up),     {0.0f, 1.0f}});
	auto c = m_vertexBuffer.PushVertex({ToVec4(position+ right),     {1.0f, 0.0f}});
	auto d = m_vertexBuffer.PushVertex({ToVec4(position+ up + right),     {1.0f, 1.0f}});

	m_indexBuffer.PushTriangle(a, b, c);
	m_indexBuffer.PushTriangle(c, b, d);

}

Cube::Cube(DeviceResources* deviceResources)
{
	std::vector<VertexLayout_PositionUV> vertices;
	std::vector<uint32_t> indices;

	AddFace({-0.5f, -0.5f, 0.5f}, Vector3::Up, Vector3::Right);
	AddFace({0.5f,  -0.5f, 0.5f}, Vector3::Up, Vector3::Forward);
	AddFace({0.5f,  -0.5f, -0.5f}, Vector3::Up, Vector3::Left);
	AddFace({-0.5f,  -0.5f, -0.5f}, Vector3::Up, Vector3::Backward);
	AddFace({-0.5f, 0.5f, 0.5f}, Vector3::Forward, Vector3::Right);
	AddFace({-0.5f,  -0.5f, -0.5f}, Vector3::Backward, Vector3::Right);
	
	m_vertexBuffer.Create(deviceResources);
	m_indexBuffer.Create(deviceResources);
}


void Cube::Draw(DeviceResources* deviceResources)
{
	auto d3dContext = deviceResources->GetD3DDeviceContext();

	m_vertexBuffer.Bind(deviceResources);
	m_indexBuffer.Bind(deviceResources);

	d3dContext->DrawIndexed(m_indexBuffer.GetCount(), 0, 0);
}

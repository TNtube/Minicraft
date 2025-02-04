#include "pch.h"
#include "Cube.h"

void Cube::AddFace(Vector3 position, Vector3 right, Vector3 up)
{
	m_vertexBuffer.PushVertex({{position.x - right.x + up.x, position.y - right.y + up.y, position.z - right.z + up.z, 1.0f},     {0.0f, 1.0f}}); // top left
	m_vertexBuffer.PushVertex({{position.x + right.x + up.x, position.y + right.y + up.y, position.z + right.z + up.z, 1.0f},     {1.0f, 1.0f}}); // top right
	m_vertexBuffer.PushVertex({{position.x + right.x - up.x, position.y + right.y - up.y, position.z + right.z - up.z, 1.0f},     {1.0f, 0.0f}}); // bottom right
	uint32_t baseIndex = m_vertexBuffer.PushVertex({{position.x - right.x - up.x, position.y - right.y - up.y, position.z - right.z - up.z, 1.0f},     {0.0f, 0.0f}}); // bottom left

	baseIndex -= 3;
	m_indexBuffer.PushTriangle(baseIndex + 0, baseIndex + 1, baseIndex + 2);
	m_indexBuffer.PushTriangle(baseIndex + 2, baseIndex + 3, baseIndex + 0);

}

Cube::Cube(DeviceResources* deviceResources)
{
	std::vector<VertexLayout_PositionUV> vertices;
	std::vector<uint32_t> indices;

	AddFace(Vector3(0, 0, 1) , Vector3::Right, Vector3::Up);
	AddFace(Vector3( 0,  0, -1), Vector3::Right, Vector3::Down);
	AddFace(Vector3( 0,  -1, 0), Vector3::Right, Vector3::Backward);
	AddFace(Vector3( 0, 1, 0), Vector3::Right, Vector3::Forward);
	AddFace(Vector3( -1,  0, 0), Vector3::Backward, Vector3::Up);
	AddFace(Vector3(1,  0, 0), Vector3::Forward, Vector3::Up);
	
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

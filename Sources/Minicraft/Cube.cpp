#include "pch.h"
#include "Cube.h"

Vector4 ToVec4(Vector3 v)
{
	return {v.x, v.y, v.z, 1.0f};
}

void Cube::AddFace(Vector3 position, Vector3 up, Vector3 right, Vector2 textCoord)
{
	float u = textCoord.x / 16.0f;
	float v = textCoord.y / 16.0f;
	float one = 1.0f / 16.0f;
	auto a = m_vertexBuffer.PushVertex({ToVec4(position), {u, v + one}});
	auto b = m_vertexBuffer.PushVertex({ToVec4(position + up), {u, v}});
	auto c = m_vertexBuffer.PushVertex({ToVec4(position+ right), { u + one, v + one}});
	auto d = m_vertexBuffer.PushVertex({ToVec4(position+ up + right), {u + one, v}});

	m_indexBuffer.PushTriangle(a, b, c);
	m_indexBuffer.PushTriangle(c, b, d);
}


Cube::Cube(BlockId blockId, Vector3 position)
	: m_blockId(blockId)
{
	transform.SetPosition(position);
}

void Cube::Create(DeviceResources* deviceResources)
{
	std::vector<VertexLayout_PositionUV> vertices;
	std::vector<uint32_t> indices;

	const BlockData& data = BlockData::Get(m_blockId);

	Vector2 topTexCoord{static_cast<float>(data.texIdTop % 16), static_cast<float>(data.texIdTop / 16)};
	Vector2 sideTexCoord{static_cast<float>(data.texIdSide % 16), static_cast<float>(data.texIdSide / 16)};
	Vector2 bottomTexCoord{static_cast<float>(data.texIdBottom % 16), static_cast<float>(data.texIdBottom / 16)};

	AddFace({-0.5f, -0.5f, 0.5f}, Vector3::Up, Vector3::Right, sideTexCoord);
	AddFace({0.5f,  -0.5f, 0.5f}, Vector3::Up, Vector3::Forward, sideTexCoord);
	AddFace({0.5f,  -0.5f, -0.5f}, Vector3::Up, Vector3::Left, sideTexCoord);
	AddFace({-0.5f,  -0.5f, -0.5f}, Vector3::Up, Vector3::Backward, sideTexCoord);
	AddFace({-0.5f, 0.5f, 0.5f}, Vector3::Forward, Vector3::Right, topTexCoord);
	AddFace({-0.5f,  -0.5f, -0.5f}, Vector3::Backward, Vector3::Right, bottomTexCoord);

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

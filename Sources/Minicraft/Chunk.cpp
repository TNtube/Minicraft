#include "pch.h"
#include "Chunk.h"
#include "World.h"

Vector4 ToVec4(Vector3 v)
{
	return {v.x, v.y, v.z, 1.0f};
}

void Chunk::AddFace(Vector3 position, Vector3 up, Vector3 right, Vector2 textCoord, ShaderPass shaderPass)
{
	float u = textCoord.x / 16.0f;
	float v = textCoord.y / 16.0f;
	float one = 1.0f / 16.0f;
	int pass = static_cast<int>(shaderPass);
	auto a = m_vertexBuffer[pass].PushVertex({ToVec4(position), {u, v + one}});
	auto b = m_vertexBuffer[pass].PushVertex({ToVec4(position + up), {u, v}});
	auto c = m_vertexBuffer[pass].PushVertex({ToVec4(position+ right), { u + one, v + one}});
	auto d = m_vertexBuffer[pass].PushVertex({ToVec4(position+ up + right), {u + one, v}});

	m_indexBuffer[pass].PushTriangle(a, b, c);
	m_indexBuffer[pass].PushTriangle(c, b, d);
}

bool Chunk::ShouldRenderFace(Vector3 position, Vector3 direction, const BlockData& data) const
{
	Vector3 next = {position.x + direction.x, position.y + direction.y, position.z + direction.z};

	auto chunkPosition = World::WorldToChunkPosition(next);
	auto chunk = chunkPosition == m_chunkPosition ? this : m_world->GetChunk(chunkPosition);

	if (chunk == nullptr)
		return true;

	BlockId nextBlock = *chunk->GetBlock(next);
	const BlockData* nextData = &BlockData::Get(nextBlock);

	bool nextTransparent = (data.pass == ShaderPass::Normal || data.id != nextData->id) && nextData->pass != ShaderPass::Normal;
	return nextTransparent || nextBlock == EMPTY;
}


Chunk::Chunk() : m_world(nullptr), m_blocks(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE, EMPTY)
{
}

Chunk::Chunk(World* world, Vector3 position)
	:	m_world(world), m_chunkPosition(position),
		m_blocks(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE, EMPTY)
{
}

BlockId* Chunk::GetBlock(Vector3 worldPosition)
{
	Vector3 localPosition = worldPosition - m_chunkPosition * CHUNK_SIZE;
	return &m_blocks[static_cast<int>(localPosition.x)
					+ static_cast<int>(localPosition.y) * CHUNK_SIZE
					+ static_cast<int>(localPosition.z) * CHUNK_SIZE * CHUNK_SIZE];
}
const BlockId* Chunk::GetBlock(Vector3 worldPosition) const
{
	Vector3 localPosition = worldPosition - m_chunkPosition * CHUNK_SIZE;
	return &m_blocks[static_cast<int>(localPosition.x)
					+ static_cast<int>(localPosition.y) * CHUNK_SIZE
					+ static_cast<int>(localPosition.z) * CHUNK_SIZE * CHUNK_SIZE];
}

void Chunk::Create(DeviceResources* deviceResources)
{
	m_hasBlocks = std::any_of(m_blocks.begin(), m_blocks.end(), [](BlockId id) { return id != EMPTY; });
	if (!m_hasBlocks)
		return;
	
	std::vector<VertexLayout_PositionUV> vertices;
	std::vector<uint32_t> indices;

	for (int i = 0; i < m_blocks.size(); i++)
	{
		auto blockId = m_blocks[i];

		if (blockId == EMPTY) continue;

		Vector3 blockPosition = {
			static_cast<float>(i % CHUNK_SIZE),
			static_cast<float>((i / CHUNK_SIZE) % CHUNK_SIZE),
			static_cast<float>(i / (CHUNK_SIZE * CHUNK_SIZE))
		};

		blockPosition += m_chunkPosition * CHUNK_SIZE;
		
		const BlockData& data = BlockData::Get(blockId);

		Vector2 topTexCoord{static_cast<float>(data.texIdTop % 16), static_cast<float>(data.texIdTop / 16)};
		Vector2 sideTexCoord{static_cast<float>(data.texIdSide % 16), static_cast<float>(data.texIdSide / 16)};
		Vector2 bottomTexCoord{static_cast<float>(data.texIdBottom % 16), static_cast<float>(data.texIdBottom / 16)};

		Vector3 back =		Vector3{-0.5f, -0.5f,  0.5f};
		Vector3 right =		Vector3{ 0.5f, -0.5f,  0.5f};
		Vector3 front =		Vector3{ 0.5f, -0.5f, -0.5f};
		Vector3 left =		Vector3{-0.5f, -0.5f, -0.5f};
		Vector3 top =		Vector3{-0.5f,  0.5f,  0.5f};
		Vector3 bottom =	Vector3{-0.5f, -0.5f, -0.5f};

		if (ShouldRenderFace(blockPosition, Vector3::Backward, data))
			AddFace(blockPosition + back, Vector3::Up, Vector3::Right, sideTexCoord, data.pass);
		if (ShouldRenderFace(blockPosition, Vector3::Right, data))
			AddFace(blockPosition + right, Vector3::Up, Vector3::Forward, sideTexCoord, data.pass);
		if (ShouldRenderFace(blockPosition, Vector3::Forward, data))
			AddFace(blockPosition + front, Vector3::Up, Vector3::Left, sideTexCoord, data.pass);
		if (ShouldRenderFace(blockPosition, Vector3::Left, data))
			AddFace(blockPosition + left, Vector3::Up, Vector3::Backward, sideTexCoord, data.pass);
		if (ShouldRenderFace(blockPosition, Vector3::Up, data))
			AddFace(blockPosition + top, Vector3::Forward, Vector3::Right, topTexCoord, data.pass);
		if (ShouldRenderFace(blockPosition, Vector3::Down, data))
			AddFace(blockPosition + bottom, Vector3::Backward, Vector3::Right, bottomTexCoord, data.pass);
		
		// AddFace({-0.5f, -0.5f, 0.5f}, Vector3::Up, Vector3::Right, sideTexCoord);					// front
		// AddFace({0.5f,  -0.5f, 0.5f}, Vector3::Up, Vector3::Forward, sideTexCoord);			// back
		// AddFace({0.5f,  -0.5f, -0.5f}, Vector3::Up, Vector3::Left, sideTexCoord);			// left
		// AddFace({-0.5f,  -0.5f, -0.5f}, Vector3::Up, Vector3::Backward, sideTexCoord);		// right
		// AddFace({-0.5f, 0.5f, 0.5f}, Vector3::Forward, Vector3::Right, topTexCoord);			// top
		// AddFace({-0.5f,  -0.5f, -0.5f}, Vector3::Backward, Vector3::Right, bottomTexCoord);	// down
	}

	for (int i = 0; i < static_cast<int>(ShaderPass::Size); ++i)
	{
		m_vertexBuffer[i].Create(deviceResources);
		m_indexBuffer[i].Create(deviceResources);
	}
}

void Chunk::Draw(DeviceResources* deviceResources, ShaderPass shaderPass)
{
	if (!m_hasBlocks)
		return;

	auto d3dContext = deviceResources->GetD3DDeviceContext();

	int pass = static_cast<int>(shaderPass);

	m_vertexBuffer[pass].Bind(deviceResources);
	m_indexBuffer[pass].Bind(deviceResources);

	d3dContext->DrawIndexed(m_indexBuffer[pass].GetCount(), 0, 0);
}

#include "pch.h"
#include "World.h"

#include "Block.h"
#include "Chunk.h"

enum BlockId : uint8_t;
using namespace DirectX::SimpleMath;

World::World(Vector3 dimension) : m_dimension(dimension)
{
}

void World::Generate(DeviceResources* deviceResources)
{
	float sizeX = m_dimension.x;
	float sizeY = m_dimension.y;
	float sizeZ = m_dimension.z;

	std::vector<BlockId> blocks = std::vector<BlockId>(m_dimension.x * m_dimension.y * m_dimension.z);

	
	for (float x = 0; x < sizeX; x+=1)
		for (float y = 0; y < sizeY; y+=1)
			for (float z = 0; z < sizeZ; z+=1)
			{
				BlockId type = EMPTY;

				if (y < 12) type = GRASS;
				if (y < 11) type = DIRT;
				if (y < 7) type = STONE;
				blocks[static_cast<int>(x + y * sizeX + z * sizeX * sizeY)] = type;
			}

	auto& chunk = m_chunks.emplace_back(Vector3(0, 0, 0), m_dimension);
	chunk.SetBlocks(blocks);
	chunk.Create(deviceResources);

	m_modelConstantBuffer.Create(deviceResources);
}

void World::Draw(DeviceResources* deviceResources)
{
	for (auto& chunk : m_chunks)
	{
		ModelData md;
		md.model = chunk.transform.GetTransformMatrix().Transpose();
		m_modelConstantBuffer.UpdateSubResource(deviceResources, md);
		m_modelConstantBuffer.Bind(deviceResources, 0);
		chunk.Draw(deviceResources);
	}
}

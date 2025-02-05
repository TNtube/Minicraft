#include "pch.h"
#include "World.h"

World::World(Vector3 dimension) : m_dimension(dimension)
{
}

void World::Generate(DeviceResources* deviceResources)
{
	float sizeX = m_dimension.x / 2;
	float sizeY = m_dimension.y / 2;
	float sizeZ = m_dimension.z / 2;
	m_blocks.reserve(m_dimension.x * m_dimension.y * m_dimension.z);

	for (float x = -sizeX; x < sizeX; x+=1)
		for (float y = -sizeY; y < sizeY; y+=1)
			for (float z = -sizeZ; z < sizeZ; z+=1)
			{
				BlockId type = EMPTY;

				if (y < 5) type = GRASS;
				if (y < 4) type = DIRT;
				if (y < 0) type = STONE;
				m_blocks.push_back(type);
			}

	for (int i = 0; i < m_blocks.size(); ++i)
	{
		if (m_blocks[i] == EMPTY) continue;
		int x = static_cast<int>(m_dimension.x);
		int y = static_cast<int>(m_dimension.y);
		Vector3 position = {static_cast<float>(i % x), static_cast<float>((i / x) % y), static_cast<float>(i / (x * y))};

		position.x -= m_dimension.x / 2.0f;
		// position.y -= m_dimension.y / 2.0f;
		position.z -= m_dimension.z / 2.0f;
		
		auto& cube = m_cubes.emplace_back(m_blocks[i], position);
		cube.Create(deviceResources);
	}

	m_modelConstantBuffer.Create(deviceResources);
}

void World::Draw(DeviceResources* deviceResources)
{
	for (auto& cube : m_cubes)
	{
		ModelData md;
		md.model = cube.transform.GetTransformMatrix().Transpose();
		m_modelConstantBuffer.UpdateSubResource(deviceResources, md);
		m_modelConstantBuffer.Bind(deviceResources, 0);
		cube.Draw(deviceResources);
	}
}

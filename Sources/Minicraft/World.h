#pragma once
#include "Cube.h"

class World
{
public:
	World(Vector3 dimension);
	void Generate(DeviceResources* deviceResources);

	void Draw(DeviceResources* deviceResources);

private:

	std::vector<BlockId> m_blocks;
	Vector3 m_dimension;
	std::vector<Cube> m_cubes;

	ConstantBuffer<ModelData> m_modelConstantBuffer;
};

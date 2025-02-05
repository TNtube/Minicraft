#pragma once
#include "Chunk.h"

class World
{
public:
	World(Vector3 dimension);

	void Generate(DeviceResources* deviceResources);
	void Draw(DeviceResources* deviceResources);

private:
	Vector3 m_dimension;
	std::vector<Chunk> m_chunks;

	ConstantBuffer<ModelData> m_modelConstantBuffer;
};

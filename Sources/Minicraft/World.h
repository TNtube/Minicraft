#pragma once
#include "Chunk.h"
#include <array>

#define CHUNK_Y_COUNT 8

class World
{
public:
	World(Vector3 dimension);

	void Generate(DeviceResources* deviceResources);
	void Draw(DeviceResources* deviceResources);

	Chunk* GetChunk(Vector3 chunkPosition);

	// whole chunk position
	static Vector3 WorldToChunkPosition(Vector3 worldPosition);
	// local block position in the chunk
	static Vector3 WorldToLocalPosition(Vector3 worldPosition);

private:
	static size_t HashChunkPosition(Vector3 position);

	std::unordered_map<size_t, std::array<Chunk, CHUNK_Y_COUNT>> m_chunks;

	ConstantBuffer<ModelData> m_modelConstantBuffer;
};

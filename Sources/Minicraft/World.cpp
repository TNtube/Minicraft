#include "pch.h"
#include "World.h"

#include "Block.h"
#include "Chunk.h"

enum BlockId : uint8_t;
using namespace DirectX::SimpleMath;


size_t World::HashChunkPosition(Vector3 v)
{
	int x = static_cast<int>(v.x);
	int z = static_cast<int>(v.z);

	return x << 16 | (z & 0xFFFF);
}


World::World(Vector3 dimension)
{
}

void World::Generate(DeviceResources* deviceResources)
{
	// for (int x = 0; x < 8; ++x)
	// {
	// 	for (int z = 0; z < 8; ++z)
	// 	{
	// 		std::array<Chunk, CHUNK_SIZE / 2> chunkArray;
	// 		for (int y = 0; y < CHUNK_SIZE / 2; ++y)
	// 		{
	// 			std::vector<BlockId> blocks = std::vector<BlockId>(CHUNK_SIZE *CHUNK_SIZE * CHUNK_SIZE);
	//
	// 			float sizeX = CHUNK_SIZE;
	// 			float sizeY = CHUNK_SIZE;
	// 			float sizeZ = CHUNK_SIZE;
	//
	// 			for (float bx = 0; bx < sizeX; bx+=1)
	// 				for (float by = 0; by < sizeY; by+=1)
	// 					for (float bz = 0; bz < sizeZ; bz+=1)
	// 					{
	// 						BlockId type = EMPTY;
	// 						float wby = by + y * CHUNK_SIZE;
	//
	// 						if (wby < 60) type = GRASS;
	// 						if (wby < 59) type = DIRT;
	// 						if (wby < 50) type = STONE;
	// 						if (wby == 0) type = BEDROCK;
	// 						blocks[static_cast<int>(bx + by * sizeX + bz * sizeX * sizeY)] = type;
	// 					}
	// 			chunkArray[y] = Chunk(this, Vector3(x, y, z), {CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE});
	// 			// chunkArray[y].SetBlocks(std::move(blocks));
	// 		}
	// 		m_chunks.insert_or_assign(HashChunkPosition(Vector3(x, 0, z)), std::move(chunkArray));
	// 	}
	// }

	constexpr int chunkCount = 8;

	for (int x = -chunkCount * CHUNK_SIZE; x < chunkCount * CHUNK_SIZE; ++x)
	{
		for (int z = -chunkCount * CHUNK_SIZE; z < chunkCount * CHUNK_SIZE; ++z)
		{
			for (int y = 0; y < CHUNK_Y_COUNT * CHUNK_SIZE; ++y)
			{
				Vector3 chunkPosition = WorldToChunkPosition(Vector3(x, y, z));
				auto chunk = GetChunk(chunkPosition);
				if (!chunk)
				{
					std::array<Chunk, CHUNK_Y_COUNT> chunkArray;
					for (int sy = 0; sy < CHUNK_Y_COUNT; ++sy)
					{
						chunkArray[sy] = Chunk(this, Vector3(chunkPosition.x, sy, chunkPosition.z));
					}
					m_chunks.insert_or_assign(HashChunkPosition(chunkPosition), std::move(chunkArray));
					chunk = GetChunk(chunkPosition);
				}

				BlockId type = EMPTY;
				if (y < 60) type = GRASS;
				if (y < 59) type = DIRT;
				if (y < 50) type = STONE;
				if (y < 20) type = BEDROCK;
				*chunk->GetBlock(Vector3(x, y, z)) = type;
			}
		}
	}

	for (auto& chunkArr : m_chunks)
	{
		std::for_each(
			chunkArr.second.begin(), chunkArr.second.end(),
			[deviceResources](Chunk& c)
			{
				c.Create(deviceResources);
			});
	}
	m_modelConstantBuffer.Create(deviceResources);
}

void World::Draw(DeviceResources* deviceResources)
{
	for (auto& chunkArray : m_chunks)
	{
		for (auto& chunk : chunkArray.second)
		{
			ModelData md;
			md.model = chunk.transform.GetTransformMatrix().Transpose();
			m_modelConstantBuffer.UpdateSubResource(deviceResources, md);
			m_modelConstantBuffer.Bind(deviceResources, 0);
			chunk.Draw(deviceResources);
		}
	}
}

Chunk* World::GetChunk(Vector3 chunkPosition)
{
	if (m_chunks.find(HashChunkPosition(chunkPosition)) == m_chunks.end())
		return nullptr;

	auto& chunkArray = m_chunks.at(HashChunkPosition(chunkPosition));

	if (chunkPosition.y < 0 || chunkPosition.y >= chunkArray.size())
		return nullptr;

	return &chunkArray.at(static_cast<int>(chunkPosition.y));
}

Vector3 World::WorldToChunkPosition(Vector3 worldPosition)
{
	return {
		std::floor(worldPosition.x / CHUNK_SIZE),
		std::floor(worldPosition.y / CHUNK_SIZE),
		std::floor(worldPosition.z / CHUNK_SIZE)
	};
}

float NegativeModulo(int a, int b)
{
	return static_cast<float>((b + (a % b)) % b);
}

Vector3 World::WorldToLocalPosition(Vector3 worldPosition)
{
	return {
		NegativeModulo(worldPosition.x, CHUNK_SIZE),
		NegativeModulo(worldPosition.y, CHUNK_SIZE),
		NegativeModulo(worldPosition.z, CHUNK_SIZE)
	};
}

#pragma once
#include "Engine/Transform.h"
#include "Engine/VertexLayout.h"
#include "Engine/Buffers.h"
#include "Minicraft/Block.h"

struct ModelData
{
	Matrix model;
};

#define CHUNK_SIZE 16

class World;


class Chunk
{
public:
	Transform transform;

	Chunk();

	Chunk(World* world, Vector3 position);

	BlockId* GetBlock(Vector3 worldPosition);

	
	void Create(DeviceResources* deviceResources);
	void Draw(DeviceResources* deviceResources);

private:
	friend class World;
	void AddFace(Vector3 position, Vector3 up, Vector3 right, Vector2 textCoord);

	bool ShouldRenderFace(Vector3 position, Vector3 direction) const;

	World* m_world;

	bool m_hasBlocks = false;
	
	VertexBuffer<VertexLayout_PositionUV> m_vertexBuffer;
	IndexBuffer m_indexBuffer;

	Vector3 m_chunkPosition;

	std::vector<BlockId> m_blocks;
};

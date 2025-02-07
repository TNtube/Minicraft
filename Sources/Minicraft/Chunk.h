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
	const BlockId* GetBlock(Vector3 worldPosition) const;

	
	void Create(DeviceResources* deviceResources);
	void Draw(DeviceResources* deviceResources, ShaderPass pass);

private:
	friend class World;
	void AddFace(Vector3 position, Vector3 up, Vector3 right, Vector4 normal, Vector2 textCoord, ShaderPass shaderPass);

	bool ShouldRenderFace(Vector3 position, Vector3 direction, const BlockData& data) const;

	World* m_world;

	bool m_hasBlocks = false;
	
	VertexBuffer<VertexLayout_PositionNormalUV> m_vertexBuffer[ShaderPass::Size];
	IndexBuffer m_indexBuffer[ShaderPass::Size];

	Vector3 m_chunkPosition;

	std::vector<BlockId> m_blocks;
};

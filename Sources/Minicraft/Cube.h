#pragma once
#include "Engine/Transform.h"
#include "Engine/VertexLayout.h"
#include "Engine/Buffers.h"
#include "Minicraft/Block.h"

struct ModelData
{
	Matrix model;
};


class Cube
{
public:
	Transform transform;

	Cube(BlockId blockId, Vector3 position);
	void Create(DeviceResources* deviceResources);
	void Draw(DeviceResources* deviceResources);

private:
	void AddFace(Vector3 position, Vector3 up, Vector3 right, Vector2 textCoord);
	VertexBuffer<VertexLayout_PositionUV> m_vertexBuffer;
	IndexBuffer m_indexBuffer;

	BlockId m_blockId;
};

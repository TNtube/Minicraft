#pragma once
#include "Engine/Transform.h"
#include "Engine/VertexLayout.h"
#include "Engine/Buffers.h"

class Cube
{
public:
	Transform transform;

	Cube(DeviceResources* deviceResources);
	void Draw(DeviceResources* deviceResources);

private:
	void AddFace(Vector3 position, Vector3 right, Vector3 up);
	VertexBuffer<VertexLayout_PositionUV> m_vertexBuffer;
	IndexBuffer m_indexBuffer;
};

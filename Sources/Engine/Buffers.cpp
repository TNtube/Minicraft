#include "pch.h"
#include "Buffers.h"

void IndexBuffer::Create(DeviceResources* deviceRes)
{
	auto d3dDevice = deviceRes->GetD3DDevice();

	CD3D11_BUFFER_DESC bufferDesc(sizeof(uint32_t) * m_indices.size(), D3D11_BIND_INDEX_BUFFER);
	D3D11_SUBRESOURCE_DATA indexData = { m_indices.data(), 0, 0 };

	d3dDevice->CreateBuffer(&bufferDesc, &indexData, m_buffer.ReleaseAndGetAddressOf());
}

void IndexBuffer::Bind(DeviceResources* deviceRes) const
{
	auto d3dContext = deviceRes->GetD3DDeviceContext();
	d3dContext->IASetIndexBuffer(m_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}

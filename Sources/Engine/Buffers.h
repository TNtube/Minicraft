#pragma once
using Microsoft::WRL::ComPtr;



template<typename TVertex>
class VertexBuffer
{
public:
	VertexBuffer() = default;

	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer& operator=(const VertexBuffer&) = delete;

	VertexBuffer(VertexBuffer&&) = default;
	VertexBuffer& operator=(VertexBuffer&&) = default;

	~VertexBuffer() = default;


	uint32_t PushVertex(TVertex v) {
		data.push_back(v);
		return data.size() - 1;
	}


	void Create(DeviceResources* deviceRes)
	{
		auto d3dDevice = deviceRes->GetD3DDevice();

		CD3D11_BUFFER_DESC bufferDesc(sizeof(TVertex) * data.size(), D3D11_BIND_VERTEX_BUFFER);

		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = data.data();

		d3dDevice->CreateBuffer(&bufferDesc, &initData, m_buffer.ReleaseAndGetAddressOf());
	}

	void Bind(DeviceResources* deviceRes)
	{
		auto d3dContext = deviceRes->GetD3DDeviceContext();

		UINT stride = sizeof(TVertex);
		UINT offset = 0;
		d3dContext->IASetVertexBuffers(0, 1, m_buffer.GetAddressOf(), &stride, &offset);
	}

private:
	ComPtr<ID3D11Buffer> m_buffer;
	std::vector<TVertex> data;
};

class IndexBuffer
{
public:
	IndexBuffer() = default;

	IndexBuffer(const IndexBuffer&) = delete;
	IndexBuffer& operator=(const IndexBuffer&) = delete;

	IndexBuffer(IndexBuffer&&) = default;
	IndexBuffer& operator=(IndexBuffer&&) = default;

	~IndexBuffer() = default;

	uint32_t GetCount() const { return m_indices.size(); }

	void PushTriangle(uint32_t a, uint32_t b, uint32_t c) {
		m_indices.push_back(a);
		m_indices.push_back(b);
		m_indices.push_back(c);
	}

	void Create(DeviceResources* deviceRes);
	void Bind(DeviceResources* deviceRes) const;

private:
	ComPtr<ID3D11Buffer> m_buffer;
	std::vector<uint32_t> m_indices;
};

template<typename T>
class ConstantBuffer
{
public:
	ConstantBuffer() = default;

	ConstantBuffer(const ConstantBuffer&) = delete;
	ConstantBuffer& operator=(const ConstantBuffer&) = delete;

	ConstantBuffer(ConstantBuffer&&) = default;
	ConstantBuffer& operator=(ConstantBuffer&&) = default;

	~ConstantBuffer() = default;

	void Create(DeviceResources* deviceRes)
	{
		auto d3dDevice = deviceRes->GetD3DDevice();

		CD3D11_BUFFER_DESC bufferDesc(sizeof(T), D3D11_BIND_CONSTANT_BUFFER);
		d3dDevice->CreateBuffer(&bufferDesc, nullptr, m_buffer.ReleaseAndGetAddressOf());
	}
	
	void UpdateSubResource(DeviceResources* deviceRes, const T& data)
	{
		auto d3dContext = deviceRes->GetD3DDeviceContext();
		d3dContext->UpdateSubresource(m_buffer.Get(), 0, nullptr, &data, 0, 0);
	}
	
	void Bind(DeviceResources* deviceRes, uint32_t slot) const
	{
		auto d3dContext = deviceRes->GetD3DDeviceContext();
		d3dContext->VSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());
	}

private:
	ComPtr<ID3D11Buffer> m_buffer;
};


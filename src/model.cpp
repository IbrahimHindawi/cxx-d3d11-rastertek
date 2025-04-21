#include "model.h"

bool modelInitialize(Model *model, ID3D11Device *device)
{
    bool result = modelInitializeBuffers(model, device);
    if (!result)
    {
        return false;
    }
    return true;
}

void modelDeinitialize(Model *model)
{
    modelDeinitializeBuffers(model);
    return;
}

bool modelInitializeBuffers(Model *model, ID3D11Device *device)
{

	VertexType* vertices = 0;
	u64* indices = 0;
	D3D11_BUFFER_DESC vertexBufferDesc = {};
    D3D11_BUFFER_DESC indexBufferDesc = {};
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
    model->indexCount = 3;
    model->vertexCount = 3;
    vertices = (VertexType *)malloc(sizeof(VertexType) * model->vertexCount);
    if (!vertices)
    {
        return false;
    }
    indices = (u64 *)malloc(sizeof(u64) * model->indexCount);
    if (!indices)
    {
        return false;
    }
    vertices[0].position = DirectX::XMFLOAT3(-1.f, -1.f, 0.f);
    vertices[0].color = DirectX::XMFLOAT4(0.f, 1.f, 0.f, 1.f);

    vertices[1].position = DirectX::XMFLOAT3(0.f, 1.f, 0.f);
    vertices[1].color = DirectX::XMFLOAT4(0.f, 1.f, 0.f, 1.f);

    vertices[2].position = DirectX::XMFLOAT3(1.f, -1.f, 0.f);
    vertices[2].color = DirectX::XMFLOAT4(0.f, 1.f, 0.f, 1.f);

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * model->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &model->vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * model->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &model->indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

    free(vertices);
    vertices = 0;
    free(indices);
    indices = 0;

    return true;
}

void modelDeinitializeBuffers(Model *model)
{
	if(model->indexBuffer)
	{
		model->indexBuffer->Release();
		model->indexBuffer = 0;
	}
	if(model->vertexBuffer)
	{
		model->vertexBuffer->Release();
		model->vertexBuffer = 0;
	}
    return;
}

void modelRender(Model *model, ID3D11DeviceContext *deviceContext)
{
    modelRenderBuffers(model, deviceContext);
}

void modelRenderBuffers(Model *model, ID3D11DeviceContext *deviceContext)
{
    u32 stride;
    u32 offset;
    stride = sizeof(VertexType);
    offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, &model->vertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(model->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    return;
}

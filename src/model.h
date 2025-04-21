#pragma once

#include <d3d11.h>
#include <directxmath.h>

#include "core.h"

struct VertexType
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT4 color;
};

struct Model
{
    ID3D11Buffer *vertexBuffer;
    ID3D11Buffer *indexBuffer;
    i32 vertexCount;
    i32 indexCount;
};

bool modelInitialize(Model *model, ID3D11Device *device);
void modelDeinitialize(Model *model);
bool modelInitializeBuffers(Model *model, ID3D11Device *device);
void modelDeinitializeBuffers(Model *model);
void modelRender(Model *model, ID3D11DeviceContext *deviceContext);
void modelRenderBuffers(Model *model, ID3D11DeviceContext *deviceContext);

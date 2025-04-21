#pragma once

#define UNICODE

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>

#include "core.h"

struct MatrixBufferType
{
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX projection;
};

struct Shader {
    MatrixBufferType matrixBufferType;
    ID3D11VertexShader *vertexShader;
    ID3D11PixelShader *pixelShader;
    ID3D11InputLayout *layout;
    ID3D11Buffer *matrixBuffer;
};

// load shader files
bool shaderInitialize(Shader *, ID3D11Device*, HWND);
void shaderDeinitialize(Shader *);
// set parameters and render
bool shaderRender(Shader *, ID3D11DeviceContext *, i32, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX);

// load shader onto gpu
bool shaderInitializeShader(Shader *, ID3D11Device *, HWND, WCHAR *, WCHAR *);
void shaderDeinitializeShader(Shader *);
void shaderOutputShaderErrorMessage(Shader *, ID3D10Blob *, HWND, WCHAR *);

bool shaderSetShaderParameters(Shader *, ID3D11DeviceContext *, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX);
// actual draw command
void shaderRenderShader(Shader *, ID3D11DeviceContext *, i32);

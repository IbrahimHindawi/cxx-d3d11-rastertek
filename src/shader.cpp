#include "shader.h"
#include <fstream>

bool shaderInitialize(Shader *shader, ID3D11Device *device, HWND hwnd)
{
    wchar_t vsFilename[128];
    wchar_t psFilename[128];
    i32 error;

    error = wcscpy_s(vsFilename, 128, L"resources/color.vs");
    if (error != 0)
    {
        return false;
    }
    error = wcscpy_s(psFilename, 128, L"resources/color.ps");
    if (error != 0)
    {
        return false;
    }
    shaderInitializeShader(shader, device, hwnd, vsFilename, psFilename);
    return true;
}

void shaderDeinitialize(Shader *shader)
{
    shaderDeinitializeShader(shader);
    return;
}

bool shaderRender(Shader *shader, ID3D11DeviceContext *deviceContext, i32 indexCount, DirectX::XMMATRIX world, DirectX::XMMATRIX view, DirectX::XMMATRIX projection) {
    bool result = false;
    result = shaderSetShaderParameters(shader, deviceContext, world, view, projection);
    if (!result)
    {
        return false;
    }
    shaderRenderShader(shader, deviceContext, indexCount);
    return true;
}

bool shaderInitializeShader(Shader *shader, ID3D11Device *device, HWND hwnd, WCHAR *vsFilename, WCHAR *psFilename)
{
    HRESULT result;
    ID3D10Blob *errorMessage = 0;
    ID3D10Blob *vertexShaderBuffer = 0;
    ID3D10Blob *pixelShaderBuffer = 0; 
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    u32 numElements;
    D3D11_BUFFER_DESC matrixBufferDesc;

	// Compile the vertex shader code.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        if (errorMessage)
        {
            shaderOutputShaderErrorMessage(shader, errorMessage, hwnd, vsFilename);
        }
        else
        {
            MessageBox(hwnd, vsFilename, TEXT("Missing Shader File"), MB_OK);
        }
        return false;
    }
    result = D3DCompileFromFile(psFilename, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        if (errorMessage)
        {
            shaderOutputShaderErrorMessage(shader, errorMessage, hwnd, psFilename);
        }
        else
        {
            MessageBox(hwnd, vsFilename, TEXT("Missing Shader File"), MB_OK);
        }
        return false;
    }
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &shader->vertexShader);
    if (FAILED(result))
    {
        return false;
    }
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &shader->pixelShader);
    if (FAILED(result))
    {
        return false;
    }
	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;
    // get elem count
    numElements = sizeofarray(polygonLayout);
    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &shader->layout);
    if (FAILED(result))
    {
        return false;
    }
    vertexShaderBuffer->Release();
    vertexShaderBuffer = 0;
    pixelShaderBuffer->Release();
    pixelShaderBuffer = 0;
	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &shader->matrixBuffer);
	if(FAILED(result))
	{
		return false;
	}
	return true;
}

void shaderDeinitializeShader(Shader *shader)
{
	if(shader->matrixBuffer)
	{
		shader->matrixBuffer->Release();
		shader->matrixBuffer = 0;
	}
	if(shader->layout)
	{
		shader->layout->Release();
		shader->layout = 0;
	}
	if(shader->pixelShader)
	{
		shader->pixelShader->Release();
		shader->pixelShader = 0;
	}
	if(shader->vertexShader)
	{
		shader->vertexShader->Release();
		shader->vertexShader = 0;
	}
	return;
}

void shaderOutputShaderErrorMessage(Shader *shader, ID3D10Blob *errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long long bufferSize, i;
    std::ofstream fout;
    // FILE *fout;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");
    // fout = fopen("shader-error.txt", "w");

	// Write out the error message.
	for(i=0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
        // fwrite(const void *Buffer, size_t ElementSize, size_t ElementCount, FILE *Stream)
	}

	// Close the file.
	fout.close();
    // fclose(fout);

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);
	return;
}

bool shaderSetShaderParameters(Shader *shader, ID3D11DeviceContext *deviceContext, DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX viewMatrix, DirectX::XMMATRIX projectionMatrix)
{
	HRESULT result = false;
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	MatrixBufferType* dataPtr = 0;
	u32 bufferNumber = 0;
	// Transpose the matrices to prepare them for the shader.
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);
	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(shader->matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
	deviceContext->Unmap(shader->matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &shader->matrixBuffer);

	return true;
    // return result;
}

void shaderRenderShader(Shader *shader, ID3D11DeviceContext *deviceContext, i32 indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(shader->layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(shader->vertexShader, NULL, 0);
	deviceContext->PSSetShader(shader->pixelShader, NULL, 0);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

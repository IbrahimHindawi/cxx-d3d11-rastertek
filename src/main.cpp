#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXCollision.h>
#include <DirectXColors.h>
#include <d3d11.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <core.h>
#include <saha.h>
#include <Array.h>
#include <stdio.h>
#include <stdlib.h>

struct Core
{
    LPCSTR name;
    HINSTANCE hinstance;
    HWND hwnd;
    bool done;
};
global Core core;

struct Input
{
    bool keys[256];
};
global Input input;

void inputInitialize()
{
    for (i32 i = 0; i < 256; i++)
    {
        input.keys[i] = false;
    }
    return;
}

void inputKeyDown(u32 inputkey)
{
    input.keys[inputkey] = true;
    return;
}

void inputKeyUp(u32 inputkey)
{
    input.keys[inputkey] = false;
    return;
}

bool inputIsKeyDown(u32 inputkey)
{
    return input.keys[inputkey];
}

struct Gfx
{
    bool FULL_SCREEN;
    bool VSYNC_ENABLED;
    f32 SCREEN_DEPTH;
    f32 SCREEN_NEAR;
    i32 videoCardMemory;
    char videoCardDesc[128];

    IDXGISwapChain *swapChain;
    ID3D11Device *device;
    ID3D11DeviceContext *deviceContext;
    ID3D11RenderTargetView *renderTargetView;
    ID3D11Texture2D *depthStencilBuffer;
    ID3D11DepthStencilState *depthStencilState;
    ID3D11DepthStencilView *depthStencilView;
    ID3D11RasterizerState *rasterState;

    DirectX::XMMATRIX projection;
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX ortho;
    D3D11_VIEWPORT viewport;
};
global Gfx gfx;

void gfxInitalize(i32 screenWidth, i32 screenHeight, HWND hwnd)
{
    bool result = false;

}

bool D3D11Initialize(i32 screenWidth, i32 screenHeight, bool vsync, HWND hwnd, bool fullscreen, f32 screen_depth, f32 screen_near)
{
    HRESULT result;
	IDXGIFactory *factory;
	IDXGIAdapter *adapter;
	IDXGIOutput *adapterOutput;
	u32 numModes, i, numerator, denominator;
	u64 stringLength;
	DXGI_MODE_DESC *displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	i32 error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D *backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	f32 fieldOfView, screenAspect;

    result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**) &factory);
    if (FAILED(result)) { return false; }

    result = factory->EnumAdapters(0, &adapter);
    if (FAILED(result)) { return false; }

    result = adapter->EnumOutputs(0, &adapterOutput);
    if (FAILED(result)) { return false; }

    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
    if (FAILED(result)) { return false; }

    // displayModeList = new DXGI_MODE_DESC[numModes];
    displayModeList = (DXGI_MODE_DESC *)malloc(sizeof(DXGI_MODE_DESC) * numModes);
    if (FAILED(result)) { return false; }

    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
    if (FAILED(result)) { return false; }

    for (i32 i = 0; i < numModes; i++)
    {
        if (displayModeList[i].Width == (u32)screenWidth)
        {
            if (displayModeList[i].Height == (u32)screenHeight)
            {
                numerator = displayModeList[i].RefreshRate.Numerator;
                denominator = displayModeList[i].RefreshRate.Denominator;
            }
        
        }
    }

    result = adapter->GetDesc(&adapterDesc);
    if (FAILED(result)) { return false; }

    gfx.videoCardMemory = (i32)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    error = wcstombs_s(&stringLength, gfx.videoCardDesc, 128, adapterDesc.Description, 128);
    if (error != 0) { return false; }

    delete[] displayModeList;
    adapterOutput->Release();
    adapter->Release();
    factory->Release();

    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    if (gfx.VSYNC_ENABLED)
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    }
    else
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
    }

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    if (fullscreen) 
    {
        swapChainDesc.Windowed = false;
    }
    else 
    {
        swapChainDesc.Windowed = true;
    }
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;

    featureLevel = D3D_FEATURE_LEVEL_11_0;
    result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, &gfx.swapChain, &gfx.device, NULL, &gfx.deviceContext);
    if (FAILED(result)) { return false; }

    result = gfx.swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&backBufferPtr);
    if (FAILED(result)) { return false; }

    result = gfx.device->CreateRenderTargetView(backBufferPtr, NULL, &gfx.renderTargetView);
    if (FAILED(result)) { return false; }

    backBufferPtr->Release();
    backBufferPtr = 0;

    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

    result = gfx.device->CreateTexture2D(&depthBufferDesc, NULL, &gfx.depthStencilBuffer);
    if (FAILED(result)) { return false; }

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    result = gfx.device->CreateDepthStencilState(&depthStencilDesc, &gfx.depthStencilState);
    if (FAILED(result)) { return false; }

    gfx.deviceContext->OMSetDepthStencilState(gfx.depthStencilState, 1);

    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

    result = gfx.device->CreateDepthStencilView(gfx.depthStencilBuffer, &depthStencilViewDesc, &gfx.depthStencilView);
    if (FAILED(result)) { return false; }

    gfx.deviceContext->OMSetRenderTargets(1, &gfx.renderTargetView, gfx.depthStencilView);

	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

    result = gfx.device->CreateRasterizerState(&rasterDesc, &gfx.rasterState);
    if (FAILED(result)) { return false; }

    gfx.deviceContext->RSSetState(gfx.rasterState);

	// Setup the viewport for rendering.
	gfx.viewport.Width = (float)screenWidth;
	gfx.viewport.Height = (float)screenHeight;
	gfx.viewport.MinDepth = 0.0f;
	gfx.viewport.MaxDepth = 1.0f;
	gfx.viewport.TopLeftX = 0.0f;
	gfx.viewport.TopLeftY = 0.0f;

	// Create the viewport.
	gfx.deviceContext->RSSetViewports(1, &gfx.viewport);

    fieldOfView = 3.1415926545f / 4.f;
    screenAspect = (f32)screenWidth / (f32)screenHeight;

    gfx.projection = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screen_near, screen_depth);

    gfx.world = DirectX::XMMatrixIdentity();

    gfx.ortho = DirectX::XMMatrixOrthographicLH((f32)screenWidth, (f32)screenHeight, screen_near, screen_depth);

    return true;
}

void D3D11Deinitialize()
{
	if(gfx.swapChain)
	{
		gfx.swapChain->SetFullscreenState(false, NULL);
	}

	if(gfx.rasterState)
	{
		gfx.rasterState->Release();
		gfx.rasterState = 0;
	}

	if(gfx.depthStencilView)
	{
		gfx.depthStencilView->Release();
		gfx.depthStencilView = 0;
	}

	if(gfx.depthStencilState)
	{
		gfx.depthStencilState->Release();
		gfx.depthStencilState = 0;
	}

	if(gfx.depthStencilBuffer)
	{
		gfx.depthStencilBuffer->Release();
		gfx.depthStencilBuffer = 0;
	}

	if(gfx.renderTargetView)
	{
		gfx.renderTargetView->Release();
		gfx.renderTargetView = 0;
	}

	if(gfx.deviceContext)
	{
		gfx.deviceContext->Release();
		gfx.deviceContext = 0;
	}

	if(gfx.device)
	{
		gfx.device->Release();
		gfx.device = 0;
	}

	if(gfx.swapChain)
	{
		gfx.swapChain->Release();
		gfx.swapChain = 0;
	}

	return;
}

void D3D11BeginScene(f32 red, f32 green, f32 blue, f32 alpha)
{
    f32 color[4];
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;

    gfx.deviceContext->ClearRenderTargetView(gfx.renderTargetView, color);

    gfx.deviceContext->ClearDepthStencilView(gfx.depthStencilView, D3D10_CLEAR_DEPTH, 1.f, 0);

    return;
}

void D3D11EndScene()
{
    if (gfx.VSYNC_ENABLED)
    {
        gfx.swapChain->Present(1, 0);
    }
    else
    {
        gfx.swapChain->Present(0, 0);
    }
    return;
}

global LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
    switch(umsg)
	{
		// Check if a key has been pressed on the keyboard.
		case WM_KEYDOWN:
		{
			// If a key is pressed send it to the input object so it can record that state.
			inputKeyDown((u32)wparam);
			return 0;
		}

		// Check if a key has been released on the keyboard.
		case WM_KEYUP:
		{
			// If a key is released then send it to the input object so it can unset the state for that key.
			inputKeyUp((u32)wparam);
			return 0;
		}

		// Any other messages send to the default message handler as our application won't make use of them.
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}
global LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}

		// All other messages pass to the message handler in the system class.
		default:
		{
			return MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}

void windowsInitalize(i32 *screenWidth, i32 *screenHeight)
{
    WNDCLASSEX wc;
    DEVMODE dm_screen_settings;
    i32 posx;
    i32 posy;
    core.hinstance = GetModuleHandle(NULL);
    core.name = TEXT("Engine");

	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = core.hinstance;
	wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm       = wc.hIcon;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = core.name;
	wc.cbSize        = sizeof(WNDCLASSEX);

    RegisterClassEx(&wc);

    *screenWidth = GetSystemMetrics(SM_CXSCREEN);
    *screenHeight = GetSystemMetrics(SM_CYSCREEN);

    if (gfx.FULL_SCREEN)
    {
        memset(&dm_screen_settings, 0, sizeof(dm_screen_settings));
		dm_screen_settings.dmSize       = sizeof(dm_screen_settings);
		dm_screen_settings.dmPelsWidth  = (unsigned long)*screenWidth;
		dm_screen_settings.dmPelsHeight = (unsigned long)*screenHeight;
		dm_screen_settings.dmBitsPerPel = 32;			
		dm_screen_settings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
        ChangeDisplaySettings(&dm_screen_settings, CDS_FULLSCREEN);
        posx = posy = 0;
    }
    else
    {
        *screenWidth = 800;
        *screenHeight = 600;
        posx = (GetSystemMetrics(SM_CXSCREEN) - *screenWidth) / 2;
        posy = (GetSystemMetrics(SM_CYSCREEN) - *screenWidth) / 2;
    }

    core.hwnd = CreateWindowEx(WS_EX_APPWINDOW, core.name, core.name, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posx, posy, *screenWidth, *screenHeight, NULL, NULL, core.hinstance, NULL);
    ShowWindow(core.hwnd, SW_SHOW);
    SetForegroundWindow(core.hwnd);
    SetFocus(core.hwnd);
    ShowCursor(false);
    return;
}

void windowsDeinitialize()
{
    ShowCursor(true);
    if (gfx.FULL_SCREEN)
    {
        ChangeDisplaySettings(NULL, 0);
    }
    DestroyWindow(core.hwnd);
    core.hwnd = NULL;
    UnregisterClass(core.name, core.hinstance);
    core.hinstance = NULL;
    return;
}

bool initialize()
{
    i32 screenWidth = 0;
    i32 screenHeight = 0;
    bool result = 0;
    gfx = {
        .FULL_SCREEN = false,
        .VSYNC_ENABLED = true,
        .SCREEN_DEPTH = 1000.f,
        .SCREEN_NEAR = .3f,
    };
    windowsInitalize(&screenWidth, &screenHeight);
    gfxInitalize(screenWidth, screenHeight, core.hwnd);
    D3D11Initialize(screenWidth, screenHeight, gfx.VSYNC_ENABLED, core.hwnd, gfx.FULL_SCREEN, gfx.SCREEN_DEPTH, gfx.SCREEN_NEAR);
    return true;
}

// auto main() -> int {
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
    Arena arena {};
    arenaInit(&arena);
    Array<i8> array = Array_reserve<i8>(&arena, 12);
    for (i32 i = 0; i < array.length; i++) {
        printf("array[%d] = %d\n", i, array.data[i]);
    }

    if (!initialize()) { return -1; }

    MSG message = {};
    core.done = false;
    while (!core.done)
    {
        if(PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        if (message.message == WM_QUIT)
        {
            core.done = true;
        }
        else
        {
            if (inputIsKeyDown(VK_ESCAPE))
            {
                core.done = true;
            }
        }
        D3D11BeginScene(1.f, 0.f, 0.f, 1.f);
        D3D11EndScene();
    }
    return 0;
}

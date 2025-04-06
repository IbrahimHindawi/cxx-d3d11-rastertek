#include <core.h>
#include <saha.h>
#include <Array.h>
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

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
};
global Gfx gfx;

void gfxInitalize(i32 screen_width, i32 screen_height, HWND hwnd)
{
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

void windowsInitalize(i32 *screen_width, i32 *screen_height)
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

    *screen_width = GetSystemMetrics(SM_CXSCREEN);
    *screen_height = GetSystemMetrics(SM_CYSCREEN);

    if (gfx.FULL_SCREEN)
    {
        memset(&dm_screen_settings, 0, sizeof(dm_screen_settings));
		dm_screen_settings.dmSize       = sizeof(dm_screen_settings);
		dm_screen_settings.dmPelsWidth  = (unsigned long)*screen_width;
		dm_screen_settings.dmPelsHeight = (unsigned long)*screen_height;
		dm_screen_settings.dmBitsPerPel = 32;			
		dm_screen_settings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
        ChangeDisplaySettings(&dm_screen_settings, CDS_FULLSCREEN);
        posx = posy = 0;
    }
    else
    {
        *screen_width = 800;
        *screen_height = 600;
        posx = (GetSystemMetrics(SM_CXSCREEN) - *screen_width) / 2;
        posy = (GetSystemMetrics(SM_CYSCREEN) - *screen_width) / 2;
    }

    core.hwnd = CreateWindowEx(WS_EX_APPWINDOW, core.name, core.name, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posx, posy, *screen_width, *screen_height, NULL, NULL, core.hinstance, NULL);
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
    i32 screen_width = 0;
    i32 screen_height = 0;
    bool result = 0;
    gfx = {
        .FULL_SCREEN = true,
        .VSYNC_ENABLED = true,
        .SCREEN_DEPTH = 1000.f,
        .SCREEN_NEAR = .3f,
    };
    windowsInitalize(&screen_width, &screen_height);
    gfxInitalize(screen_width, screen_height, core.hwnd);
    return true;
}

// auto main() -> int {
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
    Arena arena {};
    arenaInit(&arena);
    Array<i8> array = Array_reserve<i8>(&arena, 12);
    for (i32 i = 0; i < array.length; i++) {
        printf("array[%d] = %d", i, array.data[i]);
    }

    initialize();

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
    }
    return 0;
}

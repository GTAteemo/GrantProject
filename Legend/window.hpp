
#pragma once
#include "globals.hpp"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"
#include "menu.hpp"
#include "visuals.hpp"
#include <D3dx9tex.h>


struct OverlayWindow {
    WNDCLASSEX WindowClass;
    HWND Hwnd;
    LPCSTR Name;
}Overlay;

struct DirectX9Interface {
    IDirect3D9Ex* IDirect3D9 = NULL;
    IDirect3DDevice9Ex* pDevice = NULL;
    D3DPRESENT_PARAMETERS pParameters = { NULL };
    MARGINS Margin = { -1 };
    MSG Message = { NULL };
}DirectX9;

void error_message(const char* msg)
{
    std::cout << msg << std::endl;
    Sleep(2000);
    ExitProcess(EXIT_SUCCESS);
}



// Simple helper function to load an image into a DX9 texture with common settings
bool LoadTextureFromFile(const char* filename, PDIRECT3DTEXTURE9* out_texture, int* out_width, int* out_height)
{
    // Load texture from disk
    PDIRECT3DTEXTURE9 texture;
    HRESULT hr = D3DXCreateTextureFromFileA(DirectX9.pDevice, filename, &texture);
    if (hr != S_OK)
        return false;

    // Retrieve description of the texture surface so we can access its size
    D3DSURFACE_DESC my_image_desc;
    texture->GetLevelDesc(0, &my_image_desc);
    *out_texture = texture;
    *out_width = (int)my_image_desc.Width;
    *out_height = (int)my_image_desc.Height;
    return true;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
        return true;

    switch (Message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        exit(4);
        break;
    case WM_SIZE:
        if (DirectX9.pDevice != NULL && wParam != SIZE_MINIMIZED) {
            ImGui_ImplDX9_InvalidateDeviceObjects();
            DirectX9.pParameters.BackBufferWidth = LOWORD(lParam);
            DirectX9.pParameters.BackBufferHeight = HIWORD(lParam);
            HRESULT hr = DirectX9.pDevice->Reset(&DirectX9.pParameters);
            if (hr == D3DERR_INVALIDCALL)
                IM_ASSERT(0);
            ImGui_ImplDX9_CreateDeviceObjects();
        }
        break;
    default:
        return DefWindowProc(hWnd, Message, wParam, lParam);
        break;
    }
    return 0;
}


void SetupWindow() {
    Overlay.WindowClass = {
        sizeof(WNDCLASSEX), 0, WinProc, 0, 0, nullptr, LoadIcon(nullptr, IDI_APPLICATION), LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr, Overlay.Name, LoadIcon(nullptr, IDI_APPLICATION)
    };

    RegisterClassEx(&Overlay.WindowClass);
    Overlay.Hwnd = CreateWindowEx(WS_EX_TOPMOST, Overlay.Name, Overlay.Name, WS_POPUP | WS_VISIBLE, WindowLeft, WindowTop, WindowWidth, WindowHeight, NULL, NULL, 0, NULL);
    DwmExtendFrameIntoClientArea(Overlay.Hwnd, &DirectX9.Margin);
    SetWindowLong(Overlay.Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
    ShowWindow(Overlay.Hwnd, SW_SHOW);
    UpdateWindow(Overlay.Hwnd);

}


bool DirectXInit() {
    if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &DirectX9.IDirect3D9))) {
        return false;
    }

    D3DPRESENT_PARAMETERS Params = { 0 };
    Params.Windowed = TRUE;
    Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    Params.hDeviceWindow = Overlay.Hwnd;
    Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
    Params.BackBufferFormat = D3DFMT_A8R8G8B8;
    Params.BackBufferWidth = WindowWidth;
    Params.BackBufferHeight = WindowHeight;
    Params.PresentationInterval = 0;
    Params.EnableAutoDepthStencil = TRUE;
    Params.AutoDepthStencilFormat = D3DFMT_D16;
    Params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    if (FAILED(DirectX9.IDirect3D9->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Overlay.Hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &Params, 0, &DirectX9.pDevice))) {
        DirectX9.IDirect3D9->Release();
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplWin32_Init(Overlay.Hwnd);
    ImGui_ImplDX9_Init(DirectX9.pDevice);
    DirectX9.IDirect3D9->Release();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontFromFileTTF("simhei.ttf", 13.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());

    //images
    ret = LoadTextureFromFile("images/heart.png", &my_texture, &my_image_width, &my_image_height);
    IM_ASSERT(ret);

    return true;
}

void InputHandler() {
    for (int i = 0; i < 5; i++) {
        ImGui::GetIO().MouseDown[i] = false;
    }

    int Button = -1;
    if (GetAsyncKeyState(VK_LBUTTON)) {
        Button = 0;
    }

    if (Button != -1) {
        ImGui::GetIO().MouseDown[Button] = true;
    }
}
void Render() {
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    //set topup and cross window
    if (FocusWindow) {
        InputHandler();
        SetWindowLong(Overlay.Hwnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW);
        UpdateWindow(Overlay.Hwnd);
        SetFocus(Overlay.Hwnd);
        DrawMenu();
        DrawBackground();
    }
    else {
        SetWindowLong(Overlay.Hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
        UpdateWindow(Overlay.Hwnd);
    }
    /*
    drwawwww
    */
    //test
    if (GetAsyncKeyState(VK_F1) && 0x1)
    {
        Globals::Vars::ExpensiveLoot = !Globals::Vars::ExpensiveLoot;
        Sleep(200);
    }
    if (GetAsyncKeyState(VK_F2) && 0x1)
    {
        Globals::Vars::Foods = !Globals::Vars::Foods;
        Sleep(200);
    }
    if (GetAsyncKeyState(VK_F3) && 0x1)
    {
        Globals::Vars::Others = !Globals::Vars::Others;
        Sleep(200);
    }
    if (GetAsyncKeyState(VK_F4) && 0x1)
    {
        Globals::Vars::Aimbot += 1;
        if (Globals::Vars::Aimbot == 4)
        {
            Globals::Vars::Aimbot = 0;
        }
        Sleep(200);
    }
    if (GetAsyncKeyState(VK_F6) && 0x1)
    {
        Globals::Vars::Bones = !Globals::Vars::Bones;
        Sleep(200);
    }
    /*DrawRect(1, WindowHeight / 3, 180, 160, &Col.yellow, 2);
    DrawMenuText(10, WindowHeight / 3 + 20, Globals::ExpensiveLoot ? &Col.yellow : &Col.red, Globals::ExpensiveLoot ? "F1:ExpensiveLoot TRUE" : "F1:ExpensiveLoot FALSE", 15);
    DrawMenuText(10, WindowHeight / 3 + 40, Globals::Foods ? &Col.yellow : &Col.red, Globals::Foods ? "F2:Foods TRUE" : "F2:Foods FALSE", 15);
    DrawMenuText(10, WindowHeight / 3 + 60, Globals::Others ? &Col.yellow : &Col.red, Globals::Others ? "F3:Others TRUE" : "F3:Others FALSE", 15);
    DrawMenuText(10, WindowHeight / 3 + 80, Globals::AimBot ? &Col.yellow : &Col.red, Globals::AimBot ? "F4:AimBot TRUE" : "F4:AimBot FALSE", 15);
    DrawMenuText(10, WindowHeight / 3 + 100, Globals::Bones ? &Col.yellow : &Col.red, Globals::Bones ? "F5:Bones TRUE" : "F5:Bones FALSE", 15);*/

    //render


    DrawItems();
    DrawPlayers();
    DrawAimLine();
    DrawMiniMap();
    DrawTest();
    DrawAimPart();


    DirectX9.pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
    if (DirectX9.pDevice->BeginScene() >= 0) {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        DirectX9.pDevice->EndScene();
    }

    HRESULT result = DirectX9.pDevice->Present(NULL, NULL, NULL, NULL);

    if (result == D3DERR_DEVICELOST && DirectX9.pDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
        ImGui_ImplDX9_InvalidateDeviceObjects();
        DirectX9.pDevice->Reset(&DirectX9.pParameters);
        ImGui_ImplDX9_CreateDeviceObjects();
    }

    Sleep(1);
}

void MainLoop() {
    while (DirectX9.Message.message != WM_QUIT) {
        if (GetAsyncKeyState(VK_END) & 1) {
            Running = !Running;
            break;
        };
        if (GetAsyncKeyState(VK_INSERT) & 1) {
            FocusWindow = !FocusWindow;
            Sleep(200);
        };

        static RECT OldRect;
        ZeroMemory(&DirectX9.Message, sizeof(MSG));
        if (PeekMessage(&DirectX9.Message, Overlay.Hwnd, 0, 0, PM_REMOVE)) {
            TranslateMessage(&DirectX9.Message);
            DispatchMessage(&DirectX9.Message);
        }
        ImGuiIO& io = ImGui::GetIO();
        io.ImeWindowHandle = Overlay.Hwnd;

        DirectX9.pParameters.BackBufferWidth = WindowWidth;
        DirectX9.pParameters.BackBufferHeight = WindowHeight;
        SetWindowPos(Overlay.Hwnd, (HWND)0, WindowLeft, WindowTop, WindowWidth, WindowHeight, SWP_NOREDRAW);
        DirectX9.pDevice->Reset(&DirectX9.pParameters);

        Render();
    }
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    if (DirectX9.pDevice != NULL) {
        DirectX9.pDevice->EndScene();
        DirectX9.pDevice->Release();
    }
    if (DirectX9.IDirect3D9 != NULL) {
        DirectX9.IDirect3D9->Release();
    }
    DestroyWindow(Overlay.Hwnd);
    UnregisterClass(Overlay.WindowClass.lpszClassName, Overlay.WindowClass.hInstance);
}
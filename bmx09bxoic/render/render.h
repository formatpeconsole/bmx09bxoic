#pragma once
#include <d3d11.h>

#include "../deps/imguiFramework/imgui.h"
#include "../deps/imguiFramework/imgui_internal.h"
#include "../deps/imguiFramework/imgui_freetype.h"
#include "../deps/imguiFramework/imgui_impl_dx11.h"
#include "../deps/imguiFramework/imgui_impl_win32.h"

struct RenderInfo
{
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* deviceContext = nullptr;
    ID3D11RenderTargetView* renderTargetView = nullptr;
    ID3D11RenderTargetView* mainRenderTarget = nullptr;
    ImGuiContext* imguiContext = nullptr;

    HWND cs2Window = NULL;
    WNDPROC wndProcOrig = NULL;
    bool init = false;
};

namespace render
{
extern void init(IDXGISwapChain* pSwapChain);
extern void destroy();
extern void clearRenderTargetView();

extern void onRender(IDXGISwapChain* pSwapChain);
extern void onResize();

extern RenderInfo& getRenderInfoInstance();
}
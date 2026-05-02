#pragma once
#include <d3d11.h>

#include "../deps/imguiFramework/imgui.h"
#include "../deps/imguiFramework/imgui_internal.h"
#include "../deps/imguiFramework/imgui_freetype.h"
#include "../deps/imguiFramework/imgui_impl_dx11.h"
#include "../deps/imguiFramework/imgui_impl_win32.h"

enum FONTS
{
    FONT_LOGO = 0,
    FONT_ITEMS,
    FONT_WATERMARK,
    FONT_INFO,
    FONT_MAX
};

struct Font
{
    ImFont* font = nullptr;
    float size = 0.f;
};

struct RenderInfo
{
    Font fonts[FONT_MAX]{};

    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* deviceContext = nullptr;
    ID3D11RenderTargetView* renderTargetView = nullptr;
    ID3D11RenderTargetView* mainRenderTarget = nullptr;
    ImGuiContext* imguiContext = nullptr;

    HWND cs2Window = NULL;
    WNDPROC wndProcOrig = NULL;

    ImVec2 screenSize{};
    bool init = false;
};

namespace render
{
extern Font& getFont(int type);
extern void pushFont(int type);
extern void popFont();

extern void init(IDXGISwapChain* pSwapChain);
extern void destroy();
extern void clearRenderTargetView();

extern void onRender(IDXGISwapChain* pSwapChain);
extern void onResize();

extern RenderInfo& getRenderInfoInstance();
}

namespace objRender
{
extern ImDrawList* getDrawList();
extern void renderText(Font& font, ImVec2 pos, ImColor&& color, const char* string);
}
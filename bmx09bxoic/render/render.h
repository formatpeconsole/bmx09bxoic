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
extern void drawRect(ImVec2 pos, ImVec2 size, ImColor&& color, float rounding = 0.f, ImDrawFlags flags = 0, float thickness = 1.f);
extern void drawFilledRect(ImVec2 pos, ImVec2 size, ImColor&& color, float rounding = 0.f, ImDrawFlags flags = 0);
extern void drawRectShadow(ImVec2 pos, ImVec2 size, ImColor&& color, int range, float shadowAlpha, float rounding = 0.f, ImDrawFlags flags = 0);
extern void drawCircle(ImVec2 pos, float radius, ImColor&& color, int segments = 0, float thickness = 1.f);
extern void drawCircleShadow(ImVec2 pos, float radius, ImColor&& color, int range, float shadowAlpha);
extern void drawCircleFilled(ImVec2 pos, float radius, ImColor&& color, int segments = 0);
extern void drawTriangle(ImVec2 p1, ImVec2 p2, ImVec2 p3, ImColor&& color, float thickness = 1.f);
extern void drawTriangleFilled(ImVec2 p1, ImVec2 p2, ImVec2 p3, ImColor&& color);
}
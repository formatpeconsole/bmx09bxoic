#include "render.h"

#include "../hooks/hooks.h"
#include "../gui/gui.h"
#include "../gui/framework/window.h"

#include "../cheat-base/math.h"

#include "fonts/iosevkaCharon.h"

namespace render
{
using namespace gui;
using namespace gui::framework;

Font& getFont(int type)
{
    return getRenderInfoInstance().fonts[type];
}

Font makeFont(ImFontAtlas* atlas, ImFontConfig& config, const unsigned char* data, const unsigned int size, float size_pixels)
{
    Font output{};
    output.font = atlas->AddFontFromMemoryCompressedTTF(data, size, size_pixels, &config);
    output.size = size_pixels;
    return output;
}

void pushFont(int type)
{
    ImGui::PushFont(getFont(type).font);
}

void popFont()
{
    ImGui::PopFont();
}

void init(IDXGISwapChain* pSwapChain)
{
    if (getRenderInfoInstance().init)
        return;

    DXGI_SWAP_CHAIN_DESC SwapChainDesc;

    if (FAILED(pSwapChain->GetDevice(IID_PPV_ARGS(&getRenderInfoInstance().device))))
        return;

    getRenderInfoInstance().device->GetImmediateContext(&getRenderInfoInstance().deviceContext);

    if (FAILED(pSwapChain->GetDesc(&SwapChainDesc)))
        return;

    getRenderInfoInstance().cs2Window = SwapChainDesc.OutputWindow;
    getRenderInfoInstance().imguiContext = ImGui::CreateContext();

    ImGui::SetCurrentContext(getRenderInfoInstance().imguiContext);

    auto& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    auto& style = ImGui::GetStyle();
    style.FramePadding = ImVec2(4.0, 2.0);
    style.ItemSpacing = ImVec2(8.0, 2.0);
    style.WindowRounding = 1.0;
    style.FrameRounding = 1.0;
    style.ScrollbarRounding = 1.0;
    style.GrabRounding = 1.0;

    style.Colors[ImGuiCol_Text] = ImVec4(1.00, 1.00, 1.00, 0.95);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50, 0.50, 0.50, 1.00);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13, 0.12, 0.12, 1.00);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05, 0.05, 0.05, 0.94);
    style.Colors[ImGuiCol_Border] = ImVec4(0.53, 0.53, 0.53, 0.46);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00, 0.00, 0.00, 0.00);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.00, 0.00, 0.00, 0.85);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22, 0.22, 0.22, 0.40);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.16, 0.16, 0.16, 0.53);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.00, 0.00, 0.00, 1.00);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00, 0.00, 0.00, 1.00);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00, 0.00, 0.00, 0.51);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.12, 0.12, 0.12, 1.00);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02, 0.02, 0.02, 0.53);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31, 0.31, 0.31, 1.00);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41, 0.41, 0.41, 1.00);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.48, 0.48, 0.48, 1.00);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.79, 0.79, 0.79, 1.00);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.48, 0.47, 0.47, 0.91);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56, 0.55, 0.55, 0.62);
    style.Colors[ImGuiCol_Button] = ImVec4(0.50, 0.50, 0.50, 0.63);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.67, 0.67, 0.68, 0.63);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.26, 0.26, 0.26, 0.63);
    style.Colors[ImGuiCol_Header] = ImVec4(0.54, 0.54, 0.54, 0.58);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.64, 0.65, 0.65, 0.80);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.25, 0.25, 0.25, 0.80);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.58, 0.58, 0.58, 0.50);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.81, 0.81, 0.81, 0.64);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.81, 0.81, 0.81, 0.64);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.87, 0.87, 0.87, 0.53);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.87, 0.87, 0.87, 0.74);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.87, 0.87, 0.87, 0.74);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.68, 0.68, 0.68, 1.00);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.68, 0.68, 0.68, 1.00);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90, 0.77, 0.33, 1.00);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.87, 0.55, 0.08, 1.00);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.47, 0.60, 0.76, 0.47);

    ImFontConfig config{};
    config.OversampleH = 3;
    config.OversampleV = 3;

    getRenderInfoInstance().fonts[FONT_LOGO] = makeFont(io.Fonts, config, CheatFont_compressed_data, CheatFont_compressed_size, 33.f * DPI_SCALE);
    getRenderInfoInstance().fonts[FONT_ITEMS] = makeFont(io.Fonts, config, CheatFont_compressed_data, CheatFont_compressed_size, 20.f * DPI_SCALE);
    getRenderInfoInstance().fonts[FONT_WATERMARK] = makeFont(io.Fonts, config, CheatFont_compressed_data, CheatFont_compressed_size, 20.f * DPI_SCALE);
    getRenderInfoInstance().fonts[FONT_INFO] = makeFont(io.Fonts, config, CheatFont_compressed_data, CheatFont_compressed_size, 20.f * DPI_SCALE);

    ImGui_ImplWin32_Init(getRenderInfoInstance().cs2Window);
    ImGui_ImplDX11_Init(getRenderInfoInstance().device, getRenderInfoInstance().deviceContext);

    getRenderInfoInstance().wndProcOrig = (WNDPROC)SetWindowLongPtrA(getRenderInfoInstance().cs2Window, GWLP_WNDPROC, (LONG_PTR)hooks::Hooked_WndProc);

    ImGui_ImplDX11_InvalidateDeviceObjects();
    ImGui_ImplDX11_CreateDeviceObjects();

    RegisterHotKey(getRenderInfoInstance().cs2Window, 100, 0, VK_MENU);
    RegisterHotKey(getRenderInfoInstance().cs2Window, 100, 0, VK_LMENU);
    RegisterHotKey(getRenderInfoInstance().cs2Window, 100, 0, VK_RMENU);

    RECT clientRect{};
    GetClientRect(getRenderInfoInstance().cs2Window, &clientRect);

    getRenderInfoInstance().screenSize = ImVec2(static_cast<float>(clientRect.right), static_cast<float>(clientRect.bottom));
    getRenderInfoInstance().init = true;
}

void clearRenderTargetView()
{
    if (getRenderInfoInstance().renderTargetView)
    {
        getRenderInfoInstance().renderTargetView->Release();
        getRenderInfoInstance().renderTargetView = nullptr;
    }
}

void destroy()
{
    SetWindowLongPtrA(getRenderInfoInstance().cs2Window, GWLP_WNDPROC, (LONG_PTR)getRenderInfoInstance().wndProcOrig);

    clearRenderTargetView();

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();

    ImGui::DestroyContext();

    getRenderInfoInstance().init = false;
}

void onResize()
{
    getMenuInstance().windowsManager.backupAllUiPositions(framework::DPI_SCALE);
    destroy();
}

void onRender(IDXGISwapChain* pSwapChain)
{
    getMenuInstance().windowsManager.updateDpiScale();

    if (!getRenderInfoInstance().init)
        init(pSwapChain);
    else
    {
        if (!getRenderInfoInstance().renderTargetView)
        {
            ID3D11Texture2D* pBackBuffer = nullptr;
            pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

            D3D11_RENDER_TARGET_VIEW_DESC RenderTargetDesc;
            memset(&RenderTargetDesc, 0, sizeof(RenderTargetDesc));

            RenderTargetDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            RenderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

            if (pBackBuffer)
            {
                getRenderInfoInstance().device->CreateRenderTargetView(pBackBuffer, &RenderTargetDesc, &getRenderInfoInstance().renderTargetView);
                pBackBuffer->Release();
            }
        }

        ImGui::SetCurrentContext(getRenderInfoInstance().imguiContext);

        getRenderInfoInstance().deviceContext->OMGetRenderTargets(1, &getRenderInfoInstance().mainRenderTarget, 0);
        getRenderInfoInstance().deviceContext->OMSetRenderTargets(1, &getRenderInfoInstance().renderTargetView, 0);

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();

        ImGui::NewFrame();

        getMenuInstance().renderWindows();

        ImGui::EndFrame();
        ImGui::Render();

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        getRenderInfoInstance().deviceContext->OMSetRenderTargets(1, &getRenderInfoInstance().mainRenderTarget, 0);
    }
}

RenderInfo& getRenderInfoInstance()
{
    static RenderInfo renderInfo;
    return renderInfo;
}
}

namespace objRender
{
ImDrawList* getDrawList()
{
    return ImGui::GetBackgroundDrawList();
}

void renderText(Font& font, ImVec2 pos, ImColor&& color, const char* string)
{
    auto drawList = getDrawList();
    drawList->PushTextureID(font.font->OwnerAtlas->TexID);
    drawList->AddText(font.font, font.size, pos, color, string);
    drawList->PopTextureID();
}

void drawRect(ImVec2 pos, ImVec2 size, ImColor&& color, float rounding, ImDrawFlags flags, float thickness)
{
    auto drawList = getDrawList();
    drawList->AddRect(pos, pos + size, color, rounding, flags, thickness);
}

void drawFilledRect(ImVec2 pos, ImVec2 size, ImColor&& color, float rounding, ImDrawFlags flags)
{
    auto drawList = getDrawList();
    drawList->AddRectFilled(pos, pos + size, color, rounding, flags);
}

void drawRectShadow(ImVec2 pos, ImVec2 size, ImColor&& color, int range, float shadowAlpha, float rounding, ImDrawFlags flags)
{
    auto minPos = pos;
    auto maxPos = pos + size;

    auto drawList = getDrawList();
    for (int i = 0; i < range; ++i)
    {
        float step = (math::toFloat(i) / math::toFloat(range - 1));
        float alphaStep = std::lerp(shadowAlpha, 0.f, step);
        int bgAlpha = math::toInt(alphaStep);
        float alpha = math::toFloat(bgAlpha) / 255.f;
        ImColor newColor = ImColor(color.Value.x, color.Value.y, color.Value.z, color.Value.w * alpha);

        float offsetStep = math::toFloat(i);
        drawList->AddRect(
            ImVec2{ math::toFloat(math::toInt(minPos.x - offsetStep)), math::toFloat(math::toInt(minPos.y - offsetStep)) },
            ImVec2{ math::toFloat(math::toInt(maxPos.x + offsetStep)), math::toFloat(math::toInt(maxPos.y + offsetStep)) },
            newColor,
            rounding, flags, 1.f);
    }
}
}
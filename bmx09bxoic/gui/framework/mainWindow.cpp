#include <bit>

#include "mainWindow.h"

#include "../gui.h"
#include "../item.h"
#include "../../guiItems/items.h"
#include "../../main/dllInstance.h"

#include "../../guiItems/items.h"
#include "../../config/config.h"

#include "../../cheat-base/math.h"

namespace gui::framework
{
void renderSubTabs(const std::vector<std::string>& tabs, int& selection)
{
    ImGui::BeginGroup();
    for (int i = 0; i < tabs.size(); ++i)
    {
        if (ImGui::Button(tabs[i].c_str(), ImVec2(tabs[i].length() * 10, 18)))
            selection = i;

        ImGui::SameLine();
    }
    ImGui::EndGroup();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
}

void renderRageTab()
{
    ImVec2 weaponitemsChildSize = ImVec2(374 * DPI_SCALE, 440 * DPI_SCALE);
    ImVec2 mainChildSize = ImVec2(weaponitemsChildSize.x, weaponitemsChildSize.y * 0.272f);
    ImVec2 antiAimChildSize = ImVec2(weaponitemsChildSize.x, weaponitemsChildSize.y * 0.95f);
    ImVec2 configChildSize = ImVec2(weaponitemsChildSize.x, 60 * DPI_SCALE);

    ImGui::BeginGroup();
    {
        ImGui::BeginChild("Main##rage", mainChildSize, ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar);
        {
            checkbox::render(getMenuInstance().rage.enable);
            checkbox::render(getMenuInstance().rage.autoRevolver);
            checkbox::render(getMenuInstance().rage.doubleTap);
            checkbox::render(getMenuInstance().rage.noSpread);
            checkbox::render(getMenuInstance().rage.duckPeekAssist);
        }
        ImGui::EndChild();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::BeginChild("Anti Aim##rage", antiAimChildSize, ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar);
        {
            checkbox::render(getMenuInstance().rage.antiAim.enable);
            checkbox::render(getMenuInstance().rage.antiAim.atTarget);
            combobox::render(getMenuInstance().rage.antiAim.pitch);
            combobox::render(getMenuInstance().rage.antiAim.yaw);
            combobox::render(getMenuInstance().rage.antiAim.jitter);
            slider::render(getMenuInstance().rage.antiAim.jitterOffset);
            slider::render(getMenuInstance().rage.antiAim.yawOffset);

            ImGui::BeginGroup();
            {
                checkbox::render(getMenuInstance().rage.antiAim.freestanding);
                ImGui::SameLine();

                if (ImGui::SmallButton("..."))
                {
                    ImGui::OpenPopup("freestand-popup");
                }

                if (ImGui::BeginPopup("freestand-popup"))
                {
                    checkbox::render(getMenuInstance().rage.antiAim.zeroOnPeek);
                    ImGui::EndPopup();
                }
            }
            ImGui::EndGroup();
        }
        ImGui::EndChild();
    }
    ImGui::EndGroup();

    ImGui::SameLine();

    ImGui::BeginGroup();
    {
        ImGui::BeginChild("Weapon Config Selector##rage", configChildSize, ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar);
        {
            ImGui::PushItemWidth(configChildSize.x * 0.958f);
            combobox::render(getMenuInstance().rage.configSelect);
            ImGui::PopItemWidth();
        }
        ImGui::EndChild();
        ImGui::Spacing();
        ImGui::BeginChild("Weapon Config##rage", weaponitemsChildSize, ImGuiChildFlags_Borders, ImGuiWindowFlags_NoScrollbar);
        {
            auto& itemFromWeaponConfig = getMenuInstance().rage.config[getMenuInstance().rage.configSelect.item.value];

            ImGui::PushItemWidth(configChildSize.x * 0.958f);
            if (getMenuInstance().rage.configSelect.item.value != 0)
                checkbox::render(itemFromWeaponConfig.overrideGlobal);

            checkbox::render(itemFromWeaponConfig.autoFire);
            checkbox::render(itemFromWeaponConfig.autoScope);
            slider::render(itemFromWeaponConfig.fov);
            multicombobox::render(itemFromWeaponConfig.hitBoxes);
            multicombobox::render(itemFromWeaponConfig.multiPoints);
            slider::render(itemFromWeaponConfig.pointHeadScale);
            slider::render(itemFromWeaponConfig.pointBodyScale);
            checkbox::render(itemFromWeaponConfig.preferBody);
            slider::render(itemFromWeaponConfig.hitChance);
            slider::render(itemFromWeaponConfig.minDamage);
            multicombobox::render(itemFromWeaponConfig.quickStop);
            ImGui::PopItemWidth();
        }
        ImGui::EndChild();
    }
    ImGui::EndGroup();
}

void renderLegitTab()
{

}

void renderVisualsTab()
{

}

void renderMiscTab()
{

}

void renderSkinsTab()
{
}

void renderConfigsTab()
{
    ImGui::BeginGroup();
    {
        combobox::render(getMenuInstance().dpiScale);

        if (ImGui::SmallButton("Save"))
            config::saveConfig();
        ImGui::SameLine();
        if (ImGui::SmallButton("Load"))
            config::loadConfig();
    }
    ImGui::EndGroup();

#ifdef _DEBUG
    if (ImGui::SmallButton("Unload"))
        getDllInstance().shouldQuit = true;
#endif
}

void renderLUATab()
{
}

bool tabButton(const char* name, ImVec2 sizeArg, bool active, tabAnimation& animation, int mainAlpha)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(name);
    const ImVec2 label_size = ImGui::CalcTextSize(name, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = ImGui::CalcItemSize(sizeArg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

    const ImRect bb(pos, pos + size);
    ImGui::ItemSize(size, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool hovered{}, held{};
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

    ImColor baseColor{};
    float colorAdd = (static_cast<float>(hovered) + static_cast<float>(held)) * 0.15f;
    float mainColor = active ? 1.f : 0.68f + colorAdd;
    baseColor.Value.x = baseColor.Value.y = baseColor.Value.z = mainColor;
    baseColor.Value.w = static_cast<float>(mainAlpha) / 255.f;

    ImColor radioButtonColor = active ? MAIN_WINDOW_ACCENT_COLOR : baseColor;
    radioButtonColor.Value.w = static_cast<float>(mainAlpha) / 255.f;

    animation.radioButton = ImLerp(animation.radioButton, radioButtonColor.Value, 0.1f);
    animation.text = ImLerp(animation.text, baseColor.Value, 0.1f);

    auto basePos = pos + ImVec2(3.f, 10.f);

    auto drawList = objRender::getDrawList();
    drawList->AddCircleFilled(basePos, 3.5f, ImColor(animation.radioButton));

    auto textPos = pos + ImVec2(20.f, 0.f);;
    objRender::renderText(render::getFont(FONT_ITEMS), textPos, ImColor(animation.text), name);

   // drawList->AddRect(pos, pos + size, ImColor(255, 255, 255, 255));

    return pressed;
}

MainWindow::MainWindow(tabsList tabs, std::string name, ImVec2 size)
    : tabs(tabs), name(name), size(size), tabSelection(0), prevDpiScale(1.f), windowAlpha(0.f)
{
    tabsAnimations.resize(tabs.size());
}

MainWindow::~MainWindow()
{
    tabsAnimations.clear();
}

int MainWindow::getMainAlpha()
{
    return static_cast<int>(255.f * windowAlpha);
}

std::string MainWindow::getBuildType()
{
#ifdef _DEBUG
    return "Debug";
#else
    return "Release";
#endif
}

std::string MainWindow::getBuildDate()
{
    return __DATE__;
}

void MainWindow::renderLogo()
{
    auto pos = ImGui::GetWindowPos();
    ImVec2 logoPos = pos + ImVec2(33.f, 22.f);

    objRender::renderText(render::getFont(FONT_LOGO), logoPos, ImColor(255, 255, 255, getMainAlpha()), name.c_str());
}

void MainWindow::renderBottomInfo()
{
    auto pos = ImGui::GetWindowPos();
    ImVec2 buildTextPos = pos + ImVec2(33.f, size.y - 76.f);
    ImVec2 dateTextPos = pos + ImVec2(33.f, size.y - 49.f);

    objRender::renderText(render::getFont(FONT_INFO), buildTextPos, ImColor(255, 255, 255, getMainAlpha()), getBuildType().c_str());
    objRender::renderText(render::getFont(FONT_INFO), dateTextPos, ImColor(255, 255, 255, getMainAlpha()), getBuildDate().c_str());
}

void MainWindow::renderTabs()
{
    ImGui::SetCursorPos(ImVec2(34.f, 84.f));
    ImGui::BeginGroup();
    {
        ImGui::PushFont(render::getFont(FONT_ITEMS).font);

        int mainAlpha = getMainAlpha();
        for (int i = 0; i < tabs.size(); ++i)
        {
            float buttonXSize = static_cast<float>(tabs[i].length() * 11) * DPI_SCALE;

            if (tabButton(tabs[i].c_str(), ImVec2(buttonXSize, 20.f * DPI_SCALE), i == tabSelection, tabsAnimations[i], mainAlpha))
                tabSelection = i;
            
            ImGui::Dummy({0.f, 12.f * DPI_SCALE});
        }
        ImGui::PopFont();
    }
    ImGui::EndGroup();
}

void MainWindow::renderWindowContents()
{
    {
        renderTabs();
    }

    renderLogo();
    renderBottomInfo();
}

void MainWindow::init()
{

}

void MainWindow::render()
{
    getMenuInstance().windowsManager.correctSavedUIPos(name);

    if (prevDpiScale != DPI_SCALE)
    {
        float scaleStep = DPI_SCALE / prevDpiScale;

        size *= scaleStep;
        prevDpiScale = DPI_SCALE;
    }

    float deltaTime = math::roundFloat(ImGui::GetIO().DeltaTime * 15.f, 2) + 0.005f;
    if (getMenuInstance().opened)
    {
        if (windowAlpha < 1.f)
            windowAlpha += deltaTime;
    }
    else
    {
        if (windowAlpha > 0.f)
            windowAlpha -= deltaTime;
    }

    windowAlpha = std::clamp(windowAlpha, 0.0f, 1.0f);
    if (windowAlpha <= 0.f)
        return;

    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowBgAlpha(windowAlpha);

    std::string uiName = name + "##bmx0bxoic";

    auto styleColor = ImGui::GetStyleColorVec4(ImGuiCol_Border);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 17.f);
    ImGui::Begin(name.c_str(), &getMenuInstance().opened,
        ImGuiWindowFlags_NoBackground
        | ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoResize);
    {
        auto pos = ImGui::GetWindowPos();
        auto drawList = objRender::getDrawList();

        auto minPos = pos;
        auto maxPos = pos + size;

        // main bg
        {
            drawList->AddRectFilled(minPos, maxPos, ImColor(42, 42, 42, getMainAlpha()), 17.f);

            // main ui (name, items, info)
            renderWindowContents();
        }

        // bg border
        {
            // border dropshadow
            const int max = 10;
            for (int i = 0; i < max; ++i)
            {
                float step = 1.f - (static_cast<float>(i) / static_cast<float>(max - 1));
                int alphaStep = std::lerp(0, 80, step);
                int bgAlpha = static_cast<int>(alphaStep * windowAlpha);

                float offsetStep = 2.f + 1.f * static_cast<float>(i);
                drawList->AddRect(
                    ImVec2{ static_cast<float>(static_cast<int>(minPos.x - offsetStep)), static_cast<float>(static_cast<int>(minPos.y - offsetStep)) },
                    ImVec2{ static_cast<float>(static_cast<int>(maxPos.x + offsetStep)), static_cast<float>(static_cast<int>(maxPos.y + offsetStep)) },
                    ImColor(0, 0, 0, bgAlpha),
                    17.f, 0, 1.f);
            }

            // border
            int borderBgAlpha = static_cast<int>(7.f * windowAlpha);
            drawList->AddRect({ minPos.x - 1.f, minPos.y - 1.f }, { maxPos.x + 1.f, maxPos.y + 1.f }, ImColor(91, 91, 91, borderBgAlpha), 17.f);
        }
    }
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();
}

void MainWindow::updateWindowPosOrSize()
{
    const auto latestWindow = GImGui->Windows.back();
    if (latestWindow != nullptr)
        pos = latestWindow->Pos;
}

std::string MainWindow::getName()
{
    return name;
}

ImVec2 MainWindow::getPos()
{
    return pos;
}

ImVec2 MainWindow::getWindowSize()
{
    return size;
}
}
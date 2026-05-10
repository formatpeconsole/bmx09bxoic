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

ImVec4 interpolateWithoutAlpha(const ImVec4& start, const ImVec4& end, float step, float mainAlpha)
{
    if (start.isEmpty())
        return ImVec4(end.x, end.y, end.z, mainAlpha);
    else
    {
        return ImVec4(start.x + (end.x - start.x) * step,
            start.y + (end.y - start.y) * step,
            start.z + (end.z - start.z) * step,
            mainAlpha);
    }
}

bool tabButton(const char* name, ImVec2 sizeArg, bool active, tabAnimation& animation, int mainAlpha)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(name);
    const ImVec2 label_size = ImGui::CalcTextSize(name, NULL, true);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size = sizeArg;

    const ImRect bb(pos, pos + size);
    ImGui::ItemSize(size, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool hovered{}, held{};
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

    // render tab selection
    {
        float uiAlpha = static_cast<float>(mainAlpha) / 255.f;

        float colorAdd = (static_cast<float>(hovered) + static_cast<float>(held)) * 0.15f;
        float radioButtonColorBase = active ? 1.f : 0.f + colorAdd;
        float textColorBase = active ? 1.f : 0.635f + colorAdd;

        radioButtonColorBase = std::clamp(radioButtonColorBase, 0.f, 1.f);
        textColorBase = std::clamp(textColorBase, 0.f, 1.f);

        ImColor baseColor{ radioButtonColorBase, radioButtonColorBase, radioButtonColorBase, 0.f };
        ImColor textColor{ textColorBase, textColorBase, textColorBase, 0.f };

        ImColor radioButtonColor = active ? MAIN_WINDOW_ACCENT_COLOR : baseColor;

        animation.radioAnimation.setCondition(!active);
        animation.radioAnimation.process();

        animation.radioButton = interpolateWithoutAlpha(animation.radioButton, radioButtonColor.Value, 0.1f, uiAlpha);
        animation.text = interpolateWithoutAlpha(animation.text, textColor.Value, 0.1f, uiAlpha);

        animation.radioGlowAlpha = animation.radioAnimation.getAnimatedValue() * 0.01f;

        auto basePos = pos + ImVec2(4.f, 10.f) * DPI_SCALE;
        auto drawList = objRender::getDrawList();
        if (animation.radioGlowAlpha > 0.0f)
        {
            const int maxRange = 15;
            ImColor radioColor = animation.radioButton;
            for (int i = 0; i < maxRange; ++i)
            {
                float step = (static_cast<float>(i) / static_cast<float>(maxRange - 1));
                float alphaStep = std::lerp(20.f, 0.f, step);
                int bgAlpha = static_cast<int>(alphaStep * uiAlpha);

                float radius = std::lerp(3.5f, 10.f, step) * DPI_SCALE;

                radioColor.Value.w = (static_cast<float>(bgAlpha) / 255.f) * animation.radioGlowAlpha;
                drawList->AddCircle(basePos, radius, radioColor);
            }
        }

        drawList->AddCircleFilled(basePos, 3.5f * DPI_SCALE, ImColor(animation.radioButton));

        auto textPos = pos + ImVec2(21.f, 0.f) * DPI_SCALE;
        objRender::renderText(render::getFont(FONT_ITEMS), textPos, ImColor(animation.text), name);

        // drawList->AddRect(pos, pos + size, ImColor(255, 255, 255, 255));
    }

    return pressed;
}

MainWindow::MainWindow(tabsList tabs, std::string name, ImVec2 size)
    : tabs(tabs), name(name), size(size), tabSelection(0), prevDpiScale(1.f), windowAlpha(0.f)
{
    tabsAnimations.resize(tabs.size());
}

MainWindow::~MainWindow()
{
    tabs.clear();
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
    ImVec2 logoPos = pos + mainPos.logo * DPI_SCALE;

    objRender::renderText(render::getFont(FONT_LOGO), logoPos, ImColor(255, 255, 255, getMainAlpha()), name.c_str());
}

void MainWindow::renderBottomInfo()
{
    auto pos = ImGui::GetWindowPos();
    ImVec2 buildTextPos = pos + ImVec2(33.f * DPI_SCALE, size.y - 76.f * DPI_SCALE);
    ImVec2 dateTextPos = pos + ImVec2(33.f * DPI_SCALE, size.y - 49.f * DPI_SCALE);

    objRender::renderText(render::getFont(FONT_INFO), buildTextPos, ImColor(255, 255, 255, getMainAlpha()), getBuildType().c_str());
    objRender::renderText(render::getFont(FONT_INFO), dateTextPos, ImColor(255, 255, 255, getMainAlpha()), getBuildDate().c_str());
}

void MainWindow::renderTabs()
{
    ImGui::SetCursorPos(mainPos.baseTabs * DPI_SCALE);
    ImGui::BeginGroup();
    {
        ImGui::PushFont(render::getFont(FONT_ITEMS).font);

        int mainAlpha = getMainAlpha();

        float buttonYSize = 20.f * DPI_SCALE;
        float dummyYSize = 12.f * DPI_SCALE;
        for (int i = 0; i < tabs.size(); ++i)
        {
            auto tabName = tabs[i].name;
            float buttonXSize = static_cast<float>(tabName.length() * 11) * DPI_SCALE;

            if (i == tabSelection)
            {
                tabSelectionAnim.beginPosY = ImGui::GetCursorPosY();

                if (tabSelectionAnim.yPos == 0.f)
                    tabSelectionAnim.yPos = tabSelectionAnim.beginPosY + 3.f * DPI_SCALE;
            }

            if (tabButton(tabName.c_str(), ImVec2(buttonXSize, buttonYSize), i == tabSelection, tabsAnimations[i], mainAlpha))
                tabSelection = i;

            ImGui::Dummy({ 0.f, dummyYSize });
        }

        float triangleButtonCenterPosY = tabSelectionAnim.beginPosY + 3.f * DPI_SCALE;
        tabSelectionAnim.xPos = tabSelectionAnim.base.x * DPI_SCALE;
        tabSelectionAnim.yPos = std::lerp(tabSelectionAnim.yPos, triangleButtonCenterPosY, 0.1f);

        auto drawList = objRender::getDrawList();
        auto pos = ImGui::GetWindowPos() + ImVec2(tabSelectionAnim.xPos, tabSelectionAnim.yPos);

        // tab selection ( RAGE > )
        // shadow
        {
            ImVec2 center = pos + (tabSelectionAnim.first + tabSelectionAnim.second + tabSelectionAnim.third) * (1.f / 3.f) * DPI_SCALE;
            const int maxRange = 19;

            ImColor newClr = MAIN_WINDOW_ACCENT_COLOR;
            for (int i = 0; i < maxRange; ++i)
            {
                float step = (static_cast<float>(i) / static_cast<float>(maxRange - 1));
                float alphaStep = std::lerp(30.f, 0.f, step);
                int bgAlpha = static_cast<int>(alphaStep * windowAlpha);

                float radius = std::lerp(3.5f, static_cast<float>(maxRange) - 5.f, step) * DPI_SCALE;
                newClr.Value.w = (static_cast<float>(bgAlpha) / 255.f);
                drawList->AddCircle(center, radius, newClr);
            }
        }

        // selection
        {
            ImVec2 first = pos + tabSelectionAnim.first * DPI_SCALE;
            ImVec2 second = pos + tabSelectionAnim.second * DPI_SCALE;
            ImVec2 third = pos + tabSelectionAnim.third * DPI_SCALE;

            ImColor newClr = MAIN_WINDOW_ACCENT_COLOR;
            newClr.Value.w = static_cast<float>(mainAlpha) / 255.f;

            drawList->AddTriangleFilled(first, second, third, newClr);
        }

        ImGui::PopFont();
    }
    ImGui::EndGroup();
}

void MainWindow::renderTabsContents()
{
    // animate child contents 
    // depends on which tab have subtabs
    auto& selectedTab = tabs[tabSelection];
    std::optional<subTab> selectedSubTab = {};
    if (!selectedTab.noSubTabs)
        selectedSubTab = selectedTab.subTabs[selectedTab.subTabSelection];;

    tabContentsAnim.yPosAnimation.setCondition(selectedTab.noSubTabs);
    tabContentsAnim.ySizeAnimation.setCondition(selectedTab.noSubTabs);
    tabContentsAnim.yChildSizeAnimation.setCondition(selectedTab.noSubTabs);

    tabContentsAnim.yPosAnimation.process();
    tabContentsAnim.ySizeAnimation.process();
    tabContentsAnim.yChildSizeAnimation.process();

    tabContentsAnim.yPos = tabContentsAnim.yPosAnimation.getAnimatedValue();
    tabContentsAnim.ySize = tabContentsAnim.ySizeAnimation.getAnimatedValue();
    tabContentsAnim.yChildSize = tabContentsAnim.yChildSizeAnimation.getAnimatedValue();

    tabContentsAnim.selectedTabAnimation.setCondition(tabSelection == oldTabSelection);
    tabContentsAnim.selectedTabAnimation.process();

    oldTabSelection = tabSelection;

    bool noMoreChilds = true;
    if (selectedSubTab.has_value() && selectedSubTab->childCount > 1)
        noMoreChilds = false;

    tabContentsAnim.xChildSizeAnimation.setCondition(noMoreChilds);
    tabContentsAnim.xChildSizeAnimation.process();
    tabContentsAnim.xChildSize = static_cast<float>(static_cast<int>(tabContentsAnim.xChildSizeAnimation.getAnimatedValue()));

    mainPos.baseTabsContents.y = static_cast<float>(static_cast<int>(tabContentsAnim.yPos));

    auto subTabsCursorPos = mainPos.baseSubTabsContents * DPI_SCALE;
    ImGui::SetCursorPos(subTabsCursorPos);
    ImGui::BeginGroup();
    {
        ImGui::PushFont(render::getFont(FONT_ITEMS).font);
        
        int mainAlpha = static_cast<int>(windowAlpha * (tabContentsAnim.selectedTabAnimation.getAnimatedValue() * 0.01f) * 255.f);

        float buttonYSize = 20.f * DPI_SCALE;
        float buttonXSpacing = 29.f * DPI_SCALE;

        const auto size = selectedTab.subTabs.size();

        float basicSpacing = 0.f;
        for (size_t i = 0; i < size; ++i)
        {
            auto tabName = selectedTab.subTabs[i].name;
            float buttonXSize = static_cast<float>(tabName.length() * 11) * DPI_SCALE;
            if (tabButton(tabName.c_str(),
                ImVec2(buttonXSize, buttonYSize),
                i == selectedTab.subTabSelection,
                selectedTab.subTabAnimations[i],
                mainAlpha))
                selectedTab.subTabSelection = i;

            basicSpacing += (buttonXSize + buttonXSpacing);
            ImGui::SameLine(basicSpacing);
        }

        ImGui::PopFont();
    }
    ImGui::EndGroup();

    auto cursorPos = mainPos.baseTabsContents * DPI_SCALE;
    ImGui::SetCursorPos(cursorPos);
    ImGui::BeginGroup();
    {
        auto currentPos = ImGui::GetWindowPos() + cursorPos;
        auto childSize = ImVec2(484.f, static_cast<float>(static_cast<int>(tabContentsAnim.ySize))) * DPI_SCALE;

        ImGui::BeginChild("MainTabs##bmx09bxoic", childSize, ImGuiChildFlags_Borders);
        {
            auto drawList = objRender::getDrawList();
            drawList->AddRectFilled(currentPos, currentPos + childSize, ImColor(18, 18, 18, getMainAlpha()), 17.f);

            ImVec2 offset = ImVec2(1.f, 1.f) * DPI_SCALE;
            ImGui::SetCursorPos(ImGui::GetCursorPos() + offset);
            {
                const auto defaultSubChildSizeX = 218.f;
                auto prevCursorPos = ImGui::GetCursorPos();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((defaultSubChildSizeX + 14.f) * DPI_SCALE));
                {
                    auto& secondChildAnimation = tabContentsAnim.subChildsAlphaAnimation[CHILD_SECOND];

                    bool noSecondChild = true;
                    if (selectedSubTab.has_value() && selectedSubTab->childCount > 1)
                        noSecondChild = false;

                    secondChildAnimation.setCondition(noSecondChild);
                    secondChildAnimation.process();

                    tabContentsAnim.ySubChildFactorAnimation.setCondition(selectedSubTab.has_value() && selectedSubTab->childCount == 3);
                    tabContentsAnim.ySubChildFactorAnimation.process();

                    float secondChildYMultiplier = tabContentsAnim.ySubChildFactorAnimation.getAnimatedValue() * 0.01f;

                    bool noThirdChild = false;
                    if (selectedSubTab.has_value() && selectedSubTab->childCount < 3)
                        noThirdChild = true;

                    auto& thirdChildAnimation = tabContentsAnim.subChildsAlphaAnimation[CHILD_THIRD];
                    thirdChildAnimation.setCondition(noThirdChild);
                    thirdChildAnimation.process();

                    auto prevCursorPos = ImGui::GetCursorPos();
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (tabContentsAnim.yChildSize * 0.53f) * DPI_SCALE);
                    {
                        int thirdChildAlpha = static_cast<int>(static_cast<float>(getMainAlpha() * thirdChildAnimation.getAnimatedValue() * 0.01f));
                        if (thirdChildAlpha > 0)
                        {
                            ImVec2 groupBoxSize = ImVec2(218.f, static_cast<float>(static_cast<int>(tabContentsAnim.yChildSize * 0.47f))) * DPI_SCALE;
                            ImGui::BeginChild("Third##bmx09bxoic", groupBoxSize, ImGuiChildFlags_Borders);
                            {
                                auto currentGroupBoxPos = ImGui::GetWindowPos() + ImGui::GetCursorPos();
                                drawList->AddRectFilled(currentGroupBoxPos, currentGroupBoxPos + groupBoxSize, ImColor(33, 33, 33, thirdChildAlpha), 17.f);
                            }
                            ImGui::EndChild();
                        }
                    }

                    ImGui::SetCursorPos(prevCursorPos);
                    {
                        int secondChildAlpha = static_cast<int>(static_cast<float>(getMainAlpha() * secondChildAnimation.getAnimatedValue() * 0.01f));
                        if (secondChildAlpha > 0)
                        {
                            ImVec2 groupBoxSize = ImVec2(218.f, static_cast<float>(static_cast<int>(tabContentsAnim.yChildSize * secondChildYMultiplier))) * DPI_SCALE;
                            ImGui::BeginChild("Second##bmx09bxoic", groupBoxSize, ImGuiChildFlags_Borders);
                            {
                                auto currentGroupBoxPos = ImGui::GetWindowPos() + ImGui::GetCursorPos();
                                drawList->AddRectFilled(currentGroupBoxPos, currentGroupBoxPos + groupBoxSize, ImColor(33, 33, 33, secondChildAlpha), 17.f);
                            }
                            ImGui::EndChild();
                        }
                    }
                }

                {
                    ImVec2 groupBoxSize = ImVec2(tabContentsAnim.xChildSize, static_cast<float>(static_cast<int>(tabContentsAnim.yChildSize))) * DPI_SCALE;
                    ImGui::SetCursorPos(prevCursorPos);
                    ImGui::BeginChild("First##bmx09bxoic", groupBoxSize, ImGuiChildFlags_Borders);
                    {
                        auto currentGroupBoxPos = ImGui::GetWindowPos() + ImGui::GetCursorPos();
                        drawList->AddRectFilled(currentGroupBoxPos, currentGroupBoxPos + groupBoxSize, ImColor(33, 33, 33, getMainAlpha()), 17.f);
                    }
                    ImGui::EndChild();
                }
            }
        }
        ImGui::EndChild();
    }
    ImGui::EndGroup();
}

void MainWindow::renderWindowContents()
{
    renderTabs();
    renderTabsContents();

    renderLogo();
    renderBottomInfo();
}

void MainWindow::init()
{
    tabSelectionAnim.yPos = 0.f;
    tabSelectionAnim.xPos = 0.f;
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

    uiOpenAnimation.setCondition(getMenuInstance().opened);
    uiOpenAnimation.process();

    windowAlpha = static_cast<float>(static_cast<int>(uiOpenAnimation.getAnimatedValue()) * 0.01f);
    if (windowAlpha <= 0.f)
        return;

    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowBgAlpha(windowAlpha);

    std::string uiName = name + "##bmx09bxoic";

    auto styleColor = ImGui::GetStyleColorVec4(ImGuiCol_Border);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 17.f);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, windowAlpha);
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
            drawList->AddRectFilled(minPos, maxPos, ImColor(33, 33, 33, getMainAlpha()), 17.f);

            // main ui (name, items, info)
            renderWindowContents();
        }

        // bg border
        {
            // border shadow
            const int max = static_cast<int>(10.f * DPI_SCALE);
            for (int i = 0; i < max; ++i)
            {
                float step = (static_cast<float>(i) / static_cast<float>(max - 1));
                float alphaStep = std::lerp(45.f, 0.f, step);
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
            drawList->AddRect({ minPos.x - 1.f, minPos.y - 1.f }, { maxPos.x + 1.f, maxPos.y + 1.f }, ImColor(33, 33, 33, borderBgAlpha), 17.f);
        }
    }
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
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
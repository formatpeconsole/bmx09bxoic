#include <bit>

#include "mainWindow.h"

#include "../gui.h"
#include "../item.h"
#include "../../guiItems/items.h"
#include "../../guiItems/utils.h"

#include "../../main/dllInstance.h"

#include "../../config/config.h"

#include "../../cheat-base/math.h"

// what happens
// when you put full path to UI (where item should be located)
// name of item added automatically from real item (not rendered one, rendered one just stores pointer of existing item and changes it's state)
// and for itemPath there is two purposes:
// 1) calculate real item path (where it should rendered in UI, which tab, subtab & child (depends on layout ofc but still))
// 2) calculate LUA item path, which will be used for lua api for asking user get item by this way:
//                                                                                                                              
//      A) Copy lua item path which will have output as:
//          
//          For Example:
//              
//              "Ragebot", "Aimbot", "Main", "Enable"
//                                                                                                                              
//      B) Get real item location by already saved key (which will be generated from input as i showed above)
//          
//          For Example:
//          
//              "Ragebot", "Aimbot", "Main", "Enable" 
// 
//              Will be converted to:
//                  "ragebot-aimbot-main-enable"
// 
//              And then encrypted to hash
//              And when you want to find item ptr by this name in lua api
//              You will scan ONLY item path's HASH, WHICH IS GENERATED at program launch
// 
//              And find this in item's list:
// 
//              Like 
//                  "ragebot-aimbot-main-enable" hash is 0xAAAAAAAFFFFFF 
// 
//              Will be saved in list
//              And when user will put correct path
//              It will generate hashed key of it 
//              And compare to existing one
//              Which will be faster alot than scanning each "Item" from user's input
// 
//
//
#define ITEM_PATH(...) { __VA_ARGS__ }
#define PLACE_CHECKBOX(itemPtr, path) placeItem<CheckBox>(itemPtr, path)
#define PLACE_SLIDER_INT(itemPtr, path) placeItem<Slider<int>>(itemPtr, path)
#define PLACE_SLIDER_FLOAT(itemPtr, path) placeItem<Slider<float>>(itemPtr, path)
#define PLACE_COMBO(itemPtr, path) placeItem<ComboBox>(itemPtr, path)
#define PLACE_MULTICOMBO(itemPtr, path) placeItem<MultiComboBox>(itemPtr, path)
#define PLACE_COLORPICKER(itemPtr, path) placeItem<ColorPicker>(itemPtr, path)

namespace gui::framework
{
void renderConfigsTab()
{
    ImGui::BeginGroup();
    {
     //   combobox::render(getMenuInstance().dpiScale);

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

std::optional<RealItemPath> MainWindow::getRealItemPath(luaItemPath& path)
{
    if (path.empty() || path.size() < 2)
        return {};

    auto& mainTabName = path[0];

    std::optional<std::string> subTabName = std::nullopt;
    if (path.size() > 1)
        subTabName = path[1];

    std::optional<std::string> childName = std::nullopt;
    if (path.size() > 2)
        childName = path[2];

    auto currentTab = std::find_if(tabs.begin(), tabs.end(), [&mainTabName](const tabItself& tab) {
        return tab.name == mainTabName;
    });

    if (currentTab == tabs.end())
        return {};

    RealItemPath result{};
    result.tab = std::distance(tabs.begin(), currentTab);

    auto& tab = tabs[result.tab];
    if (tab.noSubTabs)
    {
        result.subTab = 0;
        result.childCategory = CHILD_CATEGORY_FIRST;
        return result;
    }

    if (!subTabName.has_value())
        return {};

    auto currentSubTab = std::find_if(tab.subTabs.begin(), tab.subTabs.end(), [&subTabName](const subTab& sub) {
        return subTabName.value() == sub.name;
    });

    if (currentSubTab == tab.subTabs.end())
        return {};

    result.subTab = std::distance(tab.subTabs.begin(), currentSubTab);

    if (!childName.has_value())
        return {};

    auto& subTab = tab.subTabs[result.subTab];
    auto currentChild = std::find_if(subTab.childs.begin(), subTab.childs.end(), [&childName](const std::string& child) {
        return childName.value() == child;
    });

    if (currentChild == subTab.childs.end())
        return {};
    
    result.childCategory = std::distance(subTab.childs.begin(), currentChild);
    return result;
}

void MainWindow::renderItem(const baseItemPtr& baseItem, const RealItemPath& currentItemPath)
{
    const auto type = baseItem->getItemType();
    auto realPath = baseItem->getRealItemPath();
    if (!realPath.canRender(currentItemPath))
        return;

    switch (type)
    {
    case ITEM_CHECKBOX:
        checkbox::render(baseItem);
        break;
    case ITEM_SLIDER_INT:
        slider::render<int>(baseItem);
        break;
    case ITEM_SLIDER_FLOAT:
        slider::render<float>(baseItem);
        break;
    case ITEM_COMBOBOX:
        combobox::render(baseItem);
        break;
    case ITEM_MULTICOMBOBOX:
        multicombobox::render(baseItem);
        break;
    case ITEM_COLOR:
        colorpicker::render(baseItem);
        break;
    }
}

void MainWindow::renderChildContents(int selection, int subTabSelection, int childType)
{
    RealItemPath itemPath{ selection, subTabSelection, childType };

    for (auto& i : items)
        renderItem(i, itemPath);
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

bool tabButton(const char* name, const char* formattedName, ImVec2 sizeArg, bool active, tabAnimation& animation, int mainAlpha)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(name);

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
        float uiAlpha = math::toFloat(mainAlpha) / 255.f;

        float colorAdd = (math::toFloat(hovered) + math::toFloat(held)) * 0.15f;
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
        if (animation.radioGlowAlpha > 0.0f)
        {
            ImColor radioColor = animation.radioButton;
            radioColor.Value.w *= animation.radioGlowAlpha;
            objRender::drawCircleShadow(basePos, 3.5f * DPI_SCALE, std::forward<ImColor>(radioColor), 20, 15.f);
        }

        objRender::drawCircleFilled(basePos, 3.5f * DPI_SCALE, ImColor(animation.radioButton));

        auto textPos = pos + ImVec2(21.f, 0.f) * DPI_SCALE;
        objRender::renderText(render::getFont(FONT_ITEMS), textPos, ImColor(animation.text), formattedName);
    }

    return pressed;
}

MainWindow::MainWindow(tabsList tabs, std::string name, ImVec2 size)
    : tabs(tabs), name(name), size(size), tabSelection(0), prevDpiScale(1.f), windowAlpha(0.f)
{
    initItems();
}

MainWindow::~MainWindow()
{
    tabs.clear();
    items.clear();
}

void MainWindow::initItems()
{
    PLACE_COMBO(&getMenuInstance().rage.configSelect, ITEM_PATH( "Ragebot", "Aimbot", "Main" ));
    PLACE_CHECKBOX(&getMenuInstance().rage.enable, ITEM_PATH( "Ragebot", "Aimbot", "Main" ));
    PLACE_CHECKBOX(&getMenuInstance().rage.autoRevolver, ITEM_PATH( "Ragebot", "Aimbot", "Main" ));
    PLACE_CHECKBOX(&getMenuInstance().rage.doubleTap, ITEM_PATH( "Ragebot", "Aimbot", "Main" ));
    PLACE_CHECKBOX(&getMenuInstance().rage.noSpread, ITEM_PATH( "Ragebot", "Aimbot", "Main" ));
    PLACE_CHECKBOX(&getMenuInstance().rage.duckPeekAssist, ITEM_PATH( "Ragebot", "Aimbot", "Main" ));

    for (int i = 0; i < MAX_CONFIGS; ++i)
    {
        auto& itemFromWeaponConfig = getMenuInstance().rage.config[i];

        PLACE_CHECKBOX(&itemFromWeaponConfig.overrideGlobal, ITEM_PATH("Ragebot", "Aimbot", "Hitscan"));
        PLACE_CHECKBOX(&itemFromWeaponConfig.autoFire, ITEM_PATH("Ragebot", "Aimbot", "Hitscan"));
        PLACE_CHECKBOX(&itemFromWeaponConfig.autoScope, ITEM_PATH("Ragebot", "Aimbot", "Hitscan"));
        PLACE_SLIDER_INT(&itemFromWeaponConfig.fov, ITEM_PATH("Ragebot", "Aimbot", "Hitscan"));
        PLACE_MULTICOMBO(&itemFromWeaponConfig.hitBoxes, ITEM_PATH("Ragebot", "Aimbot", "Hitscan"));
        PLACE_MULTICOMBO(&itemFromWeaponConfig.multiPoints, ITEM_PATH("Ragebot", "Aimbot", "Hitscan"));
        PLACE_SLIDER_INT(&itemFromWeaponConfig.pointHeadScale, ITEM_PATH("Ragebot", "Aimbot", "Hitscan"));
        PLACE_SLIDER_INT(&itemFromWeaponConfig.pointBodyScale, ITEM_PATH("Ragebot", "Aimbot", "Hitscan"));
        PLACE_CHECKBOX(&itemFromWeaponConfig.preferBody, ITEM_PATH("Ragebot", "Aimbot", "Hitscan"));
        PLACE_SLIDER_INT(&itemFromWeaponConfig.hitChance, ITEM_PATH("Ragebot", "Aimbot", "Hitscan"));
        PLACE_SLIDER_INT(&itemFromWeaponConfig.minDamage, ITEM_PATH("Ragebot", "Aimbot", "Hitscan"));
        PLACE_MULTICOMBO(&itemFromWeaponConfig.quickStop, ITEM_PATH("Ragebot", "Aimbot", "Hitscan"));
    }

    PLACE_CHECKBOX(&getMenuInstance().rage.antiAim.enable, ITEM_PATH("Ragebot", "Anti-Aim", "Main"));
    PLACE_CHECKBOX(&getMenuInstance().rage.antiAim.atTarget, ITEM_PATH("Ragebot", "Anti-Aim", "Main"));
    PLACE_COMBO(&getMenuInstance().rage.antiAim.pitch, ITEM_PATH("Ragebot", "Anti-Aim", "Main"));
    PLACE_COMBO(&getMenuInstance().rage.antiAim.yaw, ITEM_PATH("Ragebot", "Anti-Aim", "Main"));
    PLACE_COMBO(&getMenuInstance().rage.antiAim.jitter, ITEM_PATH("Ragebot", "Anti-Aim", "Main"));
    PLACE_SLIDER_INT(&getMenuInstance().rage.antiAim.jitterOffset, ITEM_PATH("Ragebot", "Anti-Aim", "Main"));
    PLACE_SLIDER_INT(&getMenuInstance().rage.antiAim.yawOffset, ITEM_PATH("Ragebot", "Anti-Aim", "Main"));
    PLACE_CHECKBOX(&getMenuInstance().rage.antiAim.freestanding, ITEM_PATH("Ragebot", "Anti-Aim", "Main"));
    PLACE_CHECKBOX(&getMenuInstance().rage.antiAim.zeroOnPeek, ITEM_PATH("Ragebot", "Anti-Aim", "Main"));
}

int MainWindow::getMainAlpha()
{
    return math::toInt(255.f * windowAlpha);
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
            auto formattedName = gui::items::getFormattedText(tabName);
            float buttonXSize = math::toFloat(formattedName.length() * 11) * DPI_SCALE;

            if (i == tabSelection)
            {
                tabSelectionAnim.beginPosY = ImGui::GetCursorPosY();

                if (tabSelectionAnim.yPos == 0.f)
                    tabSelectionAnim.yPos = tabSelectionAnim.beginPosY + 3.f * DPI_SCALE;
            }

            if (tabButton(tabName.c_str(), formattedName.c_str(), ImVec2(buttonXSize, buttonYSize), i == tabSelection, tabs[i].tabAnim, mainAlpha))
                tabSelection = i;

            ImGui::Dummy({ 0.f, dummyYSize });
        }

        float triangleButtonCenterPosY = tabSelectionAnim.beginPosY + 3.f * DPI_SCALE;
        tabSelectionAnim.xPos = tabSelectionAnim.base.x * DPI_SCALE;
        tabSelectionAnim.yPos = std::lerp(tabSelectionAnim.yPos, triangleButtonCenterPosY, 0.1f);

        auto pos = ImGui::GetWindowPos() + ImVec2(tabSelectionAnim.xPos, tabSelectionAnim.yPos);

        // tab selection ( RAGE > )
        // shadow
        {
            ImVec2 center = pos + (tabSelectionAnim.first + tabSelectionAnim.second + tabSelectionAnim.third) * (1.f / 3.f) * DPI_SCALE;
            center.x = math::toFloat(math::toInt(center.x));
            center.y = math::toFloat(math::toInt(center.y));

            ImColor newClr = MAIN_WINDOW_ACCENT_COLOR;
            objRender::drawCircleShadow(center, 5.f * DPI_SCALE, std::forward<ImColor>(newClr), 19, 25.f);
        }

        // selection
        {
            ImVec2 first = pos + tabSelectionAnim.first * DPI_SCALE;
            ImVec2 second = pos + tabSelectionAnim.second * DPI_SCALE;
            ImVec2 third = pos + tabSelectionAnim.third * DPI_SCALE;

            ImColor newClr = MAIN_WINDOW_ACCENT_COLOR;
            newClr.Value.w = math::toFloat(mainAlpha) / 255.f;

            objRender::drawTriangleFilled(first, second, third, std::forward<ImColor>(newClr));
        }

        ImGui::PopFont();
    }
    ImGui::EndGroup();
}

void MainWindow::updateTabsAnimation()
{
    auto& selectedTab = tabs[tabSelection];
    std::optional<subTab> selectedSubTab = {};
    if (!selectedTab.noSubTabs)
        selectedSubTab = selectedTab.subTabs[selectedTab.subTabSelection];

    tabContentsAnim.yPosAnimation.setCondition(selectedTab.noSubTabs);
    tabContentsAnim.ySizeAnimation.setCondition(selectedTab.noSubTabs);
    tabContentsAnim.yChildSizeAnimation.setCondition(selectedTab.noSubTabs);

    tabContentsAnim.yPosAnimation.process();
    tabContentsAnim.ySizeAnimation.process();
    tabContentsAnim.yChildSizeAnimation.process();

    tabContentsAnim.yPos = tabContentsAnim.yPosAnimation.getAnimatedValue();
    tabContentsAnim.ySize = tabContentsAnim.ySizeAnimation.getAnimatedValue();
    tabContentsAnim.yChildSize = tabContentsAnim.yChildSizeAnimation.getAnimatedValue();

    {
        tabContentsAnim.selectedTabAnimation.setCondition(tabSelection == oldTabSelection);
        tabContentsAnim.selectedTabAnimation.process();

        oldTabSelection = tabSelection;
    }

    tabContentsAnim.xChildSizeAnimation.setCondition(!selectedSubTab.has_value() || selectedSubTab->childCount <= 1);
    tabContentsAnim.xChildSizeAnimation.process();
    tabContentsAnim.xChildSize = math::toFloat(math::toInt(tabContentsAnim.xChildSizeAnimation.getAnimatedValue()));

    mainPos.baseTabsContents.y = math::toFloat(math::toInt(tabContentsAnim.yPos));
}

void MainWindow::renderTabsContents()
{
    updateTabsAnimation();
 
    auto& selectedTab = tabs[tabSelection];
    std::optional<subTab> selectedSubTab = {};
    if (!selectedTab.noSubTabs)
        selectedSubTab = selectedTab.subTabs[selectedTab.subTabSelection];

    auto subTabsCursorPos = mainPos.baseSubTabsContents * DPI_SCALE;
    ImGui::SetCursorPos(subTabsCursorPos);
    ImGui::BeginGroup();
    {
        ImGui::PushFont(render::getFont(FONT_ITEMS).font);
        
        int mainAlpha = math::toInt(windowAlpha * (tabContentsAnim.selectedTabAnimation.getAnimatedValue() * 0.01f) * 255.f);

        float buttonYSize = 20.f * DPI_SCALE;
        float buttonXSpacing = 29.f * DPI_SCALE;

        const auto size = selectedTab.subTabs.size();

        float basicSpacing = 0.f;
        for (size_t i = 0; i < size; ++i)
        {
            auto tabName = selectedTab.subTabs[i].name;
            auto formattedName = gui::items::getFormattedText(tabName);
            float buttonXSize = math::toFloat(formattedName.length() * 11) * DPI_SCALE;
            if (tabButton(tabName.c_str(), formattedName.c_str(), ImVec2(buttonXSize, buttonYSize),
                i == selectedTab.subTabSelection, selectedTab.subTabs[i].tabAnim, mainAlpha))
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
        auto childSize = ImVec2(484.f, math::toFloat(math::toInt(tabContentsAnim.ySize))) * DPI_SCALE;

        ImGui::BeginChild("MainTabs##bmx09bxoic", childSize, 0, ImGuiWindowFlags_NoBackground);
        {
            auto mainAlpha = getMainAlpha();
            int itemsAlpha = math::toInt(windowAlpha * (tabContentsAnim.selectedTabAnimation.getAnimatedValue() * 0.01f) * 255.f);

            objRender::drawFilledRect(currentPos, childSize, ImColor(18, 18, 18, mainAlpha), 17.f);

            ImVec2 offset = ImVec2(17.f, 17.f) * DPI_SCALE;
            ImVec2 textOffset = ImVec2(19.f, 10.f) * DPI_SCALE;
            ImVec2 itemsOffset = ImVec2(27.f, 39.f) * DPI_SCALE;
            ImGui::SetCursorPos(offset);
            {
                const auto defaultSubChildSizeX = 218.f;
                auto prevCursorPos = ImGui::GetCursorPos();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((defaultSubChildSizeX + 14.f) * DPI_SCALE));
                {
                    auto& secondChildAnimation = tabContentsAnim.subChildsAlphaAnimation[CHILD_SECOND];
                    secondChildAnimation.setCondition(!selectedSubTab.has_value() || selectedSubTab->childCount <= 1);
                    secondChildAnimation.process();
                    {
                        tabContentsAnim.ySubChildFactorAnimation.setCondition(selectedSubTab.has_value() && selectedSubTab->childCount == 3);
                        tabContentsAnim.ySubChildFactorAnimation.process();
                    }

                    float secondChildYMultiplier = tabContentsAnim.ySubChildFactorAnimation.getAnimatedValue() * 0.01f;

                    auto& thirdChildAnimation = tabContentsAnim.subChildsAlphaAnimation[CHILD_THIRD];
                    thirdChildAnimation.setCondition(!selectedSubTab.has_value() || selectedSubTab->childCount < 3);
                    thirdChildAnimation.process();

                    auto prevCursorPos = ImGui::GetCursorPos();
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (tabContentsAnim.yChildSize * 0.53f) * DPI_SCALE);
                    {
                        int thirdChildAlpha = math::toInt(math::toFloat(mainAlpha * thirdChildAnimation.getAnimatedValue() * 0.01f));
                        if (thirdChildAlpha > 0)
                        {
                            ImVec2 groupBoxSize = ImVec2(218.f, math::toFloat(math::toInt(tabContentsAnim.yChildSize * 0.47f))) * DPI_SCALE;
                            ImGui::BeginChild("Third##bmx09bxoic", groupBoxSize, 0, ImGuiWindowFlags_NoBackground);
                            {
                                auto currentGroupBoxPos = ImGui::GetWindowPos() + ImGui::GetCursorPos();
                                objRender::drawFilledRect(currentGroupBoxPos, groupBoxSize, ImColor(33, 33, 33, thirdChildAlpha), 17.f);

                                std::string childName = selectedSubTab.has_value() && selectedSubTab->childCount > 2 ? gui::items::getFormattedText(selectedSubTab->childs[2]) : "";
                                objRender::renderText(render::getFont(FONT_ITEMS), currentGroupBoxPos + textOffset, ImColor(174, 174, 174, thirdChildAlpha), childName.c_str());

                                ImGui::SetCursorPos(itemsOffset);
                                ImGui::BeginGroup();
                                {
                                    ImGui::PushFont(render::getFont(FONT_ITEMS).font);
                                    {
                                        renderChildContents(tabSelection, selectedTab.subTabSelection, CHILD_CATEGORY_THIRD);
                                    }
                                    ImGui::PopFont();
                                }
                                ImGui::EndGroup();
                            }
                            ImGui::EndChild();
                        }
                    }

                    ImGui::SetCursorPos(prevCursorPos);
                    {
                        int secondChildAlpha = math::toInt(math::toFloat(mainAlpha * secondChildAnimation.getAnimatedValue() * 0.01f));
                        if (secondChildAlpha > 0)
                        {
                            ImVec2 groupBoxSize = ImVec2(218.f, math::toFloat(math::toInt(tabContentsAnim.yChildSize * secondChildYMultiplier))) * DPI_SCALE;
                            ImGui::BeginChild("Second##bmx09bxoic", groupBoxSize, 0, ImGuiWindowFlags_NoBackground);
                            {
                                auto currentGroupBoxPos = ImGui::GetWindowPos() + ImGui::GetCursorPos();
                                objRender::drawFilledRect(currentGroupBoxPos, groupBoxSize, ImColor(33, 33, 33, secondChildAlpha), 17.f);

                                std::string childName = selectedSubTab.has_value() && selectedSubTab->childCount > 1 ? gui::items::getFormattedText(selectedSubTab->childs[1]) : "";
                                objRender::renderText(render::getFont(FONT_ITEMS), currentGroupBoxPos + textOffset, ImColor(174, 174, 174, secondChildAlpha), childName.c_str());

                                ImGui::SetCursorPos(itemsOffset);
                                ImGui::BeginGroup();
                                {
                                    ImGui::PushFont(render::getFont(FONT_ITEMS).font);
                                    {
                                        renderChildContents(tabSelection, selectedTab.subTabSelection, CHILD_CATEGORY_SECOND);
                                    }
                                    ImGui::PopFont();
                                }
                                ImGui::EndGroup();
                            }
                            ImGui::EndChild();
                        }
                    }
                }

                {
                    ImVec2 groupBoxSize = ImVec2(tabContentsAnim.xChildSize, math::toFloat(math::toInt(tabContentsAnim.yChildSize))) * DPI_SCALE;
                    ImGui::SetCursorPos(prevCursorPos);
                    ImGui::BeginChild("First##bmx09bxoic", groupBoxSize, 0, ImGuiWindowFlags_NoBackground);
                    {
                        auto currentGroupBoxPos = ImGui::GetWindowPos() + ImGui::GetCursorPos();
                        objRender::drawFilledRect(currentGroupBoxPos, groupBoxSize, ImColor(33, 33, 33, mainAlpha), 17.f);

                        std::string childName = selectedSubTab.has_value() ? gui::items::getFormattedText(selectedSubTab->childs[0]) : "Main";
                        objRender::renderText(render::getFont(FONT_ITEMS), currentGroupBoxPos + textOffset, ImColor(174, 174, 174, mainAlpha), childName.c_str());

                        ImGui::SetCursorPos(itemsOffset);
                        ImGui::BeginGroup();
                        {
                            ImGui::PushFont(render::getFont(FONT_ITEMS).font);
                            {
                                renderChildContents(tabSelection, selectedTab.subTabSelection, CHILD_CATEGORY_FIRST);
                            }
                            ImGui::PopFont();
                        }
                        ImGui::EndGroup();
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

    windowAlpha = uiOpenAnimation.getAnimatedValue() * 0.01f;
    if (windowAlpha <= 0.f)
        return;

    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowBgAlpha(windowAlpha);

    std::string uiName = name + "##bmx09bxoic";

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

        // background
        {
            objRender::drawFilledRect(pos, size, ImColor(33, 33, 33, getMainAlpha()), 17.f);

            // main ui (name, items, info)
            renderWindowContents();
        }

        // border
        {
            int borderBgAlpha = math::toInt(7.f * windowAlpha);
            ImVec2 borderOffset = ImVec2(1.f, 1.f);
            ImVec2 shadowBorderOffset = ImVec2(2.f, 2.f);

            const int maxShadowRange = math::toInt(10.f * DPI_SCALE);
            ImColor shadowColor = ImColor(0, 0, 0, getMainAlpha());
            objRender::drawRectShadow(pos - shadowBorderOffset, size + shadowBorderOffset * 2, std::forward<ImColor>(shadowColor), maxShadowRange, 45.f, 17.f);
            objRender::drawRect(pos - borderOffset, size + borderOffset, ImColor(33, 33, 33, borderBgAlpha), 17.f);
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
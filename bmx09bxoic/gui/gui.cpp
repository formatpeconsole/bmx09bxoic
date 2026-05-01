#include <iostream>
#include <string>
#include "gui.h"
#include "framework/window.h"

#include "../main/dllInstance.h"

#include "../guiItems/items.h"
#include "../config/config.h"

namespace gui
{
enum DPI_SCALE_AMOUNT
{
    DPI_100 = 0,
    DPI_125,
    DPI_150,
    DPI_200
};

void init()
{
    getMenuInstance().initConfig();
    binds::initBinds();
}

void destroy()
{
    binds::destroyBinds();
}

void* getItemValuePointerFromItemPointer(void* ptr, int type)
{
    switch (type)
    {
    case ITEM_CHECKBOX:
        return &reinterpret_cast<Item<bool>*>(ptr)->value;
    case ITEM_SLIDER_INT:
        return &reinterpret_cast<Item<int>*>(ptr)->value;
    case ITEM_SLIDER_FLOAT:
        return &reinterpret_cast<Item<float>*>(ptr)->value;
    case ITEM_COMBOBOX:
        return &reinterpret_cast<Item<int>*>(ptr)->value;
    case ITEM_MULTICOMBOBOX:
        return &reinterpret_cast<Item<unsigned int>*>(ptr)->value;
    case ITEM_COLOR:
        return &reinterpret_cast<Item<unsigned int>*>(ptr)->value;
    default:
        return {};
    }
}

std::optional<std::vector<std::string>> getItemList(void* ptr, int type)
{
    switch (type)
    {
    case ITEM_COMBOBOX:
    {
        auto item = reinterpret_cast<Item<int>*>(ptr);
        return item->itemsList;
    }
    break;
    case ITEM_MULTICOMBOBOX:
    {
        auto item = reinterpret_cast<Item<unsigned int>*>(ptr);
        return item->itemsList;
    }
    break;
    default:
        return {};
    }
}

// in KeyBindManager i had to save value in string format for UI purposes
// now you have to get this value and visualize in ur UI however you want
std::string getBindValueFromString(const std::string& value, void* ptr, int itemType)
{
    switch (itemType)
    {
    case ITEM_CHECKBOX:
        return value == "1" ? "On" : "Off";
    case ITEM_SLIDER_FLOAT:
        return value + "f";
    case ITEM_SLIDER_INT:
        return value;
    case ITEM_COMBOBOX:
    {
        auto list = getItemList(ptr, itemType);
        if (list.has_value())
        {
            int index = std::stoi(value);
            if (index < 0 || index >= list->size())
                return "Invalid";

            return list->at(index);
        }
        return {};
    }
    break;
    case ITEM_MULTICOMBOBOX:
    {
        auto list = getItemList(ptr, itemType);
        if (list.has_value())
        {
            unsigned int index = std::stoul(value);
            if (index == 0)
                return "Invalid";

            return gui::multicombobox::getActiveItems(index, list.value());
        }
        return {};
    }
    break;
    case ITEM_COLOR:
    {
        uint32_t color = std::stoul(value);

        auto r = (color >> 16) & 0xFF;
        auto g = (color >> 8) & 0xFF;
        auto b = color & 0xFF;
        auto a = (color >> 24) & 0xFF;

        // R: 255 G: 255 B: 255: A: 255
        return "R: "
            + std::to_string(r)
            + " G: "
            + std::to_string(g)
            + " B: "
            + std::to_string(b)
            + " A: "
            + std::to_string(a);
    }
    break;
    default:
        return value;
    }
}

void saveUiPos(const std::string& name)
{
    auto window = ImGui::FindWindowByName(name.c_str());
    if (window != nullptr)
        getMenuInstance().lastPositions.emplace_back(std::make_pair(name, window->Pos));
}

void correctSavedUIPos(const std::string& name)
{
    auto existingUI = std::find_if(getMenuInstance().lastPositions.begin(), getMenuInstance().lastPositions.end(),
        [name](const windowNamePosValue& item)
        {
            return item.has_value() && item->first == name;
        });

    if (existingUI != getMenuInstance().lastPositions.end())
    {
        ImGui::SetNextWindowPos(existingUI->value().second);
        getMenuInstance().lastPositions.erase(existingUI);
    }
}

void updateDpiScale()
{
    static float prevDpi = 1.f;

    switch (getMenuInstance().dpiScale.item.value)
    {
    case DPI_100:
        framework::DPI_SCALE = 1.f;
        break;
    case DPI_125:
        framework::DPI_SCALE = 1.25f;
        break;
    case DPI_150:
        framework::DPI_SCALE = 1.5f;
        break;
    case DPI_200:
        framework::DPI_SCALE = 2.f;
        break;
    }

    if (prevDpi != framework::DPI_SCALE)
    {
        std::string mainWindowName = "BMX09BXOIC - MAIN";
        std::string bindWindowName = "BIND LIST";

        saveUiPos(mainWindowName);
        saveUiPos(bindWindowName);

        render::getRenderInfoInstance().init = false;
        render::destroy();
        render::clearRenderTargetView();
        prevDpi = framework::DPI_SCALE;
    }
}

void renderBindsDebugWindow()
{
    std::string windowTitle = "BIND LIST";
    correctSavedUIPos(windowTitle);

    ImGui::Begin(windowTitle.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
    {
        auto& binds = getMenuInstance().keyBindManager.getBindList();
        auto& itemsInMemory = getMenuInstance().itemsInMemory;

        static std::unordered_map<void*, std::pair<void*, int>> items{};
        for (auto& i : itemsInMemory)
            items[getItemValuePointerFromItemPointer(i.first, i.second)] = std::make_pair(i.first, i.second);

        bool noBinds = true;
        for (auto it = binds.rbegin(); it != binds.rend(); ++it)
        {
            auto item = (*it);
            if (item->getItemType() == ITEM_UI_OPEN)
                continue;

            auto itemThatBindUse = items.find(item->getItemPtr());
            if (itemThatBindUse == items.end())
                continue;

            bool keyPressed = item->getType() == BIND_RELEASE ? !item->getPressed() : item->getPressed();
            if (keyPressed)
            {
                noBinds = false;

                // bind format would be:
                // [+] ItemName -> Value (BindMode)
                std::string bindValue = getBindValueFromString(item->getBindValue(), itemThatBindUse->second.first, itemThatBindUse->second.second);
                std::string infoBegin = item->getItemName();
                std::string infoEnd = " " + bindValue + " (" + gui::binds::getBindMode(item->getType()) + ")";

                ImGui::Text("%s%s", getFormattedText(infoBegin).c_str(), infoEnd.c_str());
            }
        }

        if (noBinds)
            ImGui::Text("No active binds found.");
    }
    ImGui::End();
}

enum UI_TABS
{
    TAB_RAGE = 0,
    TAB_LEGIT,
    TAB_VISUALS,
    TAB_MISC,
    TAB_SKINS,
    TAB_CONFIGS,
    TAB_LUA
};

int tabSelection = 0;
std::vector<std::string> tabs = { "Rage", "Legit", "Visuals", "Misc", "Skins", "Configs", "LUA" };

// TO-DO:
// subtabs (made as child) +
// MAIN (with weapon config) + 
// ANTI-AIM (stand/move/air/crouch/slowwalk) +
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
    ImVec2 weaponitemsChildSize = ImVec2(374, 440);
    ImVec2 mainChildSize = ImVec2(weaponitemsChildSize.x, weaponitemsChildSize.y * 0.272f);
    ImVec2 antiAimChildSize = ImVec2(weaponitemsChildSize.x, weaponitemsChildSize.y * 0.95f);
    ImVec2 configChildSize = ImVec2(weaponitemsChildSize.x, 60);

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

// TO-DO: different layout that allows to handle multiple sections at the same time
// for example:
// RAGE MAIN:
// [ Main child with enable / exploits ] + 
// [ Weapon config ] + 
// ANTI AIM MAIN:
// [ Main Child with pitch yaw (offset) jitter (offset) at target ] +
// [ Misc Child with freestanding edge ] +
void renderMainUI()
{
    if (getMenuInstance().opened)
    {
        std::string windowTitle = "BMX09BXOIC - MAIN";
        correctSavedUIPos(windowTitle);

        ImGui::SetNextWindowSize(ImVec2(780, 650));

        render::pushFont(FONT_LOGO);
        ImGui::Begin(windowTitle.c_str(), &getMenuInstance().opened, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
        render::popFont();

        {
            using namespace gui::items;
            render::pushFont(FONT_ITEMS);
            ImGui::BeginGroup();
            for (int i = 0; i < tabs.size(); ++i)
            {
                if (ImGui::Button(tabs[i].c_str(), ImVec2(tabs[i].length() * 10, 18)))
                    tabSelection = i;

                ImGui::SameLine();
            }
            ImGui::EndGroup();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::BeginGroup();
            {
                switch (tabSelection)
                {
                case TAB_RAGE:
                    renderRageTab();
                    break;
                case TAB_LEGIT:
                    renderLegitTab();
                    break;
                case TAB_VISUALS:
                    renderVisualsTab();
                    break;
                case TAB_MISC:
                    renderMiscTab();
                    break;
                case TAB_SKINS:
                    renderSkinsTab();
                    break;
                case TAB_CONFIGS:
                    renderConfigsTab();
                    break;
                case TAB_LUA:
                    renderLUATab();
                    break;
                }
            }
            ImGui::EndGroup();
            render::popFont();
        }
        ImGui::End();
    }
}

Menu& getMenuInstance()
{
    static Menu instance;
    return instance;
}

void Menu::initConfig()
{
    for (int i = 0; i < MAX_CONFIGS; ++i)
    {
        auto index = std::to_string(i);
        rage.config[i].autoFire = MAKE_CHECKBOX_RT("Auto Fire##rage" + index, false);
        rage.config[i].autoScope = MAKE_CHECKBOX_RT("Auto Scope##rage" + index, false);
        rage.config[i].fov = MAKE_SLIDER_RT("Field of View##rage" + index, 180, 0, 180);
        rage.config[i].hitBoxes = MAKE_MULTICOMBO_RT("Hit Boxes##rage" + index, 0, COMBO_LIST("Head", "Neck", "Chest", "Pelvis", "Stomach", "Arms", "Legs", "Feet"));
        rage.config[i].multiPoints = MAKE_MULTICOMBO_RT("Multi Points##rage" + index, 0, COMBO_LIST("Head", "Neck", "Chest", "Pelvis", "Stomach", "Arms", "Legs", "Feet"));
        rage.config[i].pointHeadScale = MAKE_SLIDER_RT("Head Scale##rage" + index, 0, 0, 100);
        rage.config[i].pointBodyScale = MAKE_SLIDER_RT("Body Scale##rage" + index, 0, 0, 100);
        rage.config[i].hitChance = MAKE_SLIDER_RT("Hit Chance##rage" + index, 0, 0, 100);
        rage.config[i].minDamage = MAKE_SLIDER_RT("Min Damage##rage" + index, 0, 0, 100);
        rage.config[i].preferBody = MAKE_CHECKBOX_RT("Prefer Body##rage" + index, false);
        rage.config[i].quickStop = MAKE_MULTICOMBO_RT("Quick Stop##rage" + index, 0, COMBO_LIST("Early", "Between Shots", "In Air", "Ignore Molotov"));
        rage.config[i].overrideGlobal = MAKE_CHECKBOX_RT("Override Global##rage" + index, false);

        itemsInMemory.emplace_back(ITEM_PTR_RT(rage.config[i].autoFire));
        itemsInMemory.emplace_back(ITEM_PTR_RT(rage.config[i].autoScope));
        itemsInMemory.emplace_back(ITEM_PTR_RT(rage.config[i].fov));
        itemsInMemory.emplace_back(ITEM_PTR_RT(rage.config[i].multiPoints));
        itemsInMemory.emplace_back(ITEM_PTR_RT(rage.config[i].pointHeadScale));
        itemsInMemory.emplace_back(ITEM_PTR_RT(rage.config[i].pointBodyScale));
        itemsInMemory.emplace_back(ITEM_PTR_RT(rage.config[i].hitChance));
        itemsInMemory.emplace_back(ITEM_PTR_RT(rage.config[i].minDamage));
        itemsInMemory.emplace_back(ITEM_PTR_RT(rage.config[i].preferBody));
        itemsInMemory.emplace_back(ITEM_PTR_RT(rage.config[i].quickStop));
        itemsInMemory.emplace_back(ITEM_PTR_RT(rage.config[i].overrideGlobal));
    }

    itemsInMemory.emplace_back(ITEM_PTR_RT(rage.enable));
    itemsInMemory.emplace_back(ITEM_PTR_RT(rage.autoRevolver));
    itemsInMemory.emplace_back(ITEM_PTR_RT(rage.doubleTap));
    itemsInMemory.emplace_back(ITEM_PTR_RT(rage.noSpread));
    itemsInMemory.emplace_back(ITEM_PTR_RT(rage.duckPeekAssist));
    itemsInMemory.emplace_back(ITEM_PTR_RT(rage.configSelect));
}
}
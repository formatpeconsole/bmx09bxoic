#include <iostream>
#include <string>
#include "gui.h"
#include "framework/window.h"
#include "framework/bindsWindow.h"
#include "framework/mainWindow.h"

#include "../main/dllInstance.h"

#include "../guiItems/items.h"
#include "../config/config.h"

namespace gui
{
using namespace framework;

enum DPI_SCALE_AMOUNT
{
    DPI_75 = 0,
    DPI_100,
    DPI_125,
    DPI_150,
    DPI_200
};

void init()
{
    getMenuInstance().initConfig();
    getMenuInstance().addItemsToBindList();
    getMenuInstance().initWindows();
    binds::initBinds();
}

void destroy()
{
    binds::destroyBinds();
    getMenuInstance().windows.clear();
}

ImVec2 getClampedUiPosition(windowPosSizeAndScale posSizeAndScale)
{
    auto pos = std::get<0>(posSizeAndScale);
    auto origSize = std::get<1>(posSizeAndScale);
    auto scale = std::get<2>(posSizeAndScale);

    float scaleStep = framework::DPI_SCALE / scale;
    auto newSize = std::get<1>(posSizeAndScale) * scaleStep;

    auto& screenSize = render::getRenderInfoInstance().screenSize;
  
    auto windowEdgeX = pos.x + origSize.x;
    auto windowEdgeY = pos.y + origSize.y;

    // ui got out from screen
    if (pos.y < 0.f)
        pos.y = 10.f;

    if (pos.x < 0.f)
        pos.x = 10.f;

    // ui edge got out fom screen
    // if X edge of window is too far
    if (windowEdgeX > screenSize.x)
        pos.x = screenSize.x - newSize.x - 10.f;

    // if Y edge of window is too far
    if (windowEdgeY > screenSize.y)
        pos.y = screenSize.y - newSize.y - 10.f;

    return pos;
}

void WindowsManager::saveUiPos(const std::string& name, float dpiScale)
{
    auto& windows = getMenuInstance().windows;
    auto existingWindow = std::find_if(windows.begin(), windows.end(), [name](const std::shared_ptr<IWindow>& window) {
        return window->getName() == name;
    });

    if (existingWindow != windows.end())
        savedUiInfo.emplace_back(std::make_pair(name, 
            std::make_tuple(
                (*existingWindow)->getPos(),
                (*existingWindow)->getWindowSize(),
                dpiScale
            )));
}

void WindowsManager::correctSavedUIPos(const std::string& name)
{
    if (savedUiInfo.empty())
        return;

    auto existingUI = std::find_if(savedUiInfo.begin(), savedUiInfo.end(),
        [name](const windowInfoValue& item)
        {
            return item.has_value() && item->first == name;
        });

    if (existingUI != savedUiInfo.end())
    {
        auto clampedPosition = getClampedUiPosition(existingUI->value().second);
        ImGui::SetNextWindowPos(clampedPosition);
        savedUiInfo.erase(existingUI);
    }
}

void WindowsManager::backupAllUiPositions(float dpiScale)
{
    auto& windows = getMenuInstance().windows;
    for (auto window : windows)
    {
        window->init();
        saveUiPos(window->getName(), dpiScale);
    }
}

void WindowsManager::updateDpiScale()
{
    static float prevDpi = 1.f;

    switch (getMenuInstance().dpiScale.item.value)
    {
    case DPI_75:
        framework::DPI_SCALE = 0.75f;
        break;
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
        backupAllUiPositions(prevDpi);

        render::getRenderInfoInstance().init = false;
        render::destroy();
        render::clearRenderTargetView();
        prevDpi = framework::DPI_SCALE;
    }
}

void Menu::initWindows()
{
    using namespace framework;
    windows.emplace_back(std::make_shared<BindsWindow>("BIND LIST", BINDS_WINDOW_SIZE));
    windows.emplace_back(std::make_shared<MainWindow>(tabsList{
        tabItself{
            {
                subTab{"Aimbot", subTabChilds{"Main", "Hitscan"}},
                subTab{"Anti-Aim", subTabChilds{"Main", "Movement"}}
            },
            "Ragebot", 
            false
        },
        tabItself{{},
            "Legitbot",
            true
        },
        tabItself{
            {
                subTab{"Enemy", subTabChilds{"ESP", "Chams"}},
                subTab{"Team", subTabChilds{"ESP", "Chams"}},
                subTab{"Local", subTabChilds{"ESP", "Chams"}},
                subTab{"Weapons", subTabChilds{"ESP", "Chams"}},
                subTab{"Grenades", subTabChilds{"ESP", "Prediction"}}
            },
            "Visuals",
            false
        },
        tabItself{ 
            {
                subTab{"Movement", subTabChilds{"Main"}},
                subTab{"World", subTabChilds{"View", "Ambience", "Effects"}},
            },
            "Miscellaneous",
            false
        },
        tabItself{
            {
                subTab{"Weapons", subTabChilds{"Knife", "Weapon"}},
                subTab{"Gloves", subTabChilds{"CT##gloves", "T##gloves"}},
                subTab{"Agents", subTabChilds{"CT##agents", "T##agents"}}
            },
            "Skin-changer",
            false
        },
        tabItself{{},
            "Configurations",
            true
        },
        tabItself{{}, 
            "LUA-scripts",
            true
        },
    }, "BMX09BXOIC", MAIN_WINDOW_SIZE));

    for (auto window : windows)
        window->init();
}

void Menu::renderWindows()
{
    for (auto window : windows)
    {
        window->render();
        window->updateWindowPosOrSize();
    }
}

void Menu::destroyWindows()
{
    windows.clear();
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
    }
}

void Menu::addItemsToBindList()
{
    for (int i = 0; i < MAX_CONFIGS; ++i)
    {
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

void Menu::processLuaItem(luaItem& item)
{
    switch (item.itemType)
    {
    case ITEM_CHECKBOX:
        itemsInMemory.emplace_back(ITEM_PTR_RT(std::any_cast<CheckBox&>(item.item)));
        break;
    case ITEM_SLIDER_INT:
        itemsInMemory.emplace_back(ITEM_PTR_RT(std::any_cast<Slider<int>&>(item.item)));
        break;
    case ITEM_SLIDER_FLOAT:
        itemsInMemory.emplace_back(ITEM_PTR_RT(std::any_cast<Slider<float>&>(item.item)));
        break;
    case ITEM_COMBOBOX:
        itemsInMemory.emplace_back(ITEM_PTR_RT(std::any_cast<ComboBox&>(item.item)));
        break;
    case ITEM_MULTICOMBOBOX:
        itemsInMemory.emplace_back(ITEM_PTR_RT(std::any_cast<MultiComboBox&>(item.item)));
        break;
    case ITEM_COLOR:
        itemsInMemory.emplace_back(ITEM_PTR_RT(std::any_cast<ColorPicker&>(item.item)));
        break;
    }
}

void Menu::addLuaItemsToBindList()
{
    for (auto& item : luaItems)
        processLuaItem(item);
}

void Menu::addLatestLuaItemToBindList()
{
    auto& latestItem = luaItems.back();
    processLuaItem(latestItem);
}

}
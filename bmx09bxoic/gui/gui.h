#pragma once

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <algorithm>
#include <unordered_map>
#include <any>
#include <queue>

#include "../render/render.h"
#include "binds/binds.h"
#include "framework/window.h"
#include "framework/items.h"
#include "item.h"

#undef min
#undef max

#define COMBO_LIST(...) { __VA_ARGS__ }
#define PUT_ITEM_DATA(itemName, defaultValue, type, list) \
    .item = {\
        .oldValue = {}, \
        .value = defaultValue, \
        .binds = {}, \
        .preview = {}, \
        .name = itemName, \
        .itemType = type, \
        .itemsList = list, \
    }\

#define MAKE_CHECKBOX(name, itemName, defaultValue) CheckBox name{ PUT_ITEM_DATA(itemName, defaultValue, ITEM_CHECKBOX, {}) }
#define MAKE_CHECKBOX_RT(itemName, defaultValue) CheckBox { PUT_ITEM_DATA(itemName, defaultValue, ITEM_CHECKBOX, {}) }

#define MAKE_SLIDER(name, itemName, defaultValue, valueMin, valueMax) Slider<int> name{ PUT_ITEM_DATA(itemName, defaultValue, ITEM_SLIDER_INT, {}), .min = valueMin, .max = valueMax }
#define MAKE_SLIDER_RT(itemName, defaultValue, valueMin, valueMax) Slider<int> { PUT_ITEM_DATA(itemName, defaultValue, ITEM_SLIDER_INT, {}), .min = valueMin, .max = valueMax }

#define MAKE_COMBO(name, itemName, defaultValue, list) ComboBox name{ PUT_ITEM_DATA(itemName, defaultValue, ITEM_COMBOBOX, list) }
#define MAKE_COMBO_RT(itemName, defaultValue, list) ComboBox { PUT_ITEM_DATA(itemName, defaultValue, ITEM_COMBOBOX, list) }

#define MAKE_MULTICOMBO(name, itemName, defaultValue, list) MultiComboBox name{ PUT_ITEM_DATA(itemName, defaultValue, ITEM_MULTICOMBOBOX, list) }
#define MAKE_MULTICOMBO_RT(itemName, defaultValue, list) MultiComboBox { PUT_ITEM_DATA(itemName, defaultValue, ITEM_MULTICOMBOBOX, list) }

#define ITEM_PTR(name) { reinterpret_cast<uintptr_t>(&name.item), name.item.itemType }
#define ITEM_PTR_RT(name) std::make_pair(reinterpret_cast<uintptr_t>(&name.item), name.item.itemType)

// global
// scar20
// g3sg1
// ssg08
// awp
// deagle
// revolver
// pistols
// rifles
constexpr int MAX_CONFIGS = 9;

namespace gui
{
using namespace binds;
using namespace items;
using namespace framework;

struct RageTab
{
    struct WeaponConfig
    {
        CheckBox autoFire{};
        CheckBox autoScope{};
        Slider<int> fov{};
        MultiComboBox hitBoxes{};
        MultiComboBox multiPoints{};
        Slider<int> pointHeadScale{};
        Slider<int> pointBodyScale{};
        Slider<int> hitChance{};
        Slider<int> minDamage{};
        CheckBox preferBody{};
        MultiComboBox quickStop{};
        CheckBox overrideGlobal{};
    };

    struct AntiAim
    {
        MAKE_CHECKBOX(enable, "Enable##anti-aim", false);
        MAKE_CHECKBOX(atTarget, "At Target##anti-aim", false);

        MAKE_COMBO(pitch, "Pitch##anti-aim", 0, COMBO_LIST(
            "Off",
            "Down",
            "Up",
            "Zero"
        ));

        MAKE_COMBO(yaw, "Yaw##anti-aim", 0, COMBO_LIST(
            "Off",
            "Backward",
            "Spin"
        ));

        MAKE_COMBO(jitter, "Jitter##anti-aim", 0, COMBO_LIST(
            "Off",
            "Center",
            "Offset",
            "3-way",
            "Spin"
        ));

        MAKE_SLIDER(jitterOffset, "Jitter offset##anti-aim", 0, -180, 180);
        MAKE_SLIDER(yawOffset, "Yaw offset##anti-aim", 0, -180, 180);

        MAKE_CHECKBOX(freestanding, "Freestanding##anti-aim", false);
        MAKE_CHECKBOX(zeroOnPeek, "Pitch Zero on Peek##anti-aim", false);
    };

    MAKE_CHECKBOX(enable, "Enable##rage", true);
    MAKE_CHECKBOX(autoRevolver, "Auto Revolver##rage", false);
    MAKE_CHECKBOX(doubleTap, "Double Tap##rage", false);
    MAKE_CHECKBOX(noSpread, "No Spread##rage", false);
    MAKE_CHECKBOX(duckPeekAssist, "Duck peek assist##rage", false);

    MAKE_COMBO(configSelect, "Weapon Config##rage", 0, COMBO_LIST(
        "Global",
        "SCAR-20", 
        "G3SG1", 
        "SSG-08", 
        "AWP",
        "Desert Eagle", 
        "Revolver", 
        "Pistols", 
        "Rifles" 
    ));

    WeaponConfig config[MAX_CONFIGS]{};
    AntiAim antiAim{};
};

using itemsInMemoryList = std::list<std::pair<uintptr_t, int>>;

using windowPosSizeAndScale = std::tuple<ImVec2, ImVec2, float>;
using windowInfo = std::pair<std::string, windowPosSizeAndScale>;
using windowInfoValue = std::optional<windowInfo>;
using savedUiInfoList = std::list<windowInfoValue>;

using windowsList = std::list<std::shared_ptr<framework::IWindow>>;

struct WindowsManager
{
    savedUiInfoList savedUiInfo{};

    void saveUiPos(const std::string& name, float dpiScale);
    void correctSavedUIPos(const std::string& name);
    void backupAllUiPositions(float dpiScale);
    void updateDpiScale();
};

struct Menu
{
    ItemOldValue<bool> openedOldValue{};
    bool opened = false;
    bool newOpened = true;

    windowsList windows{};

    MAKE_COMBO(dpiScale, "DPI Scale##ui", 1, COMBO_LIST(
        "75%",
        "100%",
        "125%",
        "150%",
        "200%"
    ));

    RageTab rage{};
    itemsInMemoryList itemsInMemory{};
    KeyBindManager keyBindManager{};
    WindowsManager windowsManager{};

    void initConfig();
    void initWindows();
    void renderWindows();
    void destroyWindows();
};

extern void init();
extern void updateDpiScale();
extern void destroy();

extern void renderUI();

extern Menu& getMenuInstance();
}
#pragma once

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <algorithm>
#include <unordered_map>
#include <any>

#include "../render/render.h"
#include "binds/binds.h"
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

#define ITEM_PTR(name) { reinterpret_cast<void*>(&name.item), name.item.itemType }
#define ITEM_PTR_RT(name) std::make_pair(reinterpret_cast<void*>(&name.item), name.item.itemType)

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

struct RageTab
{
    // TO-DO: Auto Fire, Quick Scope, FOV, Silent Aim
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
};

using itemsInMemoryList = std::vector<std::pair<void*, int>>;
struct Menu
{
    ItemOldValue<bool> openedOldValue{};
    bool opened = false;
    bool newOpened = true;

    RageTab rage;
    itemsInMemoryList itemsInMemory{};
    KeyBindManager keyBindManager{};

    void initConfig();
};

extern void init();
extern void destroy();

extern void renderBindsDebugWindow();
extern void renderMainUI();

extern Menu& getMenuInstance();
}
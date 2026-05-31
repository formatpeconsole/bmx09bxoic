#include "itemsManager.h"
#include "../gui.h"
#include "../../lua/luaManager.h"
#include "../../mem/string.h"

namespace gui::framework
{
using namespace lua;

#define GET_ITEM_POINTERS(type) \
    { \
        auto& currentItem = std::any_cast<type&>(item.item); \
        itemPointer = reinterpret_cast<uintptr_t>(&currentItem); \
        itemValuePointer = reinterpret_cast<uintptr_t>(&currentItem.item.value); \
    } \

ItemsManager::ItemsManager() 
{
    tabs = std::move(mainWindowTabsList{
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
        } 
    });
}

ItemsManager::~ItemsManager() 
{ 
    tabs.clear(); 
    items.clear(); 
}

mainWindowTabsList& ItemsManager::getMainWindowTabsList()
{
    return tabs;
}

mainWindowItemsList& ItemsManager::getMainWindowItemsList()
{
    return items;
}

std::optional<RealItemPath> ItemsManager::getRealItemPath(itemPath& path)
{
    if (path.empty() || path.size() < 2)
        return {};

    auto mainTabName = path[0];
    std::transform(mainTabName.begin(), mainTabName.end(), mainTabName.begin(), TOLOWER_STRING);

    std::optional<std::string> subTabName = std::nullopt;
    if (path.size() > 1)
    {
        subTabName = path[1];
        std::transform(subTabName->begin(), subTabName->end(), subTabName->begin(), TOLOWER_STRING);
    }

    std::optional<std::string> childName = std::nullopt;
    if (path.size() > 2)
    {
        childName = path[2];
        std::transform(childName->begin(), childName->end(), childName->begin(), TOLOWER_STRING);
    }

    auto currentTab = std::find_if(tabs.begin(), tabs.end(), [&mainTabName](tabItself tab) {
        auto tabNameCaseInsensitive = tab.name;
        std::transform(tabNameCaseInsensitive.begin(), tabNameCaseInsensitive.end(), tabNameCaseInsensitive.begin(), TOLOWER_STRING);
        return tabNameCaseInsensitive == mainTabName;
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

    auto currentSubTab = std::find_if(tab.subTabs.begin(), tab.subTabs.end(), [&subTabName](subTab sub) {
        auto subTabCaseInsensitive = sub.name;;
        std::transform(subTabCaseInsensitive.begin(), subTabCaseInsensitive.end(), subTabCaseInsensitive.begin(), TOLOWER_STRING);
        return subTabName.value() == subTabCaseInsensitive;
    });

    if (currentSubTab == tab.subTabs.end())
        return {};

    result.subTab = std::distance(tab.subTabs.begin(), currentSubTab);

    if (!childName.has_value())
        return {};

    auto& subTab = tab.subTabs[result.subTab];
    auto currentChild = std::find_if(subTab.childs.begin(), subTab.childs.end(), [&childName](std::string child) {
        auto childCaseInsensitive = child;;
        std::transform(childCaseInsensitive.begin(), childCaseInsensitive.end(), childCaseInsensitive.begin(), TOLOWER_STRING);
        return childName.value() == childCaseInsensitive;
    });

    if (currentChild == subTab.childs.end())
        return {};

    result.childCategory = std::distance(subTab.childs.begin(), currentChild);
    return result;
}

std::optional<std::pair<uintptr_t, int>> ItemsManager::findItemByPath(const itemPath& path)
{
    const auto pathKey = getLuaItemKey(path);
    auto iter = std::find_if(items.begin(), items.end(), [pathKey](const baseItemPtr& ptr) {
        return ptr->getLuaKey() == pathKey;
    });

    if (iter == items.end())
        return std::nullopt;

    return std::make_pair((*iter)->getItemPtr(), (*iter)->getItemType());
}

static inline luaItem getCheckBoxItem(std::string name, isVisibleFn&& isVisible)
{
    luaItem result{};
    result.isVisible = isVisible;
    result.itemType = ITEM_CHECKBOX;
    result.item = MAKE_CHECKBOX_RT(name, false);
    return result;
}

static inline luaItem getSliderIntItem(std::string name, int min, int max, isVisibleFn&& isVisible)
{
    luaItem result{};
    result.isVisible = isVisible;
    result.itemType = ITEM_SLIDER_INT;
    result.item = MAKE_SLIDER_RT(name, 0, min, max);
    return result;
}

static inline std::string getLuaItemNameInConfig(std::string name)
{
    return std::format("lua.{}.item.{}", getLuaStateInstance().getLoadedLuaName(), name);
}

std::shared_ptr<BaseItem> ItemsManager::addCheckBox(std::string name, itemPath&& path, isVisibleFn&& isVisible)
{
    auto luaName = getLuaStateInstance().getLoadedLuaName();

    auto localPathTemp = std::move(path);
    localPathTemp.push_back(luaName);

    auto& luaItems = getMenuInstance().luaItems;

    auto currentLua = luaItems.find(luaName);
    if (currentLua == luaItems.end())
        luaItems.insert(std::make_pair(luaName, luaItemsList{}));

    auto& itemsList = luaItems[luaName];
    itemsList.emplace_back(getCheckBoxItem(name, std::forward<isVisibleFn>(isVisible)));

    auto& latestItem = itemsList.back();
    auto& latestCheckBox = std::any_cast<CheckBox&>(latestItem.item);
    getMenuInstance().addCustomItem(latestCheckBox, getLuaItemNameInConfig(name));
    PLACE_CHECKBOX(&latestCheckBox, std::forward<itemPath>(localPathTemp), std::forward<isVisibleFn>(latestItem.isVisible));
    return items.back();
}

std::shared_ptr<BaseItem> ItemsManager::addSliderInt(std::string name, int min, int max, itemPath&& path, isVisibleFn&& isVisible)
{
    auto luaName = getLuaStateInstance().getLoadedLuaName();

    auto localPathTemp = std::move(path);
    localPathTemp.push_back(luaName);

    auto& luaItems = getMenuInstance().luaItems;

    auto currentLua = luaItems.find(luaName);
    if (currentLua == luaItems.end())
        luaItems.insert(std::make_pair(luaName, luaItemsList{}));

    auto& itemsList = luaItems[luaName];
    itemsList.emplace_back(getSliderIntItem(name, min, max, std::forward<isVisibleFn>(isVisible)));

    auto& latestItem = itemsList.back();
    auto& latestSliderInt = std::any_cast<Slider<int>&>(latestItem.item);
    getMenuInstance().addCustomItem(latestSliderInt, getLuaItemNameInConfig(name));
    PLACE_SLIDER_INT(&latestSliderInt, std::forward<itemPath>(localPathTemp), std::forward<isVisibleFn>(latestItem.isVisible));
    return items.back();
}

void ItemsManager::removeLoadedLuaItems(std::string luaName)
{
    auto& luaItems = getMenuInstance().luaItems;

    auto currentLua = luaItems.find(luaName);
    if (currentLua == luaItems.end())
        return;

    auto& itemsList = luaItems[luaName];
    if (itemsList.empty())
        return;

    for (auto& item : itemsList)
    {
        uintptr_t itemPointer{}, itemValuePointer{};
        switch (item.itemType)
        {
        case ITEM_CHECKBOX:
            GET_ITEM_POINTERS(CheckBox);
            break;
        case ITEM_SLIDER_INT:
            GET_ITEM_POINTERS(Slider<int>);
            break;
        case ITEM_SLIDER_FLOAT:
            GET_ITEM_POINTERS(Slider<float>);
            break;
        case ITEM_COMBOBOX:
            GET_ITEM_POINTERS(ComboBox);
            break;
        case ITEM_MULTICOMBOBOX:
            GET_ITEM_POINTERS(MultiComboBox);
            break;
        case ITEM_COLOR:
            GET_ITEM_POINTERS(ColorPicker);
            break;
        }

        getMenuInstance().keyBindManager.removeBindsInItem(itemValuePointer);
        getMenuInstance().removeCustomItem(itemPointer);

        auto result = items.remove_if([itemPointer](const baseItemPtr& item)
            {
                return item->getItemPtr() == itemPointer;
            });

//#ifdef _DEBUG
//        std::string debugMesage = std::format("Can't remove item in ui item list from Lua: {}", getLuaStateInstance().getLoadedLuaName());
//        assert(result > 0 && debugMesage.c_str());
//#else
//        (void)result;
//#endif
    }

    itemsList.clear();
    luaItems.erase(currentLua);
}

ItemsManager& getItemsManagerInstance()
{
    static ItemsManager instance;
    return instance;
}
}
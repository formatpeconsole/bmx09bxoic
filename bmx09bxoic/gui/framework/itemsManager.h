#pragma once

#include "uiItem.h"
#include "../../guiItems/utils.h"
#include "../../render/animation/animation.h"

namespace gui::framework
{
// what happens
// when you put full path to UI (where item should be located)
// name of item added automatically from real item (not rendered one, rendered one just stores pointer of existing item and changes it's state)
// and for itemPath there is two purposes:
// 1) calculate real item path (where it should rendered in UI, which tab, subtab & child (depends on layout ofc but still))
// 2) calculate LUA item path, which will be used for lua api for asking user get item by this way:
//                                                                                                                              
//      A) Copy lua item path which will have output as:
//          For Example:
//              "Ragebot", "Aimbot", "Main", "Enable"
//                                                                                                                              
//      B) Get real item location by already saved key (which will be generated from input as i showed above)
//          For Example:
//              "Ragebot", "Aimbot", "Main", "Enable" 
// 
//          Will be converted to:
//               "ragebot-aimbot-main-enable"
// 
//          And then encrypted to hash
//          And when you want to find item ptr by this name in lua api
//          You will scan ONLY item path's HASH, WHICH IS GENERATED at program launch
// 
//          And find this in item's list:
//              Like 
//                  "ragebot-aimbot-main-enable" hash is 0xAAAAAAAFFFFFF 
// 
//          Will be saved in list
//          And when user will put correct path
//          It will generate hashed key of it 
//          And compare to existing one
//          Which will be faster alot than scanning each "Item" from user's input
#define ITEM_PATH(...) { __VA_ARGS__ }
#define IS_VISIBLE_DUMMY std::nullopt
#define PLACE_CHECKBOX(itemPtr, path, visible) getItemsManagerInstance().placeItemToMainWindow<CheckBox>(itemPtr, path, visible)
#define PLACE_SLIDER_INT(itemPtr, path, visible) getItemsManagerInstance().placeItemToMainWindow<Slider<int>>(itemPtr, path, visible)
#define PLACE_SLIDER_FLOAT(itemPtr, path, visible) getItemsManagerInstance().placeItemToMainWindow<Slider<float>>(itemPtr, path, visible)
#define PLACE_COMBO(itemPtr, path, visible) getItemsManagerInstance().placeItemToMainWindow<ComboBox>(itemPtr, path, visible)
#define PLACE_MULTICOMBO(itemPtr, path, visible) getItemsManagerInstance().placeItemToMainWindow<MultiComboBox>(itemPtr, path, visible)
#define PLACE_COLORPICKER(itemPtr, path, visible) getItemsManagerInstance().placeItemToMainWindow<ColorPicker>(itemPtr, path, visible)

using namespace items;

namespace
{
constexpr auto MAX_ITEMS = 1000;
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

enum UI_RAGE_SUB_TABS
{
    SUBTAB_RAGE_AIMBOT = 0,
    SUBTAB_RAGE_ANTIAIM
};

enum CHILD_ANIMATION
{
    CHILD_SECOND = 0,
    CHILD_THIRD,
};

enum CHILD_CATEGORY
{
    CHILD_CATEGORY_FIRST = 0,
    CHILD_CATEGORY_SECOND,
    CHILD_CATEGORY_THIRD,
};

struct tabAnimation
{
    ImVec4 radioButton{ 0.0f, 0.0f, 0.0f, 0.0f };
    ImVec4 text{ 0.0f, 0.0f, 0.0f, 0.0f };

    render::Animation radioAnimation{ 100.f, 0.f, 0.15f, render::ANIMATION_EASE_OUT_EXPO };
    float radioGlowAlpha{ 0.f };
};

using subTabChilds = std::vector<std::string>;
struct subTab
{
    subTabChilds childs{};
    tabAnimation tabAnim{};
    std::string name{};
    int childCount{};

    subTab(std::string name, subTabChilds childsList)
        : name(name), childs(childsList)
    {
        childCount = childs.size();
    }
};

struct tabItself
{
    std::vector<subTab> subTabs{};
    tabAnimation tabAnim{};
    std::string name{};
    int subTabSelection{};
    bool noSubTabs = false;

    tabItself(std::vector<subTab> subTabs, std::string name, bool noSubTabs) :
        subTabs(subTabs), name(name), subTabSelection(0), noSubTabs(noSubTabs) {
    }

    ~tabItself() {}
};

using mainWindowTabsList = std::vector<tabItself>;
using mainWindowItemsList = std::list<baseItemPtr>;

class ItemsManager
{
public:
    ItemsManager();
    ~ItemsManager();
    mainWindowTabsList& getMainWindowTabsList();
    mainWindowItemsList& getMainWindowItemsList();
    std::optional<RealItemPath> getRealItemPath(itemPath& path);
    std::optional<std::pair<uintptr_t, int>> findItemByPath(const itemPath& path);

    std::shared_ptr<BaseItem> addCheckBox(std::string name, itemPath&& path, isVisibleFn&& isVisible);
    std::shared_ptr<BaseItem> addSliderInt(std::string name, int min, int max, itemPath&& path, isVisibleFn&& isVisible);

    void removeLoadedLuaItems(std::string luaName);

    template<typename T>
    void placeItemToMainWindow(T* ptr, itemPath&& path, isVisibleFn&& isVisible)
    {
        if (const auto size = items.size(); size >= MAX_ITEMS)
        {
#ifdef _DEBUG
            std::string message = "Wrong " + ptr->item.name + " Path!";
            assert(false && "Too much items. Please look at place where you allocate them!");
#endif

            return;
        }

        itemPath newPath{ std::forward<itemPath>(path) };
        newPath.emplace_back(gui::items::getFormattedText(ptr->item.name));

        auto realItemPath = getRealItemPath(newPath);
        if (!realItemPath.has_value())
        {
#ifdef _DEBUG
            std::string message = "Wrong " + ptr->item.name + " Path!";
            assert(false && message.c_str());
#endif
            return;
        }
        items.emplace_back(std::make_shared<UiItem<T>>(ptr, realItemPath.value(), newPath, std::forward<isVisibleFn>(isVisible)));
    }

private:
    void placeLatestItemToConfiguration();

    mainWindowTabsList tabs{};
    mainWindowItemsList items{};
};

extern ItemsManager& getItemsManagerInstance();
}
#pragma once
#include <functional>

#include "../../render/animation/animation.h"
#include "../../guiItems/utils.h"

#include "window.h"
#include "items.h"

namespace gui::framework
{
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
using tabAnimationsList = std::vector<tabAnimation>;

struct tabSelectionAnimation
{
    float xPos{};
    float yPos{ 0.f };
    float beginPosY{};

    ImVec2 base{ 119.f, 87.f };
    ImVec2 first{ 50.f, 0.f };
    ImVec2 second{ 62.f, 7.f };
    ImVec2 third{ 50.f, 14.f };
};

struct mainPositions
{
    ImVec2 baseTabs{ 33.f, 79.f };
    ImVec2 baseTabsContents{ 208.f, 0.f };
    ImVec2 baseSubTabsContents{ 220.f, 25.f };
};

struct tabContentsAnimation
{
    float yPos{};
    float ySize{};
    float xChildSize{};
    float yChildSize{};
    float ySubChildFactor{};
    float subChildsAlpha[2]{};

    render::Animation yPosAnimation{ 59.f, 26.f, 0.35f, render::ANIMATION_EASE_OUT_EXPO };
    render::Animation ySizeAnimation{ 437.f, 470.f, 0.35f, render::ANIMATION_EASE_OUT_EXPO };
    render::Animation xChildSizeAnimation{ 219.f, 451.f, 0.3f, render::ANIMATION_EASE_OUT_EXPO };
    render::Animation yChildSizeAnimation{ 410.f, 443.f, 0.35f, render::ANIMATION_EASE_OUT_EXPO };
    render::Animation selectedTabAnimation{ 0.f, 100.f, 0.35f, render::ANIMATION_EASE_OUT_EXPO, render::ANIMATION_FLAGS_REPLAY_FROM_START };
    render::Animation ySubChildFactorAnimation{ 100.f, 50.f, 0.35f, render::ANIMATION_EASE_OUT_EXPO };
    render::Animation subChildsAlphaAnimation[2]{
        { 100.f, 0.f, 0.35f, render::ANIMATION_EASE_OUT_QUAD },
        { 100.f, 0.f, 0.35f, render::ANIMATION_EASE_OUT_QUAD },
    };
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
        subTabs(subTabs), name(name), subTabSelection(0), noSubTabs(noSubTabs) {}

    ~tabItself() {}
};

using tabsList = std::vector<tabItself>;
using itemsList = std::list<baseItemPtr>;

class MainWindow : public IWindow
{
public:
    MainWindow(tabsList tabs, std::string name, ImVec2 size);
    ~MainWindow();

    void init() override;
    void render() override;
    void updateWindowPosOrSize() override;
    std::string getName() override;
    ImVec2 getPos() override;
    ImVec2 getWindowSize() override;

private:
    void initItems();
    void renderItem(const baseItemPtr& baseItem, const RealItemPath& currentItemPath);
    void renderChildContents(int selection, int subTabSelection, int childType);
    void updateTabsAnimation();
    void renderWindowContents();
    void renderTabs();
    void renderTabsContents();
    void renderBottomInfo();

    std::optional<RealItemPath> getRealItemPath(itemPath& path);

    std::string getBuildType();
    std::string getBuildDate();

    int getMainAlpha();

    template<typename T>
    void placeItem(T* ptr, itemPath&& path, isVisibleFn&& isVisible)
    {
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

    itemsList items{};
    tabsList tabs{};

    tabSelectionAnimation tabSelectionAnim{};
    mainPositions mainPos;
    tabContentsAnimation tabContentsAnim{};

    render::Animation uiOpenAnimation{ 0.f, 100.f, 0.1f, render::ANIMATION_EASE_OUT_QUAD };
    int tabSelection{};
    int oldTabSelection{};

    std::string name{};
    ImVec2 pos{};
    ImVec2 size{};
    float prevDpiScale{};
    float windowAlpha{};
};
}
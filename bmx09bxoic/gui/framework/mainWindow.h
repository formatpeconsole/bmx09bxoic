#pragma once
#include "window.h"
#include "../../render/animation/animation.h"

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

enum CHILD_ANIMATION
{
    CHILD_SECOND = 0,
    CHILD_THIRD,
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
    ImVec2 baseTabs{ 34.f, 84.f };
    ImVec2 baseTabsContents{ 205.f, 0.f };
    ImVec2 baseSubTabsContents{ 220.f, 25.f };
    ImVec2 logo{ 33.f, 22.f };
};

struct tabContentsAnimation
{
    float yPos{};
    float ySize{};
    float xChildSize{};
    float yChildSize{};
    float ySubChildFactor{};
    float subChildsAlpha[2]{};

    render::Animation yPosAnimation{ 57.f, 24.f, 0.35f, render::ANIMATION_EASE_OUT_EXPO };
    render::Animation ySizeAnimation{ 444.f, 477.f, 0.35f, render::ANIMATION_EASE_OUT_EXPO };
    render::Animation xChildSizeAnimation{ 219.f, 451.f, 0.3f, render::ANIMATION_EASE_OUT_EXPO };
    render::Animation yChildSizeAnimation{ 410.f, 443.f, 0.35f, render::ANIMATION_EASE_OUT_EXPO };
    render::Animation selectedTabAnimation{ 0.f, 100.f, 0.35f, render::ANIMATION_EASE_OUT_EXPO, render::ANIMATION_FLAGS_REPLAY_FROM_START };
    render::Animation ySubChildFactorAnimation{ 100.f, 50.f, 0.35f, render::ANIMATION_EASE_OUT_EXPO };
    render::Animation subChildsAlphaAnimation[2]{
        { 100.f, 0.f, 0.35f, render::ANIMATION_EASE_OUT_QUAD },
        { 100.f, 0.f, 0.35f, render::ANIMATION_EASE_OUT_QUAD },
    };
};

struct subTab
{
    std::string name{};
    int childCount{};

    subTab(std::string name, int childCount) : name(name), childCount(childCount) {}
};

struct tabItself
{
    tabAnimationsList subTabAnimations{};
    std::vector<subTab> subTabs{};
    std::string name{};
    int subTabSelection{};
    bool noSubTabs = false;

    tabItself(std::vector<subTab> subTabs, std::string name, bool noSubTabs) :
        subTabs(subTabs), name(name), subTabSelection(0), noSubTabs(noSubTabs)
    {
        subTabAnimations.resize(subTabs.size());
    }

    ~tabItself()
    {
        subTabAnimations.clear();
    }
};

using tabsList = std::vector<tabItself>;
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
    void renderWindowContents();
    void renderTabs();
    void renderTabsContents();
    void renderLogo();
    void renderBottomInfo();

    std::string getBuildType();
    std::string getBuildDate();

    int getMainAlpha();

    std::unordered_map<void*, std::pair<void*, int>> items{};
    tabAnimationsList tabsAnimations{};
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
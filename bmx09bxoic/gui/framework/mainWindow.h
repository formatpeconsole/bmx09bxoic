#pragma once
#include "window.h"

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

struct tabAnimation
{
    ImVec4 radioButton{};
    ImVec4 text{};
};

using tabsList = std::vector<std::string>;
using tabAnimationsList = std::vector<tabAnimation>;

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
    void renderLogo();
    void renderBottomInfo();

    std::string getBuildType();
    std::string getBuildDate();

    int getMainAlpha();

    std::unordered_map<void*, std::pair<void*, int>> items{};
    tabAnimationsList tabsAnimations{};
    tabsList tabs{};

    std::string name{};
    ImVec2 pos{};
    ImVec2 size{};
    int tabSelection{};
    float prevDpiScale{};
    float windowAlpha{};
};
}
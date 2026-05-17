#pragma once
#include "window.h"

namespace gui::framework
{
class BindsWindow : public IWindow
{
public:
    BindsWindow(std::string name, ImVec2 size) 
        : name(name), size(size), prevDpiScale(1.f), prevBindCount(0) {}

    void init() override;
    void reload() override {}
    void render() override;
    void updateWindowPosOrSize() override;

    std::string getName() override;
    ImVec2 getPos() override;
    ImVec2 getWindowSize() override;

private:
    std::unordered_map<uintptr_t, std::pair<uintptr_t, int>> items{};
    std::string name{};
    ImVec2 pos{};
    ImVec2 size{};
    float prevDpiScale{};
    int prevBindCount{};
    float keyBindsYPadding{};
};
}
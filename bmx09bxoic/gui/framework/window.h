#pragma once

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <algorithm>
#include <unordered_map>
#include <any>

#include "../../render/render.h"
#include "../binds/binds.h"
#include "../item.h"

namespace gui::framework
{
inline const auto MAIN_WINDOW_SIZE = ImVec2(713.f, 524.f);
inline const auto BINDS_WINDOW_SIZE = ImVec2(230.f, 75.f);
inline const auto MAIN_WINDOW_ACCENT_COLOR = ImColor(255, 255, 255, 255);
inline auto DPI_SCALE = 1.f;

class IWindow
{
public:
    virtual void init() = 0;
    virtual void render() = 0;
    virtual void updateWindowPosOrSize() = 0;
    virtual std::string getName() = 0;
    virtual ImVec2 getPos() = 0;
    virtual ImVec2 getWindowSize() = 0;
};

}
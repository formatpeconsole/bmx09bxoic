#pragma once
#include <memory>
#include <vector>
#include <string>
#include <optional>
#include <algorithm>
#include <unordered_map>
#include <Windows.h>

#include "../../render/render.h"

namespace gui::binds
{
extern WPARAM ImGui_ImplWin32_ImGuiKeyToKeyEvent(ImGuiKey imgui_key);
extern std::string ImGui_ImplWin32_VKeyToString(int wParam);

extern std::string getItemType(int type);
extern std::string getBindMode(int mode);
}
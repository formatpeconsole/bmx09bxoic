#pragma once
#include <memory>
#include <vector>
#include <string>
#include <optional>
#include <algorithm>
#include <unordered_map>
#include <Windows.h>

#include "binds-manager.h"

namespace gui::binds
{
extern void initBinds();
extern void destroyBinds();
extern void handleMainBinds(UINT uMsg, WPARAM wParam, LPARAM lParam);
}
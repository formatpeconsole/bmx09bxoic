#pragma once
#include <Windows.h>

#include <string>
#include <vector>

namespace config
{
void loadConfig();
void saveConfig();
std::string getFileName();
}
#pragma once
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <Windows.h>

#include "ptr.h"

namespace sigscan
{
Ptr find(HANDLE module, const std::string& mask);
}
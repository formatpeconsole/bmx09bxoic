#include "sigscan.h"

namespace sigscan
{
static std::vector<int> patternToByte(const char* mask)
{
    std::vector<int> result{};
    auto bytes = std::vector<int>{};
    auto start = const_cast<char*>(mask);
    auto end = const_cast<char*>(mask) + strlen(mask);

    for (auto current = start; current < end; ++current)
    {
        if (*current == '?') 
        {
            ++current;
            if (*current == '?')
                ++current;
            bytes.push_back(-1);
        }
        else 
            bytes.push_back(strtoul(current, &current, 16));
    }
    return bytes;
}

Ptr find(HANDLE module, const std::string& mask)
{
    auto dosHeader = (PIMAGE_DOS_HEADER)module;
    auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

    auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
    auto patternBytes = patternToByte(mask.data());
    auto scannedBytes = reinterpret_cast<std::uint8_t*>(module);

    auto s = patternBytes.size();
    auto d = patternBytes.data();

    for (auto i = 0ul; i < sizeOfImage - s; ++i)
    {
        bool found = true;
        for (auto j = 0ul; j < s; ++j)
        {
            if (scannedBytes[i + j] != d[j] && d[j] != -1)
            {
                found = false;
                break;
            }
        }

        if (found)
            return (uintptr_t)&scannedBytes[i];
    }
    return 0;
}
}
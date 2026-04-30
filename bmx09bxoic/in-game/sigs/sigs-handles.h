#pragma once
#include <memory>
#include <Windows.h>

#include "../../mem/ptr.h"

struct ModuleHandles
{
    HANDLE gameoverlayrenderer64;
};

struct Signatures
{
    Ptr overlay_Present;
    Ptr overlay_ResizeBuffers;
    Ptr overlay_CreateSwapChain;
};

extern Signatures& getSignaturesInstance();
extern ModuleHandles& getModuleHandlesInstance();

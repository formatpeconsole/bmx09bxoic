#include "sigs-handles.h"

Signatures& getSignaturesInstance()
{
    static Signatures sigs;
    return sigs;
}

ModuleHandles& getModuleHandlesInstance()
{
    static ModuleHandles moduleHandles;
    return moduleHandles;
}
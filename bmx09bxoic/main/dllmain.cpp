#include <Windows.h>
#include <iostream>
#include <chrono>

#include "dllInstance.h"

#include "../threadpool/threadpool.h"

#include "../mem/sigscan.h"
#include "../in-game/sigs/sigs-handles.h"
#include "../hooks/hooks.h"
#include "../render/render.h"
#include "../gui/gui.h"

using namespace std::chrono_literals;
using namespace gui;

Dll& getDllInstance()
{
    static Dll ptr;
    return ptr;
}

static void findModuleHandles()
{
    DEBUG_LOG("searching module handles...");

    auto& moduleHandles = getModuleHandlesInstance();
    moduleHandles.gameoverlayrenderer64     = GetModuleHandleA("GameOverlayRenderer64.dll");

    DEBUG_LOG("[+]");
}

static void initDllSignatures()
{
    DEBUG_LOG("scanning signatures...");

    auto& moduleHandles = getModuleHandlesInstance();

    auto& signatures = getSignaturesInstance();
    signatures.overlay_Present          = sigscan::find(moduleHandles.gameoverlayrenderer64, "48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 54 41 56 41 57 48 83 EC ? 41 8B E8");
    signatures.overlay_ResizeBuffers    = sigscan::find(moduleHandles.gameoverlayrenderer64, "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 41 54 41 56 41 57 48 83 EC ? 44 8B E2");
    signatures.overlay_CreateSwapChain  = sigscan::find(moduleHandles.gameoverlayrenderer64, "48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 20 48 8B F9 49 8B F1 48 8D ? ? ? ? ? 49 8B D8 48 8B EA E8 ? ? ? ? 48 8D ? ? ? ? ? E8 ? ? ? ? 48 8D ? ? ? ? ? E8 ? ? ? ? 48 8D ? ? ? ? ? E8 ? ? ? ? 48 8B ? ? ? ? ? 4C 8B CE 4C 8B C3 48 8B D5 48 8B CF FF D0 8B D8 85 C0 78 18 48 85 F6 74 13 48 83 3E 00 74 0D 48 8B D5 48 8B CE E8 ? ? ? ? 8B C3 48 8B 5C 24 30 48 8B 6C 24 38 48 8B 74 24 40 48 83 C4 20 5F C3 CC CC CC CC CC CC CC CC 48 83 EC 38 48 8B 01 4C 8D 44 24 40");

    DEBUG_LOG("[+]");
}

static void hookGameFunctions()
{
    DEBUG_LOG("hooking functions...");

    MH_Initialize();

    auto& signatures    = getSignaturesInstance();
    auto& hookedFuncs   = hooks::getHookedFuncsInstance();

    hookedFuncs.presentHook.hook(hooks::Hooked_Present, signatures.overlay_Present.as<void*>());
    hookedFuncs.resizeBuffersHook.hook(hooks::Hooked_ResizeBuffers, signatures.overlay_ResizeBuffers.as<void*>());
    hookedFuncs.createSwapChainHook.hook(hooks::Hooked_CreateSwapChain, signatures.overlay_CreateSwapChain.as<void*>());

    MH_EnableHook(MH_ALL_HOOKS);

    DEBUG_LOG("[+]");
}

static void unhookGameFunctions()
{
    DEBUG_LOG("destroying hooks...");

    MH_DisableHook(MH_ALL_HOOKS);
    MH_RemoveHook(MH_ALL_HOOKS);

    MH_Uninitialize();

    DEBUG_LOG("[+]");
}

void APIENTRY Main(HMODULE handle)
{
    createConsole();

    gui::init();
    getSimpleThreadPoolInstance().init();

    DEBUG_LOG("creating instance...");

    DEBUG_LOG("[+]");
    DEBUG_LOG("continue...");

    findModuleHandles();
    initDllSignatures();
    hookGameFunctions();

    const auto state = MessageBoxA(0, "HI", 0, 0);
    if (state == IDOK)
        getDllInstance().shouldQuit = true;

    while (!getDllInstance().shouldQuit)
        std::this_thread::sleep_for(1s);

    getSimpleThreadPoolInstance().destroy();
    unhookGameFunctions();
    render::destroy();
    gui::destroy();

    destroyConsole();
    FreeLibraryAndExitThread(handle, 0);
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Main, hModule, 0, 0);

    return TRUE;
}
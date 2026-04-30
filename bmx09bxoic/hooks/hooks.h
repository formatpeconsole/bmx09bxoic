#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <MinHook.h>

template<typename T = void*>
struct hookData
{
    void* original;

    void hook(T func, void* ptr)
    {
        MH_CreateHook(ptr, func, &original);
    }

    T getOriginal()
    {
        return (T)original;
    }
};

namespace hooks
{
LRESULT WINAPI Hooked_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HRESULT Hooked_Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
HRESULT Hooked_ResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
HRESULT WINAPI Hooked_CreateSwapChain(IDXGIFactory* pFactory, IUnknown* pDevice, DXGI_SWAP_CHAIN_DESC* pDesc, IDXGISwapChain** ppSwapChain);

struct hookedFuncs
{
    hookData<decltype(&Hooked_Present)> presentHook;
    hookData<decltype(&Hooked_ResizeBuffers)> resizeBuffersHook;
    hookData<decltype(&Hooked_CreateSwapChain)> createSwapChainHook;
};

extern hookedFuncs& getHookedFuncsInstance();
}
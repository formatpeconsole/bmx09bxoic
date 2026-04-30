#include "binds.h"
#include "../gui.h"

namespace gui::binds
{
void initBinds()
{
    getMenuInstance().keyBindManager.init();
    getMenuInstance().keyBindManager.addBind(
        &getMenuInstance().opened,
        &getMenuInstance().newOpened,
        &getMenuInstance().openedOldValue,
        BIND_TOGGLE,
        ITEM_UI_OPEN,
        VK_INSERT,
        {},
        {}
    );

    auto& bindList = getMenuInstance().keyBindManager.getBindList();
    const auto menuKey = std::find_if(bindList.begin(), bindList.end(), [](const std::shared_ptr<IKeyBind>& keyBind)
        {
            return keyBind->getKey() == VK_INSERT;
        });

    if (menuKey != bindList.end())
    {
        auto bindToUpdate = (*menuKey);
        bindToUpdate->setOldValue();
        bindToUpdate->updateNewValue();
    }
}
void destroyBinds()
{
    getMenuInstance().keyBindManager.destroy();
}

bool isMouseDown(UINT uMsg)
{
    switch (uMsg)
    {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONDBLCLK:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONDBLCLK:
        return true;
    }
    return false;
}

bool isMouseUp(UINT uMsg)
{
    switch (uMsg)
    {
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_XBUTTONUP:
        return true;
    }
    return false;
}

bool isKeyDown(UINT uMsg)
{
    if (uMsg == WM_KEYDOWN 
        || uMsg == WM_SYSKEYDOWN
        || isMouseDown(uMsg))
        return true;

    return false;
}

bool isKeyUp(UINT uMsg)
{
    if (uMsg == WM_KEYUP 
        || uMsg == WM_SYSKEYUP
        || isMouseUp(uMsg))
        return true;

    return false;
}

int getButtonKey(WPARAM wParam)
{
    UINT button = GET_XBUTTON_WPARAM(wParam);
    if (button == XBUTTON1)
        return VK_XBUTTON1;
    else if (button == XBUTTON2)
        return VK_XBUTTON2;

    return wParam;
}

int getKeyByMessage(UINT uMsg, WPARAM wParam)
{
    switch (uMsg)
    {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
        return VK_LBUTTON;
    case WM_RBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
        return VK_RBUTTON;
    case WM_MBUTTONDOWN:
    case WM_MBUTTONDBLCLK:
        return VK_MBUTTON;
    case WM_XBUTTONDOWN:
    case WM_XBUTTONDBLCLK:
        return wParam;
    case WM_LBUTTONUP:
        return VK_LBUTTON;
    case WM_RBUTTONUP:
        return VK_RBUTTON;
    case WM_MBUTTONUP:
        return VK_MBUTTON;
    }
    return wParam;
}

void handleMainBinds(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto& bindManager = getMenuInstance().keyBindManager;
    auto& bindList = bindManager.getBindList();
    auto buttonKey = getButtonKey(getKeyByMessage(uMsg, wParam));

    for (auto it = bindList.begin(); it != bindList.end(); ++it)
    {
        if ((*it)->getKey() != buttonKey)
            continue;

        auto bindToUpdate = (*it);
        const auto bindType = bindToUpdate->getType();

        if (bindType != BIND_ALWAYS_ON && bindType != BIND_FORCE_OFF)
        {
            switch (bindType)
            {
            case BIND_TOGGLE:
            {
                // according to winapi docs
                // 30th bit of lParam contains previous state of message
                // that button sends

                // update ui pressed state only when you didn't press it before
                // (allows to bypass spamming value when user hold key)
                if (isKeyDown(uMsg))
                {
                    bool newState = !((lParam >> 30) & 1);
                    if (newState)
                    {
                        if (!bindToUpdate->getPressed())
                            bindManager.onKeyDown(bindToUpdate);
                        else
                            bindManager.onKeyUp(bindToUpdate);

                        bindToUpdate->setPressed(!bindToUpdate->getPressed());
                    }
                }
                else if (isKeyUp(uMsg))
                {
                    bindManager.onKeyUp(bindToUpdate);
                }
            }
            break;
            case BIND_HOLD:
            case BIND_RELEASE:
            {
                if (isKeyDown(uMsg))
                {
                    if (!bindToUpdate->getPressed())
                    {
                        bindManager.onKeyDown(bindToUpdate);
                        bindToUpdate->setPressed(true);
                    }
                }
                else if (isKeyUp(uMsg))
                {
                    if (bindToUpdate->getPressed())
                    {
                        bindManager.onKeyUp(bindToUpdate);
                        bindToUpdate->setPressed(false);
                    }
                }
            }
            break;
            }
        }
    }
}
}
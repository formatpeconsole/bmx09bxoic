#pragma once
#include "../gui/item.h"
#include "../gui/gui.h"
#include "../gui/binds/utils.h"

namespace gui::items::keybind
{
using namespace binds;

template<typename T>
void keyBindSelector(const std::shared_ptr<IKeyBind> bind, typename T::iterator it)
{
    if (!it->keyEmpty)
    {
        if (ImGui::SmallButton(it->label.c_str()))
        {
            it->keyEmpty = true;
        }
    }
    else
    {
        it->label = "...";
        ImGui::SmallButton(it->label.c_str());

        if (ImGui::IsKeyDown(ImGuiKey_Escape))
        {
            it->bindKey = -1;
            it->label = "None";
            it->keyEmpty = false;
            return;
        }

        bool keyFound = false;
        for (int i = ImGuiMouseButton_Left; i < ImGuiMouseButton_COUNT; i++)
        {
            if (ImGui::GetIO().MouseDown[i])
            {
                switch (i)
                {
                case 0:
                    it->bindKey = VK_LBUTTON;
                    break;
                case 1:
                    it->bindKey = VK_RBUTTON;
                    break;
                case 2:
                    it->bindKey = VK_MBUTTON;
                    break;
                case 3:
                    it->bindKey = VK_XBUTTON1;
                    break;
                case 4:
                    it->bindKey = VK_XBUTTON2;
                    break;
                }

                it->label = ImGui_ImplWin32_VKeyToString(it->bindKey);
                it->keyEmpty = false;
                bind->setKey(it->bindKey);
                keyFound = true;
                break;
            }
        }

        if (keyFound)
            return;

        for (int i = ImGuiKey_NamedKey_BEGIN; i < ImGuiKey_NamedKey_END; ++i)
        {
            if (ImGui::IsKeyDown((ImGuiKey)i))
            {
                it->bindKey = (int)ImGui_ImplWin32_ImGuiKeyToKeyEvent((ImGuiKey)i);
                it->label = ImGui_ImplWin32_VKeyToString(it->bindKey);
                it->keyEmpty = false;
                bind->setKey(it->bindKey);
                break;
            }
        }
    }
}
}
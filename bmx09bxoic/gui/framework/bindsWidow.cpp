#include "bindsWindow.h"
#include "../gui.h"
#include "../../guiItems/items.h"

namespace gui::framework
{
using namespace items;

uintptr_t getItemValuePointerFromItemPointer(uintptr_t ptr, int type)
{
    switch (type)
    {
    case ITEM_CHECKBOX:
        return reinterpret_cast<uintptr_t>(& reinterpret_cast<Item<bool>*>(ptr)->value);
    case ITEM_SLIDER_INT:
        return reinterpret_cast<uintptr_t>(&reinterpret_cast<Item<int>*>(ptr)->value);
    case ITEM_SLIDER_FLOAT:
        return reinterpret_cast<uintptr_t>(&reinterpret_cast<Item<float>*>(ptr)->value);
    case ITEM_COMBOBOX:
        return reinterpret_cast<uintptr_t>(&reinterpret_cast<Item<int>*>(ptr)->value);
    case ITEM_MULTICOMBOBOX:
        return reinterpret_cast<uintptr_t>(&reinterpret_cast<Item<unsigned int>*>(ptr)->value);
    case ITEM_COLOR:
        return reinterpret_cast<uintptr_t>(&reinterpret_cast<Item<unsigned int>*>(ptr)->value);
    default:
        return {};
    }
}

std::optional<std::vector<std::string>> getItemList(uintptr_t ptr, int type)
{
    switch (type)
    {
    case ITEM_COMBOBOX:
    {
        auto item = reinterpret_cast<Item<int>*>(ptr);
        return item->itemsList;
    }
    break;
    case ITEM_MULTICOMBOBOX:
    {
        auto item = reinterpret_cast<Item<unsigned int>*>(ptr);
        return item->itemsList;
    }
    break;
    default:
        return {};
    }
}

// in KeyBindManager i had to save value in string format for UI purposes
// now you have to get this value and visualize in ur UI however you want
std::string getBindValueFromString(const std::string& value, uintptr_t ptr, int itemType)
{
    switch (itemType)
    {
    case ITEM_CHECKBOX:
        return value == "1" ? "On" : "Off";
    case ITEM_SLIDER_FLOAT:
        return value + "f";
    case ITEM_SLIDER_INT:
        return value;
    case ITEM_COMBOBOX:
    {
        auto list = getItemList(ptr, itemType);
        if (list.has_value())
        {
            int index = std::stoi(value);
            if (index < 0 || index >= list->size())
                return "Invalid";

            return list->at(index);
        }
        return {};
    }
    break;
    case ITEM_MULTICOMBOBOX:
    {
        auto list = getItemList(ptr, itemType);
        if (list.has_value())
        {
            unsigned int index = std::stoul(value);
            if (index == 0)
                return "Invalid";

            return gui::multicombobox::getActiveItems(index, list.value());
        }
        return {};
    }
    break;
    case ITEM_COLOR:
    {
        uint32_t color = std::stoul(value);

        auto r = (color >> 16) & 0xFF;
        auto g = (color >> 8) & 0xFF;
        auto b = color & 0xFF;
        auto a = (color >> 24) & 0xFF;

        // R: 255 G: 255 B: 255: A: 255
        return "R: "
            + std::to_string(r)
            + " G: "
            + std::to_string(g)
            + " B: "
            + std::to_string(b)
            + " A: "
            + std::to_string(a);
    }
    break;
    default:
        return value;
    }
}

void BindsWindow::init()
{
    items.clear();

    auto& itemsInMemory = getMenuInstance().itemsInMemory;
    for (auto& i : itemsInMemory)
        items[getItemValuePointerFromItemPointer(i.first, i.second)] = std::make_pair(i.first, i.second);
}

void BindsWindow::render()
{
    getMenuInstance().windowsManager.correctSavedUIPos(name);

    if (prevDpiScale != DPI_SCALE)
    {
        float scaleStep = DPI_SCALE / prevDpiScale;

        size *= scaleStep;
        prevDpiScale = DPI_SCALE;
    }

    ImGui::SetNextWindowSize(size);

    int bindCount = 0;
    ImGui::Begin(name.c_str(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
    {
        auto& binds = getMenuInstance().keyBindManager.getBindList();
 
        bool noBinds = true;
        for (auto it = binds.rbegin(); it != binds.rend(); ++it)
        {
            auto item = (*it);
            if (item->getItemType() == ITEM_UI_OPEN)
                continue;

            auto itemThatBindUse = items.find(item->getItemPtr());
            if (itemThatBindUse == items.end())
                continue;

            bool keyPressed = item->getType() == BIND_RELEASE ? !item->getPressed() : item->getPressed();
            if (keyPressed)
            {
                ++bindCount;
                noBinds = false;

                // bind format would be:
                // [+] ItemName -> Value (BindMode)
                std::string bindValue = getBindValueFromString(item->getBindValue(), itemThatBindUse->second.first, itemThatBindUse->second.second);
                std::string infoBegin = item->getItemName();
                std::string infoEnd = " " + bindValue + " (" + gui::binds::getBindMode(item->getType()) + ")";

                ImGui::Text("%s%s", getFormattedText(infoBegin).c_str(), infoEnd.c_str());
            }
        }

        if (noBinds)
            ImGui::Text("No active binds found.");
    }
    ImGui::End();

    if (prevBindCount != bindCount)
    {
        if (prevBindCount > bindCount)
            size.y -= 12.f * (prevBindCount - bindCount);
        else
            size.y += 12.f * (bindCount - prevBindCount);

        prevBindCount = bindCount;
    }
}

void BindsWindow::updateWindowPosOrSize()
{
    const auto latestWindow = GImGui->Windows.back();
    if (latestWindow != nullptr)
        pos = latestWindow->Pos;
}

std::string BindsWindow::getName()
{
    return name;
}

ImVec2 BindsWindow::getPos()
{
    auto window = ImGui::FindWindowByName(name.c_str());
    if (window != nullptr)
        return window->Pos;

    return {};
}

ImVec2 BindsWindow::getWindowSize()
{
    return size;
}
}
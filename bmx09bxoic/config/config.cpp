#include "config.h"

#include <fstream>
#include <iostream>
#include <json.hpp>

#include "../gui/gui.h"
#include "../gui/binds/utils.h"
#include "../utils/uuid.h"

#define SAVE_ITEM(section, element) addItem(section, element.item);
#define LOAD_ITEM(section, element) loadItem(section, element.item);

namespace config
{
using namespace gui;

std::string readFile(std::string fileName)
{
    std::ifstream file(fileName);
    if (!file.is_open())
        return "";

    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

template<typename T>
void addItem(nlohmann::json& jsonToWrite, const Item<T>& item)
{
    auto& itemJson = jsonToWrite;

    T valueToRead = item.value;

    auto activeBind = getMenuInstance().keyBindManager.findBindByItem(&item.value);
    if (activeBind != nullptr)
    {
        bool pressed = activeBind->getPressed();
        if (activeBind->getType() == BIND_RELEASE)
            pressed = !activeBind->getPressed();

        if (pressed && item.oldValue.wrote)
            valueToRead = item.oldValue.value;
    }

    itemJson["value"] = valueToRead;
    itemJson["itemType"] = binds::getItemType(item.itemType);
    itemJson["binds"] = nlohmann::json::array();
    for (auto& i : item.binds)
    {
        if (i.name.empty())
            continue;
        nlohmann::json bindJson{};
        bindJson["name"] = i.name;
        bindJson["value"] = i.value;
        bindJson["bindMode"] = i.bindMode;
        bindJson["bindModeName"] = binds::getBindMode(i.bindMode + 1);
        bindJson["bindKey"] = i.bindKey;
        itemJson["binds"].push_back(bindJson);
    }
}

template<typename T>
void loadItem(nlohmann::json& jsonResult, Item<T>& item)
{
    item.preview.erased = false;
    item.preview.selection = false;
    item.preview.selectedBind.reset();
    item.preview.label = "No binds.";

    item.binds.clear();

    if (jsonResult.empty())
        return;

    item.value = jsonResult["value"].get<T>();

    auto& bindsSection = jsonResult["binds"];
    for (auto& bind : bindsSection)
    {
        auto& newBind = item.binds.emplace_back();
        newBind.name = bind["name"].get<std::string>();
        newBind.value = bind["value"].get<T>();
        newBind.bindMode = bind["bindMode"].get<int>();
        newBind.bindKey = bind["bindKey"].get<int>();
        newBind.keyEmpty = false;
        newBind.label = binds::ImGui_ImplWin32_VKeyToString(newBind.bindKey);

        getMenuInstance().keyBindManager.addBind(
            &item.value,
            &newBind.value,
            &item.oldValue,
            newBind.bindMode + 1,
            item.itemType,
            newBind.bindKey,
            newBind.name,
            item.name
        );
    }
}

void loadConfig()
{
    auto fileToString = readFile(getFileName());
    auto jsonResult = nlohmann::json::parse(fileToString);

    auto& instance = getMenuInstance();
    instance.keyBindManager.eraseAllBinds();

    for (auto& item : instance.itemsInMemory)
    {
        auto& section = jsonResult[item.configSection];
        switch (item.type)
        {
        case ITEM_CHECKBOX:
            LOAD_ITEM(section, (*reinterpret_cast<CheckBox*>(item.ptr)));
            break;
        case ITEM_SLIDER_INT:
            LOAD_ITEM(section, (*reinterpret_cast<Slider<int>*>(item.ptr)));
            break;
        case ITEM_SLIDER_FLOAT:
            LOAD_ITEM(section, (*reinterpret_cast<Slider<float>*> (item.ptr)));
            break;
        case ITEM_COMBOBOX:
            LOAD_ITEM(section, (*reinterpret_cast<ComboBox*>(item.ptr)));
            break;
        case ITEM_MULTICOMBOBOX:
            LOAD_ITEM(section, (*reinterpret_cast<MultiComboBox*>(item.ptr)));
            break;
        case ITEM_COLOR:
            LOAD_ITEM(section, (*reinterpret_cast<ColorPicker*>(item.ptr)));
            break;
        }
    }
}

void saveConfig()
{
    std::ofstream configFile(getFileName());
   
    nlohmann::json jsonToWrite{};

    auto& instance = getMenuInstance();
    for (auto& item : instance.itemsInMemory)
    {
        auto& section = jsonToWrite[item.configSection];
        switch (item.type)
        {
        case ITEM_CHECKBOX:
            SAVE_ITEM(section, (*reinterpret_cast<CheckBox*>(item.ptr)));
            break;
        case ITEM_SLIDER_INT:
            SAVE_ITEM(section, (*reinterpret_cast<Slider<int>*>(item.ptr)));
            break;
        case ITEM_SLIDER_FLOAT:
            SAVE_ITEM(section, (*reinterpret_cast<Slider<float>*> (item.ptr)));
            break;
        case ITEM_COMBOBOX:
            SAVE_ITEM(section, (*reinterpret_cast<ComboBox*>(item.ptr)));
            break;
        case ITEM_MULTICOMBOBOX:
            SAVE_ITEM(section, (*reinterpret_cast<MultiComboBox*>(item.ptr)));
            break;
        case ITEM_COLOR:
            SAVE_ITEM(section, (*reinterpret_cast<ColorPicker*>(item.ptr)));
            break;
        }
    }

    configFile << jsonToWrite;
}

std::string getFileName()
{
    return "testFile.txt";
}
}
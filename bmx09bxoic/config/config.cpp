#include "config.h"

#include <fstream>
#include <iostream>
#include <json.hpp>

#include "../gui/gui.h"
#include "../gui/binds/utils.h"
#include "../utils/uuid.h"

#define SAVE_ITEM(section, element) addItem(section[getItemName(element.item)], element.item);
#define LOAD_ITEM(section, element) loadItem(section[getItemName(element.item)], element.item);

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
std::string getItemName(const Item<T>& item)
{
    return getItemType(item.itemType) + "-" + item.name;
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

    auto& rageSection = jsonResult["Rage"];
    if (!rageSection.empty())
    {
        for (int i = 0; i < MAX_CONFIGS; ++i)
        {
            LOAD_ITEM(rageSection, instance.rage.config[i].autoFire);
            LOAD_ITEM(rageSection, instance.rage.config[i].autoScope);
            LOAD_ITEM(rageSection, instance.rage.config[i].fov);
            LOAD_ITEM(rageSection, instance.rage.config[i].multiPoints);
            LOAD_ITEM(rageSection, instance.rage.config[i].pointHeadScale);
            LOAD_ITEM(rageSection, instance.rage.config[i].pointBodyScale);
            LOAD_ITEM(rageSection, instance.rage.config[i].hitChance);
            LOAD_ITEM(rageSection, instance.rage.config[i].minDamage);
            LOAD_ITEM(rageSection, instance.rage.config[i].preferBody);
            LOAD_ITEM(rageSection, instance.rage.config[i].quickStop);
            LOAD_ITEM(rageSection, instance.rage.config[i].overrideGlobal);
        }

        LOAD_ITEM(rageSection, instance.rage.configSelect);
    }
}

void saveConfig()
{
    std::ofstream configFile(getFileName());
   
    nlohmann::json jsonToWrite{};

    auto& instance = getMenuInstance();
    auto& rageSection = jsonToWrite["Rage"];
    {
        for (int i = 0; i < MAX_CONFIGS; ++i)
        {
            SAVE_ITEM(rageSection, instance.rage.config[i].autoFire);
            SAVE_ITEM(rageSection, instance.rage.config[i].autoScope);
            SAVE_ITEM(rageSection, instance.rage.config[i].fov);
            SAVE_ITEM(rageSection, instance.rage.config[i].hitBoxes);
            SAVE_ITEM(rageSection, instance.rage.config[i].multiPoints);
            SAVE_ITEM(rageSection, instance.rage.config[i].pointHeadScale);
            SAVE_ITEM(rageSection, instance.rage.config[i].pointBodyScale);
            SAVE_ITEM(rageSection, instance.rage.config[i].hitChance);
            SAVE_ITEM(rageSection, instance.rage.config[i].minDamage);
            SAVE_ITEM(rageSection, instance.rage.config[i].preferBody);
            SAVE_ITEM(rageSection, instance.rage.config[i].quickStop);
            SAVE_ITEM(rageSection, instance.rage.config[i].overrideGlobal);
        }

        SAVE_ITEM(rageSection, instance.rage.configSelect);
    }

    configFile << jsonToWrite;
}

std::string getFileName()
{
    return "testFile.txt";
}
}
#pragma once

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <algorithm>
#include <unordered_map>

#include "../gui/item.h"
#include "../gui/gui.h"
#include "keybind.h"
#include "utils.h"

namespace gui::items::combobox
{
inline void addComboBoxBind(ComboBox& comboBox)
{
    auto& item = comboBox.item;
    auto& newBind = item.binds.emplace_back();
    newBind.name = "ComboBoxBind-For " + item.name + " " + uuid::getUuid();

    getMenuInstance().keyBindManager.addBind(
        &item.value,
        &newBind.value,
        &item.oldValue,
        BIND_NONE,
        item.itemType,
        0,
        newBind.name,
        comboBox.item.name
    );
}

inline decltype(&addComboBoxBind) comboBoxBindCallback = addComboBoxBind;

inline const char* Items_VectorGetter(void* data, int idx)
{
    auto items = reinterpret_cast<std::string*>(data);
    return items[idx].c_str();
}

inline void render(ComboBox& comboBox, int childType)
{
    if (getMenuInstance().getChildType() != childType)
        return;

    auto& item = comboBox.item;

    std::string itemKey = std::to_string(reinterpret_cast<uintptr_t>(&item));
    std::string itemValueKey = std::to_string(reinterpret_cast<uintptr_t>(&item.value));

    std::string bindItemPopup = "bind-popup-comboBox##" + itemKey;
    std::string bindCombo = "##binds-for-" + itemValueKey;

    std::string bindAdd = "+##bind-add-for-" + itemKey;

    std::string bindOpenPopup = "+##" + itemKey;

    auto comboSize = static_cast<int>(item.itemsList.size());

    {
        std::string hiddenName = "##combo-box-for-" + item.name;

        ImGui::Text(getFormattedText(item.name).c_str());
        ImGui::Combo(hiddenName.c_str(), &item.value, Items_VectorGetter, item.itemsList.data(), comboSize);
        item.value = std::clamp(item.value, 0, comboSize - 1);
    }
    ImGui::OpenPopupOnItemClick(bindItemPopup.c_str(), ImGuiPopupFlags_MouseButtonRight);

    if (ImGui::BeginPopup(bindItemPopup.c_str()))
    {
        ImGui::Text("Binds");

        auto& preview = item.preview;
        preview.label = getBindsComboLabel(item, item.preview);

        if (ImGui::BeginCombo(bindCombo.c_str(), preview.label.c_str()))
        {
            if (item.binds.empty())
            {
                ImGui::Text("No binds found! Click + to add bind");

                if (preview.erased)
                {
                    preview.selection = 0;
                    preview.selectedBind.reset();
                    preview.erased = false;
                }

                if (ImGui::SmallButton(bindAdd.c_str()))
                {
                    comboBoxBindCallback(comboBox);

                    preview.selection = 0;
                    preview.selectedBind.reset();
                }
            }
            else
            {
                int bindsIter = 0;
                auto& binds = item.binds;
                for (auto it = binds.begin(); it != binds.end();)
                {
                    if (preview.erased && preview.selection == bindsIter)
                    {
                        preview.selectedBind = it;
                        preview.erased = false;
                    }

                    it->previewName = getPreviewItemName(*it);
                    if (ImGui::Selectable(it->previewName.c_str(), preview.selection == bindsIter, 0, ImVec2(100, 15)))
                    {
                        preview.selection = bindsIter;
                        preview.selectedBind = it;
                    }

                    ImGui::SameLine();
                    ++bindsIter;

                    const std::string bindPlus = "+##bind_" + it->name;
                    const std::string bindMinus = "-##bind_" + it->name;

                    if (ImGui::SmallButton(bindPlus.c_str()))
                    {
                        comboBoxBindCallback(comboBox);
                        preview.selectedBind.reset();
                        preview.selection = bindsIter;
                        continue;
                    }

                    ImGui::SameLine();

                    if (ImGui::SmallButton(bindMinus.c_str()))
                    {
                        getMenuInstance().keyBindManager.removeBind(getMenuInstance().keyBindManager.findBind(&it->value)->getBindName());
                        it = binds.erase(it);
                        preview.selectedBind.reset();
                        preview.selection = bindsIter;
                        preview.erased = true;
                        continue;
                    }

                    ++it;
                }
            }

            ImGui::EndCombo();
        }

        if (preview.selectedBind.has_value())
        {
            auto& value = preview.selectedBind.value();
            std::string valueName = "Value ##" + value->name;
            std::string bindType = "##bind-type-to" + value->name;

            auto currentBind = getMenuInstance().keyBindManager.findBind(&value->value);
            if (currentBind != nullptr)
            {
                ImGui::Text("Type");
                ImGui::Combo(bindType.c_str(), &value->bindMode, "Always On\0Hold\0Toggle\0Release\0Force Off\0");

                currentBind->setType(value->bindMode + 1);

                ImGui::Text("Current Key");
                ImGui::SameLine();
                keybind::keyBindSelector<std::list<BindValues<int>>>(currentBind, value);
            }

            value->previewName = getPreviewItemName(*value);

            {
                auto comboSize = static_cast<int>(item.itemsList.size());

                std::string hiddenName = "##combo-bind-box-for-" + valueName;

                ImGui::Text(getFormattedText(valueName).c_str());
                ImGui::Combo(hiddenName.c_str(), &value->value, Items_VectorGetter, item.itemsList.data(), static_cast<int>(item.itemsList.size()));
                value->value = std::clamp(value->value, 0, comboSize - 1);
            }
        }

        ImGui::EndPopup();
    }
}
}
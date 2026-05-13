#pragma once

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <algorithm>
#include <unordered_map>

#include "../gui/item.h"
#include "../gui/gui.h"
#include "../gui/framework/items.h"

#include "keybind.h"
#include "utils.h"

namespace gui::items::checkbox
{
inline void addCheckBoxBind(CheckBox& checkbox)
{
    auto& item = checkbox.item;
    auto& newBind = item.binds.emplace_back();
    newBind.name = "CheckBox-For " + item.name + " " + uuid::getUuid();

    getMenuInstance().keyBindManager.addBind(
        &item.value,
        &newBind.value,
        &item.oldValue,
        BIND_NONE,
        item.itemType,
        0,
        newBind.name,
        checkbox.item.name
    );
}

inline decltype(&addCheckBoxBind) checkBoxBindCallback = addCheckBoxBind;

inline void render(const gui::framework::baseItemPtr& baseItem)
{
    auto checkBoxItemPtr = reinterpret_cast<CheckBox*>(baseItem->getItemPtr());

    auto& item = checkBoxItemPtr->item;

    std::string itemKey = std::to_string(reinterpret_cast<uintptr_t>(&item));
    std::string itemValueKey = std::to_string(reinterpret_cast<uintptr_t>(&item.value));

    std::string bindItemPopup = "bind-popup-checkbox##" + itemKey;
    std::string bindCombo = "##binds-for-" + itemValueKey;

    std::string bindAdd = "+##bind-add-for-" + itemKey;

    std::string bindOpenPopup = "+##" + itemKey;

    {
        std::string hiddenName = "##checkbox-for-" + item.name;
        ImGui::Text(getFormattedText(item.name).c_str());
        ImGui::SameLine();
        ImGui::Checkbox(hiddenName.c_str(), &item.value);
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
                    checkBoxBindCallback(*checkBoxItemPtr);

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
                        checkBoxBindCallback(*checkBoxItemPtr);
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
                keybind::keyBindSelector<std::list<BindValues<bool>>>(currentBind, value);
            }

            value->previewName = getPreviewItemName(*value);

            {
                std::string hiddenName = "##checkbox-for-" + valueName;
                ImGui::Text(getFormattedText(valueName).c_str());
                ImGui::SameLine();
                ImGui::Checkbox(hiddenName.c_str(), &value->value);
            }
        }

        ImGui::EndPopup();
    }
}
}
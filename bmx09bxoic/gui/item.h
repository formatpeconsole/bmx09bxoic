#pragma once

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <algorithm>
#include <unordered_map>

#include "binds/binds.h"

namespace gui::items
{
using namespace binds;

template<typename T>
struct BindValues
{
    T value = {};
    int bindMode = -1;
    int bindKey = 0;
    std::string name{};
    std::string previewName{"New bind"};
    std::string lastKey{};
    std::string label{ "Press any key" };
    bool keyEmpty = false;
};

template<typename T>
struct BindPreview
{
    int selection = 0;
    std::string label{};
    std::optional<typename std::list<BindValues<T>>::iterator> selectedBind{};
    bool erased = false;
};

template<typename T>
struct Item
{
    ItemOldValue<T> oldValue{};
    T value{};
    std::list<BindValues<T>> binds{};

    BindPreview<T> preview{};
    std::string name{};

    int itemType{};
    std::vector<std::string> itemsList{};
};

template<typename T>
struct Slider
{
    Item<T> item{};

    T min{};
    T max{};
};

struct CheckBox
{
    Item<bool> item{};
};

struct ComboBox
{
    Item<int> item{};
};

struct MultiComboBox
{
    Item<unsigned int> item{};
};

struct ColorPicker
{
    Item<unsigned int> item{};
};
}
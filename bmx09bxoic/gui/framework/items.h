#pragma once
#include <sstream>
#include <optional>
#include <functional>

#include "window.h"
#include "../item.h"

#include "../../deps/fnv64hash/hash.h"

namespace gui::framework
{
using namespace items;

// get item key that stores in hash
// key containts full path to item
//
// for example:
//
// checkbox "Enable" that located at "Ragebot->Aimbot->Main(groupbox)"
// full key will look like:
// ragebot-aimbot-main-enable
using itemPath = std::vector<std::string>;
inline fnv64Hash getLuaItemKey(itemPath path)
{
    std::string key{};
    bool first = true;
    for (auto item : path)
    {
        std::transform(item.begin(), item.end(), item.begin(), [](char c) { return std::tolower(c); });

        if (!first)
            key += "-";

        key += item;
        first = false;
    }

    return fnv64::fnv1a_hash(key);
}

// get full path to make key
// 
// for example:
//
// checkbox "Enable" that located at "Ragebot->Aimbot->Main(groupbox)"
// full key will look like:
// "Ragebot", "Aimbot", "Main", "Enable"
inline std::string getLuaItemPath(itemPath path)
{
    std::ostringstream oss{};
    bool first = true;

    for (auto item : path)
    {
        if (!first) 
            oss << ", ";

        oss << '"' << item << '"';
        first = false;
    }

    return oss.str();
}

struct LuaItemKey
{
    std::string path{};
    fnv64Hash hash{};
};

struct RealItemPath
{
    int tab{-1};
    int subTab{-1};
    int childCategory{-1};

    bool canRender(const RealItemPath& other)
    {
        return tab == other.tab
            && subTab == other.subTab
            && childCategory == other.childCategory;
    }
};

class BaseItem
{
public:
    virtual bool isVisible() = 0;
    virtual int getItemType() = 0;
    virtual RealItemPath getRealItemPath() = 0;
    virtual void* getItemPtr() = 0;
    virtual void createKey() = 0;
    virtual fnv64Hash getLuaKey() = 0;
    virtual std::string getLuaPath() = 0;
};
using baseItemPtr = std::shared_ptr<BaseItem>;
using isVisibleFn = std::optional<std::function<bool()>>;

template<typename T>
class UiItem : public BaseItem
{
public:
    UiItem(T* itemPtr, RealItemPath realItemPath, itemPath path, isVisibleFn&& visibleCallback)
        : path(path), realItemPath(realItemPath),
        itemPtr(itemPtr), itemType(itemPtr->item.itemType),
        visibleCallback(std::forward<isVisibleFn>(visibleCallback)) {}

    bool isVisible() override
    {
        if (visibleCallback.has_value())
            return visibleCallback.value()();

        return true;
    }

    int getItemType() override
    {
        return itemType;
    }

    RealItemPath getRealItemPath() override
    {
        return realItemPath;
    }

    void* getItemPtr() override
    {
        return reinterpret_cast<void*>(itemPtr);
    }

    void createKey() override
    {
        luaItemKey.path = getLuaItemPath(path);
        luaItemKey.hash = getLuaItemKey(path);
    }

    fnv64Hash getLuaKey() override
    {
        return luaItemKey.hash;
    }

    std::string getLuaPath() override
    {
        return luaItemKey.path;
    }

private:
    itemPath path{};
    RealItemPath realItemPath{};
    LuaItemKey luaItemKey{};
    isVisibleFn visibleCallback{};
    T* itemPtr{};
    int itemType{};
};

}
#include <vector>
#include <iostream>

#include "luaManager.h"

#include "../gui/framework/uiItem.h"
#include "../gui/framework/itemsManager.h"

namespace lua
{
namespace metatables
{
constexpr auto UI = "ui";
}

using namespace gui::framework;

// "Ragebot", "Aimbot", "Main", "Enable"

// path concept for items are:
// 
// if ui contains subtabs:
//  "Tab", "Subtab", "Child", "ItemName"
//
// if not:
//  "Tab", "Main", "ItemName"
itemPath getLuaItemPath(lua_State* L, int argc)
{
    // due to fact that path will contain lua name
    // reserve one more string for it
    itemPath fullPath{};
    fullPath.reserve(argc + 1);

    // parse path expect last one (it will be item name)
    for (int i = 1; i <= argc - 1; i++)
    {
        if (lua_isstring(L, i))
            fullPath.emplace_back(lua_tostring(L, i));
    }

    return fullPath;
}

static int lua_CheckBoxSet(lua_State* L)
{
    lua_getfield(L, 1, "_ptr");

    BaseItem** ud = (BaseItem**)lua_touserdata(L, -1);
    bool value = lua_toboolean(L, 2);

    auto checkBoxPtr = reinterpret_cast<CheckBox*>((*ud)->getItemPtr());
    checkBoxPtr->item.value = value;

    return 0;
}

static int lua_CheckBoxGet(lua_State* L)
{
    lua_getfield(L, 1, "_ptr");
    BaseItem** ud = (BaseItem**)lua_touserdata(L, -1);

    auto checkBoxPtr = reinterpret_cast<CheckBox*>((*ud)->getItemPtr());
    lua_pushboolean(L, checkBoxPtr->item.value);

    return 1;
}

static int lua_CheckBoxSetVisible(lua_State* L)
{
    lua_getfield(L, 1, "_ptr");

    BaseItem** ud = (BaseItem**)lua_touserdata(L, -1);
    bool value = lua_toboolean(L, 2);
    (*ud)->setVisible(value);
   
    return 0;
}

static int lua_AddCheckBox(lua_State* L)
{
    int argc = lua_gettop(L);

    const auto itemName = lua_tostring(L, argc);
    auto item = getItemsManagerInstance().addCheckBox(itemName, getLuaItemPath(L, argc), IS_VISIBLE_DUMMY);

    lua_createtable(L, 0, 4);

    BaseItem** luaItemPtr = reinterpret_cast<BaseItem**>(lua_newuserdata(L, sizeof(BaseItem*)));
    *luaItemPtr = item.get();

    lua_setfield(L, -2, "_ptr");
    lua_pushcfunction(L, lua_CheckBoxSet);
    lua_setfield(L, -2, "set");
    lua_pushcfunction(L, lua_CheckBoxGet);
    lua_setfield(L, -2, "get");
    lua_pushcfunction(L, lua_CheckBoxSetVisible);
    lua_setfield(L, -2, "setVisible");

    return 1;
}

void createUiMetaTable(lua_State* L)
{
    struct luaL_Reg uiMethods[] = 
    {
        {"checkbox", lua_AddCheckBox},
    };

    if (luaL_newmetatable(L, metatables::UI))
    {
        luaL_setfuncs(L, uiMethods, 0);
        lua_pushliteral(L, "__index");
        lua_pushvalue(L, -2);
        lua_settable(L, -3);

        lua_pushliteral(L, "__metatable");
        lua_pushliteral(L, "must not access this metatable");
        lua_settable(L, -3);
    }
    lua_setglobal(L, metatables::UI);
}

void LuaState::init()
{
    state = luaL_newstate();
    luaL_openlibs(state);
    createUiMetaTable(state);
}

void LuaState::destroy()
{
    lua_close(state);
}

lua_State* LuaState::getState()
{
    return state;
}

void LuaState::executeScript(std::string path)
{
    setLoadedLuaName(path);
    luaL_dofile(state, path.c_str());
}

std::string LuaState::getLoadedLuaName()
{
    return luaName;
}

void LuaState::setLoadedLuaName(const std::string& name)
{
    luaName = name;
}

LuaState& getLuaStateInstance()
{
    static LuaState instance;
    return instance;
}
}
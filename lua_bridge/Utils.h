//
// Created by Alexander G. Pronchenkov on 27.12.2024.
//
#ifndef U7_LUA_BRIDGE_UTILS_H_
#define U7_LUA_BRIDGE_UTILS_H_

#include "lua.hpp"

#include <memory>

namespace u7::lua_bridge {

struct LuaStateCloser {
  void operator()(lua_State* L) const { lua_close(L); }
};

using LuaStatePtr = std::unique_ptr<lua_State, LuaStateCloser>;

LuaStatePtr MakeLuaState();

}  // namespace u7::lua_bridge

#endif  // U7_LUA_BRIDGE_UTILS_H_

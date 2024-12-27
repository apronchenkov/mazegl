#include "lua_bridge/Utils.h"

#include "lua.hpp"

namespace u7::lua_bridge {

LuaStatePtr MakeLuaState() {
  LuaStatePtr result(luaL_newstate());
  if (result == nullptr) {
    throw std::runtime_error("cannot create lua state");
  }
  return result;
}

}  // namespace u7::lua_bridge

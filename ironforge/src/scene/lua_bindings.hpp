#pragma once

#include <cstdint>
#include <lua.hpp>

namespace bindings {
    auto init(lua_State *L) -> int32_t;
} // namespace bindings

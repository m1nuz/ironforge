#pragma once

#include <cstdint>
#include <lua.hpp>

namespace bindings {
    auto init(scene::instance_t &sc, lua_State *L) -> int32_t;
} // namespace bindings

#include <scene/scene.hpp>
#include <core/game.hpp>
#include "physics.hpp"
#include "lua_bindings.hpp"

static int
set_entity_velocity(lua_State *L) {
    int32_t e = luaL_checkinteger(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);
    float z = luaL_checknumber(L, 4);

    auto &sc = game::get_current();
    auto body = sc->get_body(e);

    body->current.velocity = glm::vec3(x, y, z);

    return 0;
}

static int
get_entity_velocity(lua_State *L) {
    int32_t e = luaL_checkinteger(L, 1);

    auto &sc = game::get_current();
    auto body = sc->get_body(e);

    lua_pushnumber(L, body->current.velocity.x);
    lua_pushnumber(L, body->current.velocity.y);
    lua_pushnumber(L, body->current.velocity.z);

    return 3;
}

static const struct luaL_Reg scene_functions[] = {
    {"get_entity_velocity", get_entity_velocity},
    {"set_entity_velocity", set_entity_velocity},
    {NULL, NULL}
};

namespace bindings {
    auto init(lua_State *L) -> int32_t {
        luaL_newlib(L, scene_functions);
        lua_setglobal(L, "scene");

        return 0;
    }
} // namespace bindings

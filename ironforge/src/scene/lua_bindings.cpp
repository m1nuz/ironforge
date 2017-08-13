#include <core/journal.hpp>
#include <core/game.hpp>
#include <scene/scene.hpp>
#include <scene/instance.hpp>

#include "physics.hpp"
#include "lua_bindings.hpp"

static int
set_entity_velocity(lua_State *L) {
    int32_t e = luaL_checkinteger(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);
    float z = luaL_checknumber(L, 4);

    auto &sc = game::current_scene();
    auto body = sc->get_body(e);

    body->current.velocity = glm::vec3(x, y, z);

    return 0;
}

static int
get_entity_velocity(lua_State *L) {
    int32_t e = luaL_checkinteger(L, 1);

    auto &sc = game::current_scene();
    auto body = sc->get_body(e);

    lua_pushnumber(L, body->current.velocity.x);
    lua_pushnumber(L, body->current.velocity.y);
    lua_pushnumber(L, body->current.velocity.z);

    return 3;
}

static int
load_scene(lua_State *L) {
    using namespace game;

    const char *s = luaL_checkstring(L, 1);

    if (!game::load_scene(s))
        journal::error(journal::_SCENE, "Can't load scene %", s);

    return 0;
}

static int
load_asset(lua_State *L) {
    using namespace game;

    const char *s = luaL_checkstring(L, 1);

    if (!game::load_asset(s))
        journal::error(journal::_SCENE, "Can't load asset %", s);

    return 0;
}

static int
load_settings(lua_State *L) {
    using namespace game;

    const char *s = luaL_checkstring(L, 1);

    if (!game::load_settings(s))
        journal::error(journal::_SCENE, "Can't load settings %", s);

    return 0;
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

        lua_register(L, "load_scene", load_scene);
        lua_register(L, "load_asset", load_asset);
        lua_register(L, "load_settings", load_settings);

        return 0;
    }
} // namespace bindings

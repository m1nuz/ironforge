#include <core/journal.hpp>
#include <core/game.hpp>
#include <scene/scene.hpp>
#include <scene/instance.hpp>

#include "physics.hpp"
#include "lua_bindings.hpp"

static scene::instance_t* g_instance;

static int
set_entity_velocity(lua_State *L) {
    using namespace game;
    using namespace glm;

    const auto e = static_cast<uint32_t>(luaL_checkinteger(L, 1));

    const float x = luaL_checknumber(L, 2);
    const float y = luaL_checknumber(L, 3);
    const float z = luaL_checknumber(L, 4);

    journal::debug(journal::_SCENE, "% id=% x=% y=% z=%", __FUNCTION__, e, x, y, z);

    auto& body = g_instance->get_body(e);
    body.current.velocity = vec3{x, y, z};

    /*auto &sc = game::current_scene();
    auto body = sc->get_body(e);

    body->current.velocity = glm::vec3{x, y, z};*/

    return 0;
}

static int
get_entity_velocity(lua_State *L) {
    using namespace game;

    const auto e = static_cast<uint32_t>(luaL_checkinteger(L, 1));

    const auto body = g_instance->get_body(e);

    const auto x = body.current.velocity.x;
    const auto y = body.current.velocity.y;
    const auto z = body.current.velocity.z;

    journal::debug(journal::_SCENE, "% id=% x=% y=% z=%", __FUNCTION__, e, x, y, z);

    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    lua_pushnumber(L, z);

    /*auto &sc = game::current_scene();
    auto body = sc->get_body(e);

    const auto x = body->current.velocity.x;
    const auto y = body->current.velocity.y;
    const auto z = body->current.velocity.z;

    journal::debug(journal::_SCENE, "%1 id=%2 x=%3 y=%4 z=%5", __FUNCTION__, x, y, z);

    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    lua_pushnumber(L, z);*/

    return 3;
}

static const struct luaL_Reg scene_functions[] = {
    {"get_entity_velocity", get_entity_velocity},
    {"set_entity_velocity", set_entity_velocity},
    {NULL, NULL}
};

namespace bindings {
    auto init(scene::instance_t& sc, lua_State *L) -> int32_t {
        g_instance = &sc;

        luaL_newlib(L, scene_functions);
        lua_setglobal(L, "scene");

        //lua_register(L, "load_scene", load_scene);
        //lua_register(L, "load_asset", load_asset);
        //lua_register(L, "load_settings", load_settings);

        return 0;
    }
} // namespace bindings

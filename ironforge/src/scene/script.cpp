#include <core/application.hpp>
#include <core/assets.hpp>
#include <scene/scene.hpp>
#include <lua.hpp>

#include "script.hpp"
#include "lua_bindings.hpp"

namespace scene {
    static std::vector<script_instance> scripts;
    static lua_State *lua_state;

    auto call_fn(const script_instance *sc, const char *fn_name) -> int32_t {
        lua_State *L = lua_state;

        lua_getglobal(L, sc->table.c_str());

        if (lua_type(L, -1) != LUA_TTABLE) {
            application::error(application::log_category::scene, "% not found\n", sc->table.c_str());
            return -1;
        }

        lua_getfield(L, -1, fn_name);

        if (lua_type(L, -1) == LUA_TFUNCTION) {
            lua_pushvalue(L, -2);
            lua_pushinteger(L, sc->entity);

            if (lua_pcall(L, 2, 0, 0) != 0) {
                application::error(application::log_category::scene, "call function '%' : %\n", "_init", lua_tostring(L, -1));
                return -1;
            }
        }

        return 0;
    }

    static auto reset() -> int32_t {
        if (lua_state)
            lua_close(lua_state);

        if ((lua_state = luaL_newstate()) == nullptr) {
            application::error(application::log_category::scene, "%\n", "Can't init LUA");

            return -1;
        }

        luaL_openlibs(lua_state);
        bindings::init(lua_state);

        return 0;
    }

    auto init_all_scripts() -> void {
        scripts.reserve(max_scripts);

        reset();
    }

    auto cleanup_all_scripts() -> void {
        if (lua_state)
            lua_close(lua_state);
    }

    static void
    lua_clear_stack(lua_State *L) {
        int n = lua_gettop(L);
        lua_pop(L, n);
    }

    auto create_script(int32_t entity, const script_info &info) -> script_instance* {
        application::debug(application::log_category::scene, "Create script %\n", info.name);

        auto td = assets::get_text(info.name);

        auto L = lua_state;

        if (luaL_dostring(L, td.text)) {
            application::error(application::log_category::scene, "% %\n", "Could not load module", info.name);
            lua_close(L);
            return nullptr;
        }

        lua_clear_stack(L);

        char text[100];
        snprintf(text, sizeof text, "%s = M", info.table_name);

        if (luaL_dostring(L, text)) {
            application::error(application::log_category::scene, "%s\n", "Could set module");
            lua_close(L);
            return NULL;
        }

        lua_clear_stack(L);

        scripts.push_back({info.name, info.source, info.table_name, entity});

        return &scripts.back();
    }
} // namespace

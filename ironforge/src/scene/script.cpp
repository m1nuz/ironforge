#include <core/journal.hpp>
#include <core/assets.hpp>
#include <scene/scene.hpp>
#include <scene/entity.hpp>
#include <lua.hpp>

#include "script.hpp"
#include "lua_bindings.hpp"

namespace scene {
    static std::vector<script_instance> scripts;
    lua_State *lua_state;

    inline void push() {
    }

    inline void push(const int value) {
        lua_pushinteger(lua_state, value);
    }

    inline void push(const double value) {
        lua_pushnumber(lua_state, value);
    }

    inline void push(const float value) {
        lua_pushnumber(lua_state, value);
    }

    template <typename T, typename... Ts>
    inline void push(const T&& value, const Ts&&... values) {
        push(value);
        push(values...);
    }

    template<typename Arg, typename... Args>
    auto call_with_args(const script_instance *sc, const char *fn_name,  Arg&& arg, Args&&... args) -> int32_t {
        const int num_args = sizeof...(Args);

        lua_State *L = lua_state;

        lua_getglobal(L, sc->table.c_str());

        if (lua_type(L, -1) != LUA_TTABLE) {
            game::journal::error(game::journal::_SCENE, "% not found", sc->table.c_str());
            return -1;
        }

        lua_getfield(L, -1, fn_name);

        if (lua_type(L, -1) == LUA_TFUNCTION) {
            lua_pushvalue(L, -2);
            lua_pushinteger(L, sc->entity);

            push(arg, std::forward<Args>(args)...);

            if (lua_pcall(L, num_args + 3, 0, 0) != 0) {
                game::journal::error(game::journal::_SCENE, "call function '%' : %", "_init", lua_tostring(L, -1));
                return -1;
            }
        }

        return 0;
    }

    auto call_fn(const script_instance *sc, const char *fn_name) -> int32_t {
        if (!sc)
            return -1;

        lua_State *L = lua_state;

        lua_getglobal(L, sc->table.c_str());

        if (lua_type(L, -1) != LUA_TTABLE) {
            game::journal::error(game::journal::_SCENE, "% not found", sc->table.c_str());
            return -1;
        }

        lua_getfield(L, -1, fn_name);

        if (lua_type(L, -1) == LUA_TFUNCTION) {
            lua_pushvalue(L, -2);
            lua_pushinteger(L, sc->entity);

            if (lua_pcall(L, 2, 0, 0) != 0) {
                game::journal::error(game::journal::_SCENE, "call function '%' : %", "_init", lua_tostring(L, -1));
                return -1;
            }
        }

        return 0;
    }

    static auto reset() -> int32_t {
        if (lua_state)
            lua_close(lua_state);

        if ((lua_state = luaL_newstate()) == nullptr) {
            game::journal::error(game::journal::_SCENE, "%", "Can't init LUA");

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

    auto update_all_scripts(const float dt) -> void {
        for (const auto &s : scripts)
            if (s.flags & static_cast<uint32_t>(entity_flags::call_update))
                call_with_args(&s, "_update", dt);
    }

    static void
    lua_clear_stack(lua_State *L) {
        int n = lua_gettop(L);
        lua_pop(L, n);
    }

    auto create_script(int32_t entity, const script_info &info, uint32_t flags) -> script_instance* {
        game::journal::debug(game::journal::_SCENE, "Create script %", info.name);

        auto td = assets::get_text(info.name);

        auto L = lua_state;

        if (luaL_dostring(L, td.text)) {
            game::journal::error(game::journal::_SCENE, "% %", "Could not load module", info.name);
            lua_close(L);
            return nullptr;
        }

        lua_clear_stack(L);

        char text[100];
        snprintf(text, sizeof text, "%s = M", info.table_name);

        if (luaL_dostring(L, text)) {
            game::journal::error(game::journal::_SCENE, "%s", "Could set module");
            lua_close(L);
            return NULL;
        }

        lua_clear_stack(L);

        scripts.push_back({info.name, info.source, info.table_name, entity, flags});

        if (flags & static_cast<uint32_t>(entity_flags::call_init))
            call_fn(&scripts.back(), "_init");

        return &scripts.back();
    }

    auto do_script(const std::string &name) -> bool {
        auto td = assets::get_text_absolute(name);

        auto L = lua_state;

        if (td.size != 0) {
            if (luaL_dostring(L, td.text)) {
                game::journal::error(game::journal::_SCENE, "% %", "Could not load script", name);
                lua_close(L);
                return false;
            }
        } else
            return false;

        return true;
    }

    template auto call_with_args<int>(const script_instance *, const char *, int&&) -> int32_t;
    template auto call_with_args<float>(const script_instance *, const char *, float&&) -> int32_t;
} // namespace

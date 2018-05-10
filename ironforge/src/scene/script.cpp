#include <core/journal.hpp>
#include <core/assets.hpp>
#include <scene/scene.hpp>
#include <lua.hpp>

#include "script.hpp"
#include "lua_bindings.hpp"

namespace scene {
    static lua_State *lua_state;

    static void
    lua_clear_stack(lua_State *L) {
        int n = lua_gettop(L);
        lua_pop(L, n);
    }

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

        if (!L)
            return -1;

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

        lua_clear_stack(L);

        return 0;
    }

    auto call_fn(const script_instance *sc, const char *fn_name) -> int32_t {
        using namespace game;

        if (!sc)
            return -1;

        lua_State *L = lua_state;

        if (sc->table.empty()) {
            journal::error(journal::_SCENE, "%", "No class name");

            return -1;
        }

        lua_getglobal(L, sc->table.c_str());

        if (lua_type(L, -1) != LUA_TTABLE) {
            journal::error(journal::_SCENE, "% not found", sc->table.c_str());
            return -1;
        }

        lua_getfield(L, -1, fn_name);

        if (lua_type(L, -1) == LUA_TFUNCTION) {
            lua_pushvalue(L, -2);
            lua_pushinteger(L, sc->entity);

            if (lua_pcall(L, 2, 0, 0) != 0) {
                journal::error(journal::_SCENE, "call function '%' : %", "_init", lua_tostring(L, -1));
                return -1;
            }
        }

        return 0;
    }

    auto reset_scripts_engine() -> bool {
        using namespace game;

        if (lua_state)
            lua_close(lua_state);

        if ((lua_state = luaL_newstate()) == nullptr) {
            journal::error(journal::_SCENE, "%", "Can't init LUA");

            return false;
        }

        luaL_openlibs(lua_state);

        return true;
    }

    auto setup_bindings(instance_t &sc) -> void {
        bindings::init(sc, lua_state);
    }

    auto create_script(assets::instance_t &asset, const uint32_t entity, const json &info) -> std::optional<script_instance> {
        using namespace std;
        using namespace game;

        const auto name = info["name"].get<string>();
        const auto source = name;
        const auto class_name = info["class"].get<string>();

        journal::debug(journal::_SCENE, "Create script %", name);

        auto text_data = assets::get_text(asset, name);

        auto L = lua_state;

        if (!L) {
            journal::critical(journal::_SCENE, "%", "Lua VM not accessable");

            return {};
        }

        if (!text_data || luaL_dostring(L, text_data.value().c_str())) {
            journal::error(journal::_SCENE, "% %", "Could not load module", name);
            lua_close(L);

            return {};
        }

        lua_clear_stack(L);

        char text[100];
        snprintf(text, sizeof text, "%s = M", class_name.c_str());

        if (luaL_dostring(L, text)) {
            journal::error(journal::_SCENE, "%s", "Could set module");
            lua_close(L);

            return {};
        }

        lua_clear_stack(L);

        script_instance si;
        si.entity = entity;
        si.name = name;
        si.source = source;
        si.table = class_name;

        return si;
    }

    auto update_all_scripts(instance_t &sc, const float dt) -> void {
        for (const auto &[ix, s] : sc.scripts) {
            (void)ix;
            if (s.flags & static_cast<uint32_t>(script_flags::call_update))
                call_with_args(&s, "_update", dt);
        }
    }

    template auto call_with_args<int>(const script_instance *, const char *, int&&) -> int32_t;
    template auto call_with_args<float>(const script_instance *, const char *, float&&) -> int32_t;
} // namespace

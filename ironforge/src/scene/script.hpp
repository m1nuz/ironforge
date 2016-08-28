#pragma once

#include <cstdint>
#include <string>

#include <lua.hpp>

namespace scene {
    struct script_instance {
        std::string name;
        std::string source;
        std::string table;
        int32_t     entity;
        //script_type type; but now only Lua supported
    };

    struct script_info {
        const char  *name;
        const char  *source;
        const char  *table_name;
    };

    template<typename Arg, typename... Args>
    auto call_with_args(const script_instance *sc, const char *fn_name,  Arg&& arg, Args&&... args) -> int32_t;

    auto call_fn(const script_instance *sc, const char *fn_name) -> int32_t;

    auto init_all_scripts() -> void;
    auto cleanup_all_scripts() -> void;

    auto create_script(int32_t entity, const script_info &info) -> script_instance*;

    auto do_script(const std::string &name) -> bool;
} // namespace scene

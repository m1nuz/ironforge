#pragma once

#include <cstdint>
#include <string>

namespace scene {
    struct script_instance {
        std::string name;
        std::string source;
        std::string table;
        int32_t     entity;
    };

    struct script_info {
        const char  *name;
        const char  *source;
        const char  *table_name;
    };

    auto call_fn(const script_instance *sc, const char *fn_name) -> int32_t;

    auto init_all_scripts() -> void;
    auto cleanup_all_scripts() -> void;

    auto create_script(int32_t entity, const script_info &info) -> script_instance*;
} // namespace scene

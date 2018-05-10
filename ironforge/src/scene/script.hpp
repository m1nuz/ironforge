#pragma once

#include <cstdint>
#include <string>
#include <optional>

#include <core/json.hpp>

namespace assets {
    struct instance_type;
    typedef instance_type instance_t;
}

namespace scene {
    enum class script_flags : uint32_t {
        call_init,
        call_done,
        call_update
    };

    inline script_flags operator |(script_flags a, script_flags b) {
        return static_cast<script_flags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    struct script_instance {
        std::string name;
        std::string source;
        std::string table;
        uint32_t    entity;
        uint32_t    flags;

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

    struct instance_type;
    typedef instance_type instance_t;

    auto reset_scripts_engine() -> bool;
    auto setup_bindings(instance_t &sc) -> void;
    auto create_script(assets::instance_t &asset, const uint32_t entity, const json &info) -> std::optional<script_instance>;

    auto update_all_scripts(instance_t &sc, const float dt) -> void;
} // namespace scene


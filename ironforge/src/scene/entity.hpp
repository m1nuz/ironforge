#pragma once

#include <core/json.hpp>

namespace assets {
    struct instance_type;
    typedef instance_type instance_t;
}

namespace scene {
    struct instance_type;
    typedef instance_type instance_t;

    auto create_entity(assets::instance_t &asset, instance_t &sc, const json &info) -> uint32_t;
    auto find_entity(instance_t &sc, const std::string &name) -> uint32_t;
} // namespace scene

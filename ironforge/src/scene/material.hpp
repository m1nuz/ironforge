#pragma once

#include <vector>
#include <string>
#include <optional>

#include <core/common.hpp>
#include <core/json.hpp>
#include <video/video.hpp>
#include <renderer/renderer.hpp>

namespace assets {
    struct instance_type;
    typedef instance_type instance_t;
}

namespace video {
    struct instance_type;
    typedef instance_type instance_t;
}

namespace scene {
    struct material_instance {
        renderer::phong::material m0; // TODO: array of materials?
        std::string name = {};
        uint64_t name_hash = 0;
    };

    auto create_material(video::instance_t &vi, const json &info) -> std::optional<material_instance>;
} // namespace scene

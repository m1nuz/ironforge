#pragma once

#include <vector>
#include <string>
#include <core/common.hpp>
#include <video/video.hpp>
#include <renderer/renderer.hpp>

#include <optional>

#include <json.hpp>

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

    using json = nlohmann::json;
    auto create_material(video::instance_t &vi, const json &info) -> std::optional<material_instance>;
} // namespace scene

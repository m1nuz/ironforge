#pragma once

#include <string>
#include <variant>
#include <optional>
#include <functional>

#include <core/common.hpp>
#include <core/json.hpp>
#include <renderer/renderer.hpp>

namespace scene {
    struct instance_type;
    typedef instance_type instance_t;

    using light_instance = std::variant<renderer::phong::ambient_light, renderer::phong::directional_light, renderer::phong::point_light>;
    using light_ref = std::reference_wrapper<light_instance>;

    auto create_light(const json &info) -> std::optional<light_instance>;
    auto present_all_lights(instance_t& sc, std::unique_ptr<renderer::instance> &renderer) -> void;
} // namespace scene

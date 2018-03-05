#pragma once

#include <string>
#include <variant>
#include <optional>

#include <core/common.hpp>
#include <renderer/renderer.hpp>

#include <json.hpp>

namespace scene {
    struct instance_type;
    typedef instance_type instance_t;

    enum class light_type : uint32_t {
        unknown,
        ambient,
        directional,
        point,
        //spot
    };

    struct ambient_light_info {
        glm::vec3 ambient;
    };

    struct directional_light_info {
        glm::vec3 direction;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };

    struct point_light_info {
        glm::vec3 diffuse;
        glm::vec3 specular;
        glm::vec3 attenuation; // Constant, Linear, Quadratic
    };

    struct spot_light_info {

    };

    struct light_info {
        light_type              type;
        ambient_light_info      ambient_light;
        directional_light_info  directional_light;
        point_light_info        point_light;
        spot_light_info         spot_light;
    };

    using ambient_light_instance = renderer::phong::ambient_light;
    using directional_light_instance = renderer::phong::directional_light;

    /*struct ambient_light_instance : public renderer::phong::ambient_light {
        std::string     name;
        uint64_t        name_hash;
    };

    struct directional_light_instance : public renderer::phong::directional_light {
        std::string     name;
        uint64_t        name_hash;
    };*/

    struct point_light_instance : public renderer::phong::point_light {
        point_light_instance() = default;
        point_light_instance(const glm::vec3 &d, const glm::vec3 &s, const glm::vec3 &a, int32_t _entity) : renderer::phong::point_light{d, s, a}, entity{_entity} {

        }

        int32_t         entity;
    };

    typedef std::variant<renderer::phong::ambient_light, renderer::phong::directional_light, renderer::phong::point_light> light_t;

    using json = nlohmann::json;

    auto create_light(const json &info) -> std::optional<light_t>;
    auto present_all_lights(instance_t& sc, std::unique_ptr<renderer::instance> &renderer) -> void;
} // namespace scene

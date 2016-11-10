#pragma once

#include <string>
#include <ironforge_common.hpp>
#include <video/video.hpp>
#include <renderer/renderer.hpp>

namespace scene {
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

    auto init_all_lights() -> void;
    auto cleanup_all_lights() -> void;
    auto create_light(int32_t entity, const light_info &info) -> std::pair<light_type, void*>;
    auto present_all_lights(std::unique_ptr<instance>& s, std::unique_ptr<renderer::instance> &r) -> void;
} // namespace scene

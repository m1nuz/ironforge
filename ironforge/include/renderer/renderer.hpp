#pragma once

#include <memory>
#include <ironforge_common.hpp>

namespace renderer {
    namespace phong {
        struct material {
            glm::vec3   ke; // emission
            glm::vec3   ka;
            glm::vec3   kd;
            glm::vec3   ks;
            float       ns;
            float       tr;
            //texture *emission_map;
            //texture *diffuse_map;
            //texture *specular_map;
            //texture *gloss_map;
        };

        struct ambient_light {
            glm::vec3   la; // Ambient intensity
        };

        struct point_light {
            glm::vec3  ld; // Diffuse intensity
            glm::vec3  ls; // Specular intensity
            glm::vec3  attenuation; // Constant, Linear, Quadratic
        };

        struct directional_light {
            glm::vec3  direction;
            glm::vec3  Ld;
            glm::vec3  Ls;
        };
    } // namespace phong

    struct instance {
        // interface
        //virtual auto set(const phong::ambient_light &light) -> void = 0;
        //virtual auto set(const phong::directional_light &light) -> void = 0;
        //virtual auto append(const phong::point_light &light) -> void = 0;
        virtual ~instance() = default;
    };

    auto create_null_renderer() -> std::unique_ptr<instance>;
    //auto create_forward_renderer() -> std::unique_ptr<instance>;
    //auto create_deffered_renderer() -> std::unique_ptr<instance>;
} // namespace renderer

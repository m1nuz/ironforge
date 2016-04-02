#pragma once

#include <memory>
#include <ironforge_common.hpp>
#include <video/video.hpp>
#include <video/vertices.hpp>

namespace renderer {
    namespace phong {
        struct material {
            glm::vec3   ke; // emission
            glm::vec3   ka;
            glm::vec3   kd;
            glm::vec3   ks;
            float       ns;
            float       tr;

            video::texture emission_tex;
            video::texture diffuse_tex;
            video::texture specular_tex;
            video::texture gloss_tex;
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
            glm::vec3  ld;
            glm::vec3  ls;
        };
    } // namespace phong

    // TODO: think about static dispatching
    struct instance {
        // interface
        // virtual auto is_phong_spported() -> bool = 0;
        virtual auto append(const phong::ambient_light &light) -> void = 0;
        virtual auto append(const phong::directional_light &light) -> void = 0;
        virtual auto append(const phong::point_light &light) -> void = 0;
        virtual auto append(const phong::material &material) -> void = 0;
        virtual auto append(const video::vertices_source &source, const video::vertices_draw &draw) -> void = 0;
        virtual auto append(const glm::mat4 &model) -> void = 0;
        // TODO: think about add/remove from ubo mechanism

        virtual auto reset() -> void = 0;
        virtual auto present(const glm::mat4 &proj, const glm::mat4 &view) -> void = 0;        

        virtual ~instance() = default;
    };

    auto create_null_renderer() -> std::unique_ptr<instance>;
    auto create_forward_renderer() -> std::unique_ptr<instance>;
    //auto create_deffered_renderer() -> std::unique_ptr<instance>;
} // namespace renderer

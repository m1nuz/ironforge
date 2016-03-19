#pragma once

#include <vector>

#include <video/video.hpp>
#include <core/application.hpp>
#include <renderer/renderer.hpp>

namespace renderer {
    // limits
    constexpr size_t max_ambient_lights     = 1;
    constexpr size_t max_directional_lights = 1;
    constexpr size_t max_point_lights       = 10;
    constexpr size_t max_materials          = 10;
    constexpr size_t max_draws              = 10;
    constexpr size_t max_matrices           = 10;
    constexpr size_t max_sources            = 10;

    struct row_material {
        glm::vec4 values[4];
    };

    // TODO : make video api specific template
    struct forward_renderer : public instance {
        forward_renderer();
        ~forward_renderer();

        virtual auto append(const phong::ambient_light &light) -> void;
        virtual auto append(const phong::directional_light &light) -> void;
        virtual auto append(const phong::point_light &light) -> void;
        virtual auto append(const phong::material &material) -> void;
        virtual auto append(const video::vertices_source &source, const video::vertices_draw &draw) -> void;
        virtual auto append(const glm::mat4 &model) -> void;

        virtual auto reset() -> void;
        virtual auto present(const glm::mat4 &proj, const glm::mat4 &view) -> void;

        std::vector<video::vertices_source>     sources;
        std::vector<video::vertices_draw>       draws;
        std::vector<glm::mat4>                  matrices;
        std::vector<phong::material>            materials;

        std::vector<phong::ambient_light>       ambient_lights;
        std::vector<phong::directional_light>   directional_lights;
        std::vector<phong::point_light>         point_lights;

        video::gl::sampler                      texture_sampler;

        video::gl::command_buffer               prepare_commands;
        video::gl::command_buffer               post_commands;
        video::gl::command_buffer               ambient_commands;
        video::gl::command_buffer               directional_commands;
        video::gl::command_buffer               point_commands;
        video::gl::command_buffer               transparent_commands;
        video::gl::command_buffer               glow_commands;

        video::gl::program                      emission_shader;
        video::gl::program                      ambient_light_shader;
        video::gl::program                      directional_light_shader;
        video::gl::program                      point_light_shader;
        video::gl::program                      postprocess_shader;
        video::gl::program                      filter_vblur_shader;
        video::gl::program                      filter_hblur_shader;
    };
} // namespace renderer

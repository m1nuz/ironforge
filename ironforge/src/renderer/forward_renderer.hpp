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
    constexpr size_t max_drawables          = 10;

    struct forward_renderer : public instance {
        forward_renderer();
        ~forward_renderer();

        virtual auto append(const phong::ambient_light &light) -> void;
        virtual auto append(const phong::directional_light &light) -> void;
        virtual auto append(const phong::point_light &light) -> void;
        virtual auto append(const phong::material &material) -> void;

        virtual auto reset() -> void;
        virtual auto present(const glm::mat4 &proj, const glm::mat4 &view) -> void;

        std::vector<phong::ambient_light>       ambient_lights;
        std::vector<phong::directional_light>   directional_lights;
        std::vector<phong::point_light>         point_lights;
        std::vector<phong::material>            materials;

        video::gl::command_buffer               prepare_commands;
        video::gl::command_buffer               post_commands;
        video::gl::command_buffer               ambient_commands;
        video::gl::command_buffer               directional_commands;
        video::gl::command_buffer               point_commands;
        video::gl::command_buffer               transparent_commands;
        video::gl::command_buffer               glow_commands;
    };
} // namespace renderer

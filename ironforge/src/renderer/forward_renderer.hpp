#pragma once

#include <vector>

#include <core/json.hpp>
#include <video/video.hpp>
#include <video/atlas.hpp>
#include <video/sprite_batch.hpp>
#include <video/triangles_batch.hpp>
#include <renderer/renderer.hpp>

namespace renderer {
    // limits
    constexpr size_t max_ambient_lights     = 1;
    constexpr size_t max_directional_lights = 1;
    constexpr size_t max_point_lights       = 20;
    constexpr size_t max_materials          = 20;
    constexpr size_t max_draws              = 20;
    constexpr size_t max_matrices           = 20;
    constexpr size_t max_sources            = 20;

    struct raw_material {
        glm::vec4 values[4];
    };

    // TODO : make video api specific template
    struct forward_renderer : public instance {
        forward_renderer(video::instance_t &vi, const json &info);
        ~forward_renderer();

        virtual auto append(const phong::ambient_light &light) -> void;
        virtual auto append(const phong::directional_light &light) -> void;
        virtual auto append(const phong::point_light &light) -> void;
        virtual auto append(const phong::material &material) -> void;
        virtual auto append(const video::vertices_source &source, const video::vertices_draw &draw) -> void;
        virtual auto append(const glm::mat4 &model) -> void;
        virtual auto append(const video::texture &cubemap, uint32_t flags) -> void;

        virtual auto dispath(video::instance_t &vi, const ui::draw_command_t &c) -> void override;

        virtual auto reset() -> void;
        virtual auto present(video::instance_t &vi, const glm::mat4 &proj, const glm::mat4 &view) -> void;

        auto draw_text(const video::font_t &font, float _x, float _y, float _w, float _h, const std::string &text, uint32_t _align, uint32_t _color) -> void;
        auto draw_line(float _x0, float _y0, float _x1, float _y1, float _w, uint32_t _color) -> void;
        auto draw_rect(const float _x, const float _y, const float _w, const float _h, const uint32_t _color) -> void;

        float                                   aspect_ratio;
        float                                   display_width;
        float                                   display_height;

        std::vector<video::vertices_source>     sources;
        std::vector<video::vertices_draw>       draws; // TODO: use raw_draw
        std::vector<glm::mat4>                  matrices;
        std::vector<phong::material>            materials;

        std::vector<phong::ambient_light>       ambient_lights;
        std::vector<phong::directional_light>   directional_lights;
        std::vector<phong::point_light>         point_lights;
        //std::vector<phong::spot_light>        spot_lights;

        video::gl::sampler                      texture_sampler;
        video::gl::sampler                      filter_sampler;

        video::gl::texture                      color_map;
        video::gl::texture                      depth_map;
        video::gl::texture                      glow_map;
        video::gl::texture                      blur_map;

        video::gl::renderbuffer                 sample_color;
        video::gl::renderbuffer                 sample_depth;
        video::gl::renderbuffer                 blur_depth;

        video::texture                          skybox_map;
        video::texture                          glyphs_map;

        video::gl::framebuffer                  sample_framebuffer;
        video::gl::framebuffer                  color_framebuffer;
        video::gl::framebuffer                  glow_framebuffer;
        video::gl::framebuffer                  blur_framebuffer;

        video::vertices_source                  fullscreen_quad;
        video::vertices_draw                    fullscreen_draw;

        video::vertices_source                  skybox_cube;
        video::vertices_draw                    skybox_draw;

        video::gl::command_buffer               prepare_commands;
        video::gl::command_buffer               post_commands;
        video::gl::command_buffer               ambient_commands;
        video::gl::command_buffer               directional_commands;
        //video::gl::command_buffer               point_commands;
        //video::gl::command_buffer               transparent_commands;
        video::gl::command_buffer               glow_commands;
        video::gl::command_buffer               skybox_commands;
        video::gl::command_buffer               terrain_commands;

        video::program                          emission_shader;
        video::program                          ambient_light_shader;
        video::program                          directional_light_shader;
        video::program                          point_light_shader;
        video::program                          postprocess_shader;
        video::program                          filter_vblur_shader;
        video::program                          filter_hblur_shader;
        video::program                          skybox_shader;

        video::sprite_batch                     sprites;
        video::program                          sprite_shader;
        video::triangles_batch                  triangles;
    };
} // namespace renderer

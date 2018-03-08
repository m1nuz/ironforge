#include <video/screen.hpp>
#include <video/commands.hpp>
#include <video/glyphs.hpp>
#include <video/screen.hpp>
#include <core/journal.hpp>
#include <core/settings.hpp>
#include "forward_renderer.hpp"

#include <GL/ext_texture_filter_anisotropic.h>

namespace renderer {
    forward_renderer::forward_renderer(video::instance_t &vi, const json &info) {
        game::journal::debug(game::journal::_RENDER, "% % with % %", "Create forward render", "version 1.00", video::gl::api_name, video::gl::api_version);

        emission_shader = video::get_shader(vi, "emission-shader");
        ambient_light_shader = video::get_shader(vi, "ambient-light-shader");
        directional_light_shader = video::get_shader(vi, "forward-directional-shader");
        postprocess_shader = video::get_shader(vi, "postprocess-shader");
        filter_vblur_shader = video::get_shader(vi, "vblur-shader");
        filter_hblur_shader = video::get_shader(vi, "hblur-shader");
        skybox_shader = video::get_shader(vi, "skybox-shader");
        sprite_shader = video::get_shader(vi, "sprite-shader");

        video::gl::sampler_info sam_info;

        switch (vi.texture_filter) {
        case video::texture_filtering::bilinear:
            sam_info.mag_filter = video::gl::texture_mag_filter::linear;
            sam_info.min_filter = video::gl::texture_min_filter::linear;
            break;
        case video::texture_filtering::trilinear:
            sam_info.mag_filter = video::gl::texture_mag_filter::linear;
            sam_info.min_filter = video::gl::texture_min_filter::linear_mipmap_linear;
            break;
        case video::texture_filtering::anisotropic:
        case video::texture_filtering::max_filtering:
            sam_info.mag_filter = video::gl::texture_mag_filter::linear;
            sam_info.min_filter = video::gl::texture_min_filter::linear_mipmap_linear;
            sam_info.anisotropy = vi.max_supported_anisotropy;
            break;
        }

        texture_sampler = video::gl::create_sampler(sam_info);

        video::gl::sampler_info filter_info;
        filter_info.min_filter = video::gl::texture_min_filter::linear;
        filter_info.mag_filter = video::gl::texture_mag_filter::linear;
        filter_info.wrap_r = video::gl::texture_wrap::clamp_to_edge;
        filter_info.wrap_s = video::gl::texture_wrap::clamp_to_edge;
        filter_sampler = video::gl::create_sampler(filter_info);

        const auto ratio = 2;
        const auto samples = info.find("fsaa") != info.end() ? info["fsaa"].get<uint32_t>() : 0u;
        const auto w = static_cast<uint32_t>(vi.w);
        const auto h = static_cast<uint32_t>(vi.h);

        color_map = video::gl::create_texture_2d({video::pixel_format::rgba16f, 0, 0, w, h, 0, {}});
        depth_map = video::gl::create_texture_2d({video::pixel_format::depth, 0, 0, w, h, 0, {}});
        glow_map = video::gl::create_texture_2d({video::pixel_format::rgb16f, 0, 0, w / ratio, h / ratio, 0, {}});
        blur_map = video::gl::create_texture_2d({video::pixel_format::rgb16f, 0, 0, w / ratio, h / ratio, 0, {}});

        blur_depth = video::gl::create_renderbuffer({video::pixel_format::depth, w / ratio, h/ ratio, 0});
        sample_color = video::gl::create_renderbuffer({video::pixel_format::rgb16f, w, h, samples});
        sample_depth = video::gl::create_renderbuffer({video::pixel_format::depth, w, h, samples});

        memset(&skybox_map, 0, sizeof skybox_map);

        uint32_t mask = static_cast<uint32_t>(video::gl::framebuffer_mask::color_buffer) | static_cast<uint32_t>(video::gl::framebuffer_mask::depth_buffer);

        using namespace video;

        color_framebuffer = gl::create_framebuffer({w, h, mask, {{gl::framebuffer_attachment::color0, gl::framebuffer_attachment_target::texture, color_map.id},
                                                                                        {gl::framebuffer_attachment::depth, gl::framebuffer_attachment_target::texture, depth_map.id}}});

        glow_framebuffer = gl::create_framebuffer({w / ratio, h / ratio, mask, {{gl::framebuffer_attachment::color0, gl::framebuffer_attachment_target::texture, glow_map.id},
                                                                                       {gl::framebuffer_attachment::depth, gl::framebuffer_attachment_target::renderbuffer, blur_depth.id}}});

        blur_framebuffer = gl::create_framebuffer({w / ratio, h / ratio, mask, {{gl::framebuffer_attachment::color0, gl::framebuffer_attachment_target::texture, blur_map.id}}});

        sample_framebuffer = gl::create_framebuffer({w, h, mask, {{gl::framebuffer_attachment::color0, gl::framebuffer_attachment_target::renderbuffer, sample_color.id},
                                                                                         {gl::framebuffer_attachment::depth, gl::framebuffer_attachment_target::renderbuffer, sample_depth.id}}});

        auto quad_vi = video::vertgen::make_plane(glm::mat4{1.f});
        std::vector<vertices_draw> quad_vd;
        fullscreen_quad = video::make_vertices_source(vi, {quad_vi.data}, quad_vi.desc, quad_vd);
        fullscreen_draw = quad_vd[0];

        auto cube_vi = video::vertgen::make_cube(glm::mat4{1.f});
        std::vector<vertices_draw> cube_vd;
        skybox_cube = video::make_vertices_source(vi, {cube_vi.data}, cube_vi.desc, cube_vd);
        skybox_draw = cube_vd[0];

        sprite_batch_info sb_info;
        sb_info.max_sprites = 1000;
        sb_info.tex = video::get_texture(vi, "white-map");
        sb_info.tex = video::get_texture(vi, "glyphs-map");
        sprites = video::create_sprite_batch(vi, sb_info);

        triangles_batch_info tb_info;
        tb_info.max_triangles = 1000;
        tb_info.tex = video::get_texture(vi, "glyphs-map");

        triangles = video::create_triangles_batch(vi, tb_info);

        sources.reserve(max_sources);
        draws.reserve(max_draws);
        matrices.reserve(max_matrices);
        materials.reserve(max_materials);

        reset();
    }

    forward_renderer::~forward_renderer() {
        video::delete_sprite_batch(sprites);

        video::gl::destroy_framebuffer(color_framebuffer);
        video::gl::destroy_texture(color_map);
        video::gl::destroy_texture(depth_map);
        video::gl::destroy_texture(glow_map);
        video::gl::destroy_texture(blur_map);
        video::gl::destroy_renderbuffer(blur_depth);
        video::gl::destroy_renderbuffer(sample_color);
        video::gl::destroy_renderbuffer(sample_depth);
        video::gl::destroy_sampler(texture_sampler);
        video::gl::destroy_sampler(filter_sampler);
        game::journal::debug(game::journal::_RENDER, "%", "Destroy forward render");
    }

    auto forward_renderer::append(const phong::ambient_light &light) -> void {
        ambient_lights.push_back(light);
    }

    auto forward_renderer::append(const phong::directional_light &light) -> void {
        directional_lights.push_back(light);
    }

    auto forward_renderer::append(const phong::point_light &light) -> void {
        point_lights.push_back(light);
    }

    auto forward_renderer::append(const phong::material &material) -> void {
        // TODO: make row vector
        materials.push_back(material);
    }

    auto forward_renderer::append(const video::vertices_source &source, const video::vertices_draw &draw) -> void {
        sources.push_back(source);
        draws.push_back(draw);
    }

    auto forward_renderer::append(const glm::mat4 &model) -> void {
        matrices.push_back(model);
    }

    auto forward_renderer::append(const video::texture &cubemap, uint32_t flags) -> void {
        UNUSED(flags);

        // TODO: add flag for special case
        skybox_map = cubemap;
    }

    /*auto forward_renderer::append(const video::font_t &font, const std::string &text, const glm::vec2 &pos, const glm::vec4 &color) -> void {
        using namespace glm;

        const auto tw = 512.f;
        const auto th = 512.f;

        const int adv_y = font.lineskip;
        const int fh = font.size;
        const float spt = 2.f / video::screen.width;

        vec2 p = pos;

        for (const auto &c : text) {
            if (c == '\n') {
                p[0] = pos[0];
                p[1] -= spt * adv_y * video::screen.aspect;
                continue;
            }

            auto glyph = video::glyph_cache_find(c, font);
            if (glyph.ch == 0)
            {
                game::journal::warning(game::journal::_RENDER, "%", "Glyph not found");
                continue;
            }

            vec2 size = {glyph.advance * spt, fh * spt};
            vec4 offset;
            offset[0] = (float)glyph.rc.x / tw;
            offset[1] = (float)glyph.rc.y / th;
            offset[2] = (float)glyph.rc.w / tw;
            offset[3] = (float)glyph.rc.h / th;

            //video::append_sprite(sprites, vec3{p, 0}, size, offset, color);

            const float correction = video::screen.aspect;

            const video::v3t2c4 vertices[6] = {
                {{p[0], p[1] + size[1] * correction, 0}, {offset[0], offset[1]}, {color[0], color[1], color[2], color[3]}},
                {{p[0] + size[0], p[1] + size[1] * correction, 0}, {offset[0] + offset[2], offset[1]}, {color[0], color[1], color[2], color[3]}},
                {{p[0] + size[0], p[1], 0}, {offset[0] + offset[2], offset[1] + offset[3]}, {color[0], color[1], color[2], color[3]}},
                {{p[0] + size[0], p[1], 0}, {offset[0] + offset[2], offset[1] + offset[3]}, {color[0], color[1], color[2], color[3]}},
                {{p[0], p[1], 0}, {offset[0], offset[1] + offset[3]}, {color[0], color[1], color[2], color[3]}},
                {{p[0], p[1] + size[1] * correction, 0}, {offset[0], offset[1]}, {color[0], color[1], color[2], color[3]}},
            };

            video::append_triangles_vertices(triangles, vertices, 6);

            p[0] += glyph.advance * spt;
        }
    }*/

    auto forward_renderer::dispath(const ui::command &c) -> void {
        using namespace ui;

        switch (c.type) {
        case command_type::line:
            draw_line(c.line);
            break;
        case command_type::rect:
            draw_rect(c.rect);
            break;
        case command_type::rounded_rect:
            draw_roundrect(c.roundrect);
            break;
        case command_type::text:
            draw_text(c.text);
            break;
        case command_type::icon:
            draw_icon(c.icon);
            break;
        }
    }

    auto forward_renderer::reset() -> void {
        sources.clear();
        materials.clear();
        matrices.clear();
        draws.clear();

        ambient_lights.clear();
        directional_lights.clear();
        point_lights.clear();

        prepare_commands.clear_color = glm::vec4(0.7f, 0.7f, 0.7f, 0.f);
        prepare_commands.memory_offset = 0;
        prepare_commands.commands.clear();
        prepare_commands.depth.depth_func = video::gl::depth_fn::less;
        prepare_commands.depth.depth_test = false;
        prepare_commands.depth.depth_write = true;

        ambient_commands.clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.f);
        ambient_commands.memory_offset = 0;
        ambient_commands.commands.clear();
        ambient_commands.depth.depth_test = true;
        ambient_commands.depth.depth_write = true;
        ambient_commands.depth.depth_func = video::gl::depth_fn::less;
        //ambient_commands.rasterizer.cull_face = true;
        ambient_commands.rasterizer.cull_mode = video::gl::cull_face_mode::back;

        directional_commands.clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.f);
        directional_commands.memory_offset = 0;
        directional_commands.commands.clear();
        directional_commands.blend.enable = true;
        directional_commands.blend.dfactor = video::gl::blend_factor::one;
        directional_commands.blend.sfactor = video::gl::blend_factor::one;
        directional_commands.depth.depth_test = true;
        directional_commands.depth.depth_write = false;
        directional_commands.depth.depth_func = video::gl::depth_fn::equal;
        //directional_commands.rasterizer.cull_face = true;
        directional_commands.rasterizer.cull_mode = video::gl::cull_face_mode::back;

        glow_commands.clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.f);
        glow_commands.memory_offset = 0;
        glow_commands.commands.clear();
        glow_commands.depth.depth_test = true;
        glow_commands.depth.depth_write = true;
        //glow_commands.rasterizer.cull_face = true;
        glow_commands.rasterizer.cull_mode = video::gl::cull_face_mode::back;

        post_commands.clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.f);
        post_commands.memory_offset = 0;
        post_commands.commands.clear();
        post_commands.depth.depth_write = false;
        post_commands.blend.enable = true;
        post_commands.blend.sfactor = video::gl::blend_factor::one;
        post_commands.blend.sfactor = video::gl::blend_factor::src_alpha;
        post_commands.blend.dfactor = video::gl::blend_factor::one_minus_src_alpha;

        skybox_commands.clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.f);
        skybox_commands.memory_offset = 0;
        skybox_commands.commands.clear();
        skybox_commands.depth.depth_test = true;
        skybox_commands.depth.depth_write = false;
        skybox_commands.depth.depth_func = video::gl::depth_fn::lequal;
        skybox_commands.rasterizer.cull_face = true;
        skybox_commands.rasterizer.cull_mode = video::gl::cull_face_mode::front;
        skybox_commands.rasterizer.polygon_mode = true;

        terrain_commands.clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.f);
        terrain_commands.memory_offset = 0;
        terrain_commands.commands.clear();
        terrain_commands.depth.depth_test = false;
        terrain_commands.depth.depth_write = true;
        terrain_commands.depth.depth_func = video::gl::depth_fn::lequal;
    }

    auto forward_renderer::present(video::instance_t &vi, const glm::mat4 &proj, const glm::mat4 &view) -> void {
        using namespace game;

        //journal::debug(journal::_VIDEO, "Proj % View %", proj, view);        

        const auto white_tex = video::get_texture(vi, "white-map");

        glm::mat4 cam_model = glm::translate(glm::mat4(1.f), -glm::vec3(view[3])/*glm::vec3(0.f)*/);
        cam_model = glm::scale(cam_model, glm::vec3(5.f));
        glm::mat4 projection_view = proj * view;

        auto def_framebuffer = video::gl::default_framebuffer();

        prepare_commands << vcs::bind{sample_framebuffer};
        prepare_commands << vcs::viewport{sample_framebuffer};
        prepare_commands << vcs::clear{};

        skybox_commands << vcs::bind{skybox_shader};
        skybox_commands << vcs::uniform{skybox_shader, "projection_view_matrix", projection_view};
        skybox_commands << vcs::uniform{skybox_shader, "model_matrix", cam_model};

        skybox_commands << vcs::bind{skybox_shader, "cubemap", 0, skybox_map};
        skybox_commands << vcs::bind{0, texture_sampler};

        skybox_commands << vcs::bind{skybox_cube};
        skybox_commands << vcs::draw_elements{skybox_draw};

        ambient_commands << vcs::bind{ambient_light_shader};
        ambient_commands << vcs::uniform{ambient_light_shader, "projection_view_matrix", projection_view};

        for (const auto &lt : ambient_lights) {
            ambient_commands << vcs::uniform{ambient_light_shader, "ambient_intensity", lt.la};

            for (size_t i = 0; i < draws.size(); i++) {
                ambient_commands << vcs::uniform{ambient_light_shader, "model_matrix", matrices[i]};
                ambient_commands << vcs::uniform{ambient_light_shader, "ambient_color", materials[i].ka};

                ambient_commands << vcs::bind{ambient_light_shader, "ambient_map", 0, materials[i].diffuse_tex};
                ambient_commands << vcs::bind{0, texture_sampler};

                ambient_commands << vcs::bind{sources[i]};
                ambient_commands << vcs::draw_elements{draws[i]};
            }
        }

        directional_commands << vcs::bind{directional_light_shader};
        directional_commands << vcs::uniform{directional_light_shader, "projection_view_matrix", projection_view};

        directional_commands << vcs::uniform{directional_light_shader, "view_position", -glm::vec3(view[3])};


        for (const auto &lt : directional_lights) {


            directional_commands << vcs::uniform{directional_light_shader, "light_direction", lt.direction};
            directional_commands << vcs::uniform{directional_light_shader, "light.Ld", lt.ld};
            directional_commands << vcs::uniform{directional_light_shader, "light.Ls", lt.ls};

            for (size_t i = 0; i < draws.size(); i++) {

                directional_commands << vcs::uniform{directional_light_shader, "model_matrix", matrices[i]};
                directional_commands << vcs::uniform{directional_light_shader, "material.Kd", materials[i].kd};
                directional_commands << vcs::uniform{directional_light_shader, "material.Ks", materials[i].ks};
                directional_commands << vcs::uniform{directional_light_shader, "material.shininess", materials[i].ns};
                directional_commands << vcs::uniform{directional_light_shader, "material.transparency", 1.f};
                directional_commands << vcs::uniform{directional_light_shader, "material.reflectivity", materials[i].reflectivity};

                directional_commands << vcs::bind{directional_light_shader, "diffuse_map", 0, materials[i].diffuse_tex};
                directional_commands << vcs::bind{0, texture_sampler};

                directional_commands << vcs::bind{directional_light_shader, "specular_map", 1, /*materials[i].specular_tex*/white_tex};
                directional_commands << vcs::bind{1, texture_sampler};

                directional_commands << vcs::bind{directional_light_shader, "gloss_map", 2, /*materials[i].gloss_tex*/white_tex};
                directional_commands << vcs::bind{2, texture_sampler};

                directional_commands << vcs::bind{directional_light_shader, "normal_map", 3, materials[i].normal_tex};
                directional_commands << vcs::bind{3, texture_sampler};

                directional_commands << vcs::bind{directional_light_shader, "environment_map", 4, /*skybox_map*/white_tex};
                directional_commands << vcs::bind{4, filter_sampler};

                directional_commands << vcs::bind{sources[i]};
                directional_commands << vcs::draw_elements{draws[i]};
            }
        }

        glow_commands << vcs::bind{glow_framebuffer};
        glow_commands << vcs::viewport{glow_framebuffer};
        glow_commands << vcs::clear{};

        glow_commands << vcs::bind{emission_shader};
        glow_commands << vcs::uniform{emission_shader, "projection_view_matrix", projection_view};

        for (size_t i = 0; i < draws.size(); i++) {
            glow_commands << vcs::uniform{emission_shader, "model_matrix", matrices[i]};
            glow_commands << vcs::uniform{emission_shader, "emission_color", materials[i].ke};

            glow_commands << vcs::bind{emission_shader, "emission_map", 0, white_tex};
            glow_commands << vcs::bind{1, texture_sampler};

            glow_commands << vcs::bind{sources[i]};
            glow_commands << vcs::draw_elements{draws[i]};
        }

        // vblur
        post_commands << vcs::bind{blur_framebuffer};
        post_commands << vcs::viewport{blur_framebuffer};
        post_commands << vcs::clear{};

        post_commands << vcs::bind{filter_vblur_shader};

        const glm::vec2 size = glm::vec2(1.f / blur_framebuffer.width, 1.f / blur_framebuffer.height);
        post_commands << vcs::uniform{filter_vblur_shader, "size", size};
        post_commands << vcs::uniform{filter_vblur_shader, "scale", 2.0f};

        post_commands << vcs::bind{filter_vblur_shader, "tex0", 0, glow_map};
        post_commands << vcs::bind{0, filter_sampler};

        post_commands << vcs::bind{fullscreen_quad};
        post_commands << vcs::draw_elements{fullscreen_draw};

        // hblur
        post_commands << vcs::bind{glow_framebuffer};
        post_commands << vcs::viewport{glow_framebuffer};
        post_commands << vcs::clear{};

        post_commands << vcs::bind{filter_hblur_shader};

        post_commands << vcs::uniform{filter_hblur_shader, "size", size};
        post_commands << vcs::uniform{filter_hblur_shader, "scale", 2.0f};

        post_commands << vcs::bind{filter_hblur_shader, "tex0", 0, blur_map};
        post_commands << vcs::bind{0, filter_sampler};

        post_commands << vcs::bind{fullscreen_quad};
        post_commands << vcs::draw_elements{fullscreen_draw};

        // blit sample to color
        post_commands << vcs::blit{sample_framebuffer, color_framebuffer};

        // postprocess
        post_commands << vcs::bind{def_framebuffer};
        post_commands << vcs::viewport{def_framebuffer};
        post_commands << vcs::clear{};

        post_commands << vcs::bind{postprocess_shader};

        post_commands << vcs::bind{postprocess_shader, "color_map", 0, color_map};
        post_commands << vcs::bind{0, filter_sampler};

        post_commands << vcs::bind{postprocess_shader, "glow_map", 1, glow_map};
        post_commands << vcs::bind{1, filter_sampler};

        post_commands << vcs::bind{fullscreen_quad};
        post_commands << vcs::draw_elements{fullscreen_draw};

        video::submit_triangles_batch(post_commands, triangles, sprite_shader, texture_sampler);
        video::submit_sprite_batch(post_commands, sprites, sprite_shader, texture_sampler);

        video::present(vi, {&prepare_commands, &skybox_commands, &ambient_commands, &directional_commands, &glow_commands, &post_commands});
        reset();
    }

    inline auto color_to_vec4(const uint32_t color) -> glm::vec4 {
        constexpr auto max_component = 255.0f;
        float x = static_cast<float>((color & 0xff000000) >> 24) / max_component;
        float y = static_cast<float>((color & 0x00ff0000) >> 16) / max_component;
        float z = static_cast<float>((color & 0x0000ff00) >> 8) / max_component;
        float w = static_cast<float>(color & 0x000000ff) / max_component;

        return glm::vec4(x, y, z, w);
    }

    auto forward_renderer::draw_text(const ui::draw_text_command &c) -> void {
        using namespace glm;
        using namespace game;

        if (!c.font) {
            journal::error(journal::_VIDEO, "%", "Empty font");
            return;
        }

        // FIXME: use real texture size
        const auto tw = 1024.f;
        const auto th = 1024.f;

        const int adv_y = c.font->lineskip;
        const int fh = c.font->size;
        const float spt = 2.f / video::screen.width;
        const float correction = video::screen.aspect;
        const auto color = color_to_vec4(c.color);

        auto [bsx, bsy] = video::get_text_length(*c.font, c.text);
        vec2 p = vec2(c.x, c.y);

        if (c.align & ui::align_horizontal_right)
             p.x += c.w * 2.f - bsx * 2.f;
        else if (c.align & ui::align_horizontal_center)
            p.x += c.w - bsx * 0.5;
        else
            p.x += 0;

        if (c.align & ui::align_vertical_top)
            p.y += c.h * 2.f - bsy;
        else if (c.align & ui::align_vertical_center)
            p.y += c.h - bsy * 0.5;
        else
            p.y += 0;

        for (size_t i = 0; i < c.size; i++) {
            auto ch = c.text[i];
            if (ch == '\n') {
                p[0] = c.x;
                p[1] -= spt * adv_y * video::screen.aspect;
                continue;
            }

            auto glyph = video::get_glyph_rect(*c.font, ch);
            if (!glyph) {
                game::journal::warning(game::journal::_RENDER, "%", "Glyph not found");
                continue;
            }

            vec2 size = {glyph.value().advance * spt, fh * spt};
            vec4 offset;
            offset[0] = (float)glyph.value().x / tw;
            offset[1] = (float)glyph.value().y / th;
            offset[2] = (float)glyph.value().w / tw;
            offset[3] = (float)glyph.value().h / th;

            const video::v3t2c4 vertices[6] = {
                {{p[0], p[1] + size[1] * correction, 0}, {offset[0], offset[1]}, {color[0], color[1], color[2], color[3]}},
                {{p[0] + size[0], p[1] + size[1] * correction, 0}, {offset[0] + offset[2], offset[1]}, {color[0], color[1], color[2], color[3]}},
                {{p[0] + size[0], p[1], 0}, {offset[0] + offset[2], offset[1] + offset[3]}, {color[0], color[1], color[2], color[3]}},
                {{p[0] + size[0], p[1], 0}, {offset[0] + offset[2], offset[1] + offset[3]}, {color[0], color[1], color[2], color[3]}},
                {{p[0], p[1], 0}, {offset[0], offset[1] + offset[3]}, {color[0], color[1], color[2], color[3]}},
                {{p[0], p[1] + size[1] * correction, 0}, {offset[0], offset[1]}, {color[0], color[1], color[2], color[3]}},
            };

            video::append_triangles_vertices(triangles, vertices, 6);

            p[0] += glyph.value().advance * spt;
        }
    }

    auto forward_renderer::draw_line(const ui::draw_line_command &c) -> void {
        using namespace glm;

        const auto color = color_to_vec4(c.color);

        const auto p = normalize(vec2{c.x1 - c.x0, c.y1 - c.y0});
        const auto n = normalize(vec2{c.y1 - c.y0, -(c.x1 - c.x0)});
        const auto e = vec2{c.w * n.x, c.w * n.y * video::screen.aspect};
        const auto z = vec2{c.w * p.x, c.w * p.y * video::screen.aspect};

        const auto coords = vec4{0.f, 0, 63.f / 512.f, 63.f / 512.f};
        const auto offset = vec2{1.f / 64.f, 1.f / 64.f};

        const auto x0 = c.x0 * 2.f;
        const auto x1 = c.x1 * 2.f;
        const auto y0 = c.y0 * 2.f/* video::screen.aspect*/;
        const auto y1 = c.y1 * 2.f/* video::screen.aspect*/;

        const video::v3t2c4 vertices[6] = {
            {{x0 - e.x - z.x, y0 - e.y - z.y, 0.f}, {coords.x            + offset.x, coords.y            + offset.y}, color}, // 0
            {{x1 - e.x + z.x, y1 - e.y + z.y, 0.f}, {coords.x + coords.z - offset.x, coords.y            + offset.y}, color}, // 1
            {{x0 + e.x - z.x, y0 + e.y - z.y, 0.f}, {coords.x            + offset.x, coords.y + coords.w - offset.y}, color}, // 2
            {{x1 - e.x + z.x, y1 - e.y + z.y, 0.f}, {coords.x + coords.z - offset.x, coords.y            + offset.y}, color}, // 1
            {{x1 + e.x + z.x, y1 + e.y + z.y, 0.f}, {coords.x + coords.z - offset.x, coords.y + coords.w - offset.y}, color}, // 3
            {{x0 + e.x - z.x, y0 + e.y - z.y, 0.f}, {coords.x            + offset.x, coords.y + coords.w - offset.y}, color}  // 2
        };

        video::append_triangles_vertices(triangles, vertices, 6);
    }

    auto forward_renderer::draw_rect(const ui::draw_rect_command &c) -> void {
        using namespace glm;

        const auto color = color_to_vec4(c. color);
        const auto x = c.x * 2;
        const auto w = c.w * 2;
        const auto y = c.y * 2;
        const auto h = c.h * 2/* video::screen.aspect*/;

        const auto coords = vec4{0.f, 0, 63.f / 512.f, 63.f / 512.f};
        const auto offset = vec2{1.f / 64.f, 1.f / 64.f};

        const video::v3t2c4 vertices[6] = {
            {{x    , y    , 0.f}, {coords.x            + offset.x, coords.y            + offset.y}, color}, // 0
            {{x + w, y    , 0.f}, {coords.x + coords.z - offset.x, coords.y            + offset.y}, color}, // 1
            {{x    , y + h, 0.f}, {coords.x            + offset.x, coords.y + coords.w - offset.y}, color}, // 2
            {{x + w, y    , 0.f}, {coords.x + coords.z - offset.x, coords.y            + offset.y}, color}, // 1
            {{x + w, y + h, 0.f}, {coords.x + coords.z - offset.x, coords.y + coords.w - offset.y}, color}, // 3
            {{x    , y + h, 0.f}, {coords.x            + offset.x, coords.y + coords.w - offset.y}, color}  // 2
        };

        video::append_triangles_vertices(triangles, vertices, 6);
    }

    auto forward_renderer::draw_roundrect(const ui::draw_round_rect_command &c) -> void {
        (void)c;
    }

    auto forward_renderer::draw_icon(const ui::draw_icon_command &c) -> void {
        (void)c;
    }
} // namespace renderer

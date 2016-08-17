#include <video/screen.hpp>
#include <video/commands.hpp>
#include <video/glyphs.hpp>
#include <video/screen.hpp>
#include <core/settings.hpp>
#include "forward_renderer.hpp"

namespace renderer {
    forward_renderer::forward_renderer() {
        application::debug(application::log_category::render, "% % with % %\n", "Create forward render", "version 1.00", video::gl::api_name, video::gl::api_version);        

        init_ui();

        emission_shader = video::get_shader("emission-shader");
        ambient_light_shader = video::get_shader("ambient-light-shader");
        directional_light_shader = video::get_shader("forward-directional-shader");
        postprocess_shader = video::get_shader("postprocess-shader");
        filter_vblur_shader = video::get_shader("vblur-shader");
        filter_hblur_shader = video::get_shader("hblur-shader");
        skybox_shader = video::get_shader("skybox-shader");
        sprite_shader = video::get_shader("sprite-shader");

        video::gl::sampler_info sam_info;

        switch (video::config.filtering) {
        case video::texture_filtering::bilinear:
            sam_info.mag_filter = video::gl::texture_mag_filter::linear;
            sam_info.min_filter = video::gl::texture_min_filter::linear;
            break;
        case video::texture_filtering::trilinear:
            sam_info.mag_filter = video::gl::texture_mag_filter::linear;
            sam_info.min_filter = video::gl::texture_min_filter::linear_mipmap_linear;
            break;
        case video::texture_filtering::anisotropic:
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
        const auto samples = application::int_value("video_fsaa", 1);

        color_map = video::gl::create_texture_2d({video::pixel_format::rgba16f, 0, 0, video::screen.width, video::screen.height, 0, nullptr});
        depth_map = video::gl::create_texture_2d({video::pixel_format::depth, 0, 0, video::screen.width, video::screen.height, 0, nullptr});
        glow_map = video::gl::create_texture_2d({video::pixel_format::rgb16f, 0, 0, video::screen.width / ratio, video::screen.height / ratio, 0, nullptr});
        blur_map = video::gl::create_texture_2d({video::pixel_format::rgb16f, 0, 0, video::screen.width / ratio, video::screen.height / ratio, 0, nullptr});

        blur_depth = video::gl::create_renderbuffer({video::pixel_format::depth, video::screen.width / ratio, video::screen.height / ratio, 0});
        sample_color = video::gl::create_renderbuffer({video::pixel_format::rgb16f, video::screen.width, video::screen.height, samples});
        sample_depth = video::gl::create_renderbuffer({video::pixel_format::depth, video::screen.width, video::screen.height, samples});

        memset(&skybox_map, 0, sizeof skybox_map);

        uint32_t mask = static_cast<uint32_t>(video::gl::framebuffer_mask::color_buffer) | static_cast<uint32_t>(video::gl::framebuffer_mask::depth_buffer);

        using namespace video;

        color_framebuffer = gl::create_framebuffer({screen.width, screen.height, mask, {{gl::framebuffer_attachment::color0, gl::framebuffer_attachment_target::texture, color_map.id},
                                                                                        {gl::framebuffer_attachment::depth, gl::framebuffer_attachment_target::texture, depth_map.id}}});

        glow_framebuffer = gl::create_framebuffer({screen.width / ratio, screen.height / ratio, mask, {{gl::framebuffer_attachment::color0, gl::framebuffer_attachment_target::texture, glow_map.id},
                                                                                       {gl::framebuffer_attachment::depth, gl::framebuffer_attachment_target::renderbuffer, blur_depth.id}}});

        blur_framebuffer = gl::create_framebuffer({screen.width / ratio, screen.height / ratio, mask, {{gl::framebuffer_attachment::color0, gl::framebuffer_attachment_target::texture, blur_map.id}}});

        sample_framebuffer = gl::create_framebuffer({screen.width, screen.height, mask, {{gl::framebuffer_attachment::color0, gl::framebuffer_attachment_target::renderbuffer, sample_color.id},
                                                                                         {gl::framebuffer_attachment::depth, gl::framebuffer_attachment_target::renderbuffer, sample_depth.id}}});

        auto quad_vi = video::vertgen::make_plane(glm::mat4{1.f});
        std::vector<vertices_draw> quad_vd;
        fullscreen_quad = video::make_vertices_source({quad_vi.data}, quad_vi.desc, quad_vd);
        fullscreen_draw = quad_vd[0];

        auto cube_vi = video::vertgen::make_cube(glm::mat4{1.f});
        std::vector<vertices_draw> cube_vd;
        skybox_cube = video::make_vertices_source({cube_vi.data}, cube_vi.desc, cube_vd);
        skybox_draw = cube_vd[0];

        sprite_batch_info sb_info;
        sb_info.max_sprites = 1000;
        sb_info.tex = video::default_white_texture();
        sb_info.tex = video::get_texture("glyphs-map");
        sprites = video::create_sprite_batch(sb_info);

        triangles_batch_info tb_info;
        tb_info.max_triangles = 1000;
        tb_info.tex = video::get_texture("glyphs-map");

        triangles = video::create_triangles_batch(tb_info);

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
        application::debug(application::log_category::render, "%\n", "Destroy forward render");
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

    auto forward_renderer::append(int32_t font, const std::string &text, const glm::vec2 &pos, const glm::vec4 &color) -> void {
        using namespace glm;

        const auto tw = 512.f;
        const auto th = 512.f;

        const int adv_y = video::glyph_cache_get_font_lineskip(font);
        const int fh = video::glyph_cache_get_font_size(font);
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
                application::warning(application::log_category::render, "%\n", "Glyph not found");
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
    }

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
    }

    auto forward_renderer::present(const glm::mat4 &proj, const glm::mat4 &view) -> void {
        UNUSED(proj), UNUSED(view);

        if (skybox_map.id == 0)
            skybox_map = video::get_texture("skybox1");

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

                directional_commands << vcs::bind{directional_light_shader, "specular_map", 1, materials[i].specular_tex};
                directional_commands << vcs::bind{1, texture_sampler};

                directional_commands << vcs::bind{directional_light_shader, "gloss_map", 2, materials[i].gloss_tex};
                directional_commands << vcs::bind{2, texture_sampler};

                directional_commands << vcs::bind{directional_light_shader, "normal_map", 3, materials[i].normal_tex};
                directional_commands << vcs::bind{3, texture_sampler};

                directional_commands << vcs::bind{directional_light_shader, "environment_map", 4, skybox_map};
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

            glow_commands << vcs::bind{emission_shader, "emission_map", 0, video::default_white_texture()};
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

        video::present({&prepare_commands, &skybox_commands, &ambient_commands, &directional_commands, &glow_commands, &post_commands});
        reset();
    }

    auto forward_renderer::init_ui() -> void {
        std::vector<video::font_info> fonts = {
            {"Play.ttf", 22, video::default_charset()}
        };

        const int asz = 512;

        auto white_im = video::imgen::make_color(64, 64, {255, 255, 255});
        ui_atlas = video::create_atlas(asz, asz, 1);
        auto rc = video::insert_image(ui_atlas, white_im);
        delete[] white_im.pixels;
        video::glyph_cache_build(fonts, ui_atlas);
        video::make_texture_2d("glyphs-map", get_atlas_texture(ui_atlas));
        ui_rc = glm::vec4{rc.x / (float)asz, rc.y / (float)asz, rc.w / (float)asz, rc.h / (float)asz};
    }

    inline auto color_to_vec4(uint32_t color) -> glm::vec4 {
        constexpr auto max_component = 255.0f;
        float x = static_cast<float>((color & 0xff000000) >> 24) / max_component;
        float y = static_cast<float>((color & 0x00ff0000) >> 16) / max_component;
        float z = static_cast<float>((color & 0x0000ff00) >> 8) / max_component;
        float w = static_cast<float>(color & 0x000000ff) / max_component;

        return glm::vec4(x, y, z, w);
    }

    auto forward_renderer::draw_text(const ui::draw_text_command &c) -> void {
        using namespace glm;

        // FIXME: use real texture size
        const auto tw = 512.f;
        const auto th = 512.f;

        const int adv_y = video::glyph_cache_get_font_lineskip(c.font);
        const int fh = video::glyph_cache_get_font_size(c.font);
        const float spt = 2.f / video::screen.width;
        const float correction = video::screen.aspect;
        const auto color = color_to_vec4(c.color);

        vec2 p = vec2(c.x, c.y);

        for (size_t i = 0; i < c.size; i++) {
            auto ch = c.text[i];
            if (ch == '\n') {
                p[0] = c.x;
                p[1] -= spt * adv_y * video::screen.aspect;
                continue;
            }

            auto glyph = video::glyph_cache_find(ch, c.font);
            if (glyph.ch == 0) {
                application::warning(application::log_category::render, "%\n", "Glyph not found");
                continue;
            }

            vec2 size = {glyph.advance * spt, fh * spt};
            vec4 offset;
            offset[0] = (float)glyph.rc.x / tw;
            offset[1] = (float)glyph.rc.y / th;
            offset[2] = (float)glyph.rc.w / tw;
            offset[3] = (float)glyph.rc.h / th;

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

        const auto x0 = c.x0;
        const auto x1 = c.x1;
        const auto y0 = c.y0 * video::screen.aspect;
        const auto y1 = c.y1 * video::screen.aspect;

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
        const auto x = c.x;
        const auto w = c.w;
        const auto y = c.y;
        const auto h = c.h * video::screen.aspect;

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

    }

    auto forward_renderer::draw_icon(const ui::draw_icon_command &c) -> void {

    }
} // namespace renderer

#include <video/screen.hpp>
#include <video/commands.hpp>
#include <video/glyphs.hpp>
#include <video/screen.hpp>
#include <core/journal.hpp>
#include <core/settings.hpp>
#include <utility/utf.hpp>
#include "forward_renderer.hpp"

#include <GL/ext_texture_filter_anisotropic.h>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace renderer {
    forward_renderer::forward_renderer(video::instance_t &vi, const json &info) : aspect_ratio{vi.aspect_ratio} {
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

        glyphs_map = video::get_texture(vi, "glyphs-map");

        sprite_batch_info sb_info;
        sb_info.max_sprites = 1000;
        sb_info.tex = glyphs_map;
        sprites = video::create_sprite_batch(vi, sb_info);

        triangles_batch_info tb_info;
        tb_info.max_triangles = 1000;
        tb_info.tex = glyphs_map;

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

    auto forward_renderer::dispath(video::instance_t &vi, const ui::draw_command_t &c) -> void {
        using namespace imui;

        std::visit(overloaded {
                       [this](const ui::draw_commands::draw_line &line) {
                           draw_line(line.x0, line.y0, line.x1, line.y1, line.w, line.color);
                       },
                       [this](const ui::draw_commands::draw_rect &rect) {
                           draw_rect(rect.x, rect.y, rect.w, rect.h, rect.color);
                       },
                       [this, vi](const ui::draw_commands::draw_text &text) {
                           if (text.font < vi.fonts.size()) {
                               const auto &font = vi.fonts[text.font];
                               draw_text(font, text.x, text.y, text.w, text.h, text.text, text.align, text.color);
                           }
                       }
                   }, c);
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
        const float x = static_cast<float>((color & 0xff000000) >> 24) / max_component;
        const float y = static_cast<float>((color & 0x00ff0000) >> 16) / max_component;
        const float z = static_cast<float>((color & 0x0000ff00) >> 8) / max_component;
        const float w = static_cast<float>(color & 0x000000ff) / max_component;

        return {x, y, z, w};
    }

    auto forward_renderer::draw_text(const video::font_t &font, float _x, float _y, float _w, float _h, const std::string &text, uint32_t _align, uint32_t _color) -> void {
        using namespace glm;
        using namespace game;

        const auto tw = static_cast<float>(glyphs_map.width);
        const auto th = static_cast<float>(glyphs_map.height);

        //const float screen_pt_x = 1.f / video::screen.width;
        const float screen_pt_y = 1.f / video::screen.height;
        const int adv_y = font.lineskip;
        const float fh = font.size * screen_pt_y;
        const auto correction = aspect_ratio;
        const auto color = color_to_vec4(_color);

        const auto x = _x * 2.f;
        const auto w = _w * 2.f;
        const auto y = _y * 2.f;
        const auto h = _h * 2.f;


        auto p = vec2{x, y};

        if (_align != 0) {
            auto [bsw, bsh] = video::get_text_length(font, text);

            if (_align & ui::align_horizontal_right)
                 p.x += w - bsw;
            else if (_align & ui::align_horizontal_center)
                p.x += w * 0.5f - bsw * 0.5f;
            else
                p.x += 0.f;

            if (_align & ui::align_vertical_top)
                p.y += h - bsh;
            else if (_align & ui::align_vertical_center)
                p.y += h * 0.5f - bsh * 0.5f;
            else
                p.y += 0.f;
        }

        const auto unicode_text = utility::to_utf16(text);

        for (const auto ch : unicode_text) {
            if (ch == '\n') {
                p.x = x;
                p.y -= screen_pt_y * adv_y * correction;
                continue;
            }

            const auto glyph = video::get_glyph_rect(font, ch);
            if (!glyph) {
                game::journal::warning(game::journal::_RENDER, "Glyph % not found", ch);
                continue;
            }

            vec2 size = {glyph.value().advance * screen_pt_y, fh * correction};
            vec4 offset;
            offset[0] = (float)glyph.value().x / tw;
            offset[1] = (float)glyph.value().y / th;
            offset[2] = (float)glyph.value().w / tw;
            offset[3] = (float)glyph.value().h / th;

            const video::v3t2c4 vertices[6] = {
                {{p.x         , p.y + size.y, 0}, {offset[0]            , offset[1]}            , color},
                {{p.x + size.x, p.y + size.y, 0}, {offset[0] + offset[2], offset[1]}            , color},
                {{p.x + size.x, p.y,          0}, {offset[0] + offset[2], offset[1] + offset[3]}, color},
                {{p.x + size.x, p.y,          0}, {offset[0] + offset[2], offset[1] + offset[3]}, color},
                {{p.x         , p.y,          0}, {offset[0]            , offset[1] + offset[3]}, color},
                {{p.x         , p.y + size.y, 0}, {offset[0]            , offset[1]}            , color},
            };

            video::append_triangles_vertices(triangles, vertices, 6);

            p.x += glyph.value().advance * screen_pt_y;
        }
    }

    auto forward_renderer::draw_line(float _x0, float _y0, float _x1, float _y1, float _w, uint32_t _color) -> void {
        using namespace glm;

        const auto color = color_to_vec4(_color);

        const auto p = normalize(vec2{_x1 - _x0, _y1 - _y0});
        const auto n = normalize(vec2{_y1 - _y0, -(_x1 - _x0)});
        const auto e = vec2{_w * n.x, _w * n.y * aspect_ratio};
        const auto z = vec2{_w * p.x, _w * p.y * aspect_ratio};

        const auto coords = vec4{0.f, 0, 63.f / static_cast<float>(glyphs_map.width), 63.f / static_cast<float>(glyphs_map.height)};
        const auto offset = vec2{1.f / 64.f, 1.f / 64.f};

        const auto x0 = _x0 * 2.f;
        const auto x1 = _x1 * 2.f;
        const auto y0 = _y0 * 2.f;
        const auto y1 = _y1 * 2.f;

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

    auto forward_renderer::draw_rect(const float _x, const float _y, const float _w, const float _h, const uint32_t _color) -> void {
        using namespace glm;

        const auto color = color_to_vec4(_color);
        const auto x = _x * 2.f;
        const auto w = _w * 2.f;
        const auto y = _y * 2.f;
        const auto h = _h * 2.f;

        const auto coords = vec4{0.f, 0, 63.f / static_cast<float>(glyphs_map.width), 63.f / static_cast<float>(glyphs_map.height)};
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
} // namespace renderer

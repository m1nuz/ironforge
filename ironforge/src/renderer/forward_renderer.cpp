#include <video/screen.hpp>
#include "forward_renderer.hpp"

namespace renderer {
    forward_renderer::forward_renderer() {
        application::debug(application::log_category::render, "% % with % %\n", "Create forward render", "version 1.00", video::gl::api_name, video::gl::api_version);        

        //emission_shader = video::get_shader("emission-shader");
        ambient_light_shader = video::get_shader("ambient-light-shader");
        directional_light_shader = video::get_shader("forward-directional-shader");
        postprocess_shader = video::get_shader("postprocess-shader");

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
        color_map = video::gl::create_texture_2d({video::pixel_format::rgba16f, 0, 0, video::screen.width, video::screen.height, 0, nullptr});
        depth_map = video::gl::create_texture_2d({video::pixel_format::depth, 0, 0, video::screen.width, video::screen.height, 0, nullptr});

        uint32_t mask = static_cast<uint32_t>(video::gl::framebuffer_mask::color_buffer) | static_cast<uint32_t>(video::gl::framebuffer_mask::depth_buffer);

        using namespace video;

        color_framebuffer = gl::create_framebuffer({screen.width, screen.height, mask, {{gl::framebuffer_attachment::color0, gl::framebuffer_attachment_target::texture, color_map.id},
                                                                                        {gl::framebuffer_attachment::depth, gl::framebuffer_attachment_target::texture, depth_map.id}}});

        auto quad_vi = video::vertgen::make_quad_plane(glm::mat4{1.f});
        std::vector<vertices_draw> quad_vd;
        fullscreen_quad = video::make_vertices_source({quad_vi.data}, quad_vi.desc, quad_vd);
        fullscreen_draw = quad_vd[0];

        sources.reserve(max_sources);
        draws.reserve(max_draws);
        matrices.reserve(max_matrices);
        materials.reserve(max_materials);

        reset();
    }

    forward_renderer::~forward_renderer() {
        video::gl::destroy_framebuffer(color_framebuffer);
        video::gl::destroy_texture(color_map);
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

        directional_commands.clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.f);
        directional_commands.memory_offset = 0;
        directional_commands.commands.clear();
        directional_commands.blend.enable = true;
        directional_commands.blend.dfactor = video::gl::blend_factor::one;
        directional_commands.blend.sfactor = video::gl::blend_factor::one;
        directional_commands.depth.depth_test = true;
        directional_commands.depth.depth_write = false;
        directional_commands.depth.depth_func = video::gl::depth_fn::equal;

        post_commands.memory_offset = 0;
        post_commands.commands.clear();
    }

    auto forward_renderer::present(const glm::mat4 &proj, const glm::mat4 &view) -> void {
        UNUSED(proj), UNUSED(view);

        glm::mat4 projection_view = proj * view;

        auto def_framebuffer = video::gl::default_framebuffer();

        prepare_commands << video::gl::bind_framebuffer_op{color_framebuffer.id};
        prepare_commands << video::gl::viewpor_op{0, 0, color_framebuffer.width, color_framebuffer.height};
        prepare_commands << video::gl::clear_op{};

        ambient_commands << video::gl::bind_program_op{ambient_light_shader.pid};
        ambient_commands << video::gl::send_uniform{video::gl::get_uniform_location(ambient_light_shader, "projection_view_matrix"), projection_view};

        for (const auto &lt : ambient_lights) {
            ambient_commands << video::gl::send_uniform{video::gl::get_uniform_location(ambient_light_shader, "ambient_intensity"), lt.la};

            for (size_t i = 0; i < draws.size(); i++) {
                ambient_commands << video::gl::send_uniform{video::gl::get_uniform_location(ambient_light_shader, "model_matrix"), matrices[i]};
                ambient_commands << video::gl::send_uniform{video::gl::get_uniform_location(ambient_light_shader, "ambient_color"), materials[i].ka};

                ambient_commands << video::gl::bind_texture_op{video::gl::get_uniform_location(ambient_light_shader, "ambient_map"), 0, materials[i].diffuse_tex.target, materials[i].diffuse_tex.id};
                ambient_commands << video::gl::bind_sampler_op{0, texture_sampler.id};

                ambient_commands << video::gl::bind_vertex_array_op{sources[i].array.id};
                ambient_commands << video::gl::draw_elements_op{draws[i].count};
            }
        }

        directional_commands << video::gl::bind_program_op{directional_light_shader.pid};
        directional_commands << video::gl::send_uniform{video::gl::get_uniform_location(directional_light_shader, "projection_view_matrix"), projection_view};

        for (const auto &lt : directional_lights) {
            directional_commands << video::gl::send_uniform{video::gl::get_uniform_location(directional_light_shader, "light_direction"), lt.direction};
            directional_commands << video::gl::send_uniform{video::gl::get_uniform_location(directional_light_shader, "light.Ld"), lt.ld};
            directional_commands << video::gl::send_uniform{video::gl::get_uniform_location(directional_light_shader, "light.Ls"), lt.ls};

            for (size_t i = 0; i < draws.size(); i++) {
                directional_commands << video::gl::send_uniform{video::gl::get_uniform_location(directional_light_shader, "model_matrix"), matrices[i]};
                directional_commands << video::gl::send_uniform{video::gl::get_uniform_location(directional_light_shader, "material.Kd"), materials[i].kd};
                directional_commands << video::gl::send_uniform{video::gl::get_uniform_location(directional_light_shader, "material.Ks"), materials[i].ks};
                directional_commands << video::gl::send_uniform{video::gl::get_uniform_location(directional_light_shader, "material.shininess"), materials[i].ns};
                directional_commands << video::gl::send_uniform{video::gl::get_uniform_location(directional_light_shader, "material.transparency"), 1.f};

                directional_commands << video::gl::bind_texture_op{video::gl::get_uniform_location(directional_light_shader, "diffuse_map"), 0, materials[i].diffuse_tex.target, materials[i].diffuse_tex.id};
                directional_commands << video::gl::bind_sampler_op{0, texture_sampler.id};

                directional_commands << video::gl::bind_texture_op{video::gl::get_uniform_location(directional_light_shader, "specular_map"), 1, materials[i].specular_tex.target, materials[i].specular_tex.id};
                directional_commands << video::gl::bind_sampler_op{1, texture_sampler.id};

                directional_commands << video::gl::bind_texture_op{video::gl::get_uniform_location(directional_light_shader, "gloss_map"), 2, materials[i].gloss_tex.target, materials[i].gloss_tex.id};
                directional_commands << video::gl::bind_sampler_op{2, texture_sampler.id};

                directional_commands << video::gl::bind_vertex_array_op{sources[i].array.id};
                directional_commands << video::gl::draw_elements_op{draws[i].count};
            }
        }

        post_commands << video::gl::bind_framebuffer_op{def_framebuffer.id};
        post_commands << video::gl::viewpor_op{0, 0, def_framebuffer.width, def_framebuffer.height};
        post_commands << video::gl::clear_op{};

        post_commands << video::gl::bind_program_op{postprocess_shader.pid};

        post_commands << video::gl::bind_texture_op{video::gl::get_uniform_location(postprocess_shader, "color_map"), 0, color_map.target, color_map.id};
        post_commands << video::gl::bind_sampler_op{0, filter_sampler.id};

        post_commands << video::gl::bind_texture_op{video::gl::get_uniform_location(postprocess_shader, "glow_map"), 1, video::default_black_texture().target, video::default_black_texture().id};
        post_commands << video::gl::bind_sampler_op{1, filter_sampler.id};

        post_commands << video::gl::bind_vertex_array_op{fullscreen_quad.array.id};
        post_commands << video::gl::draw_elements_op{fullscreen_draw.count};

        video::present({&prepare_commands, &ambient_commands, &directional_commands, &post_commands});
        reset();
    }
} // namespace renderer

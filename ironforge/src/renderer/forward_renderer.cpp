#include "forward_renderer.hpp"

namespace renderer {
    forward_renderer::forward_renderer() : prepare_commands{1024 * 4}, ambient_commands{1024 * 4}, directional_commands{1024 * 4} {
        application::debug(application::log_category::render, "% % with % %\n", "Create forward render", "version 1.00", video::gl::api_name, video::gl::api_version);        

        //emission_shader = video::get_shader("emission-shader");
        ambient_light_shader = video::get_shader("ambient-light-shader");
        directional_light_shader = video::get_shader("forward-directional-shader");

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

        sources.reserve(max_sources);
        draws.reserve(max_draws);
        matrices.reserve(max_matrices);
        materials.reserve(max_materials);

        reset();
    }

    forward_renderer::~forward_renderer() {
        video::gl::destroy_sampler(texture_sampler);
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

        ambient_commands.clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.f);
        ambient_commands.memory_offset = 0;
        ambient_commands.commands.clear();

        /*directional_commands.clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.f);
        directional_commands.memory_offset = 0;
        directional_commands.commands.clear();
        directional_commands.blend.enable = true;
        directional_commands.blend.dfactor = video::gl::blend_factor::one;
        directional_commands.blend.sfactor = video::gl::blend_factor::one;
        directional_commands.depth.depth_write = false;*/
    }

    auto forward_renderer::present(const glm::mat4 &proj, const glm::mat4 &view) -> void {
        UNUSED(proj), UNUSED(view);

        glm::mat4 projection_view = proj * view;

        prepare_commands << video::gl::bind_framebuffer_op{video::gl::default_framebuffer().id};
        prepare_commands << video::gl::clear_op{};

        ambient_commands << video::gl::bind_program_op{ambient_light_shader.pid};

        for (const auto &lt : ambient_lights) {
            for (size_t i = 0; i < draws.size(); i++) {
                ambient_commands << video::gl::send_uniform{video::gl::get_uniform_location(ambient_light_shader, "projection_view_matrix"), projection_view};
                ambient_commands << video::gl::send_uniform{video::gl::get_uniform_location(ambient_light_shader, "model_matrix"), matrices[i]};

                ambient_commands << video::gl::send_uniform{video::gl::get_uniform_location(ambient_light_shader, "ambient_color"), materials[i].kd};
                ambient_commands << video::gl::send_uniform{video::gl::get_uniform_location(ambient_light_shader, "ambient_intensity"), lt.la/*glm::vec3{1.f}*/};

                ambient_commands << video::gl::bind_texture_op{video::gl::get_uniform_location(ambient_light_shader, "ambient_map"), 0, materials[i].diffuse_tex.target, materials[i].diffuse_tex.id};
                ambient_commands << video::gl::bind_sampler_op{0, texture_sampler.id};

                ambient_commands << video::gl::bind_vertex_array_op{sources[i].array.id};
                ambient_commands << video::gl::draw_elements_op{draws[i].count};
            }
        }

        video::present({&prepare_commands, &ambient_commands});
        reset();
    }
} // namespace renderer

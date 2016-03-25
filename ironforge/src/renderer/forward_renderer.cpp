#include "forward_renderer.hpp"

namespace renderer {
    forward_renderer::forward_renderer() : prepare_commands{1024 * 4} {
        application::debug(application::log_category::render, "% % with % %\n", "Create forward render", "version 1.00", video::gl::api_name, video::gl::api_version);

        emission_shader = video::get_shader("emission-shader");
        ambient_light_shader = video::get_shader("ambient-light-shader");

        video::gl::sampler_info sam_info;
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
        UNUSED(light);
        // do nothing
    }

    auto forward_renderer::append(const phong::directional_light &light) -> void {
        UNUSED(light);
        // do nothing
    }

    auto forward_renderer::append(const phong::point_light &light) -> void {
        UNUSED(light);
        // do nothing
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

        prepare_commands.clear_color = glm::vec4(0.7f, 0.7f, 0.7f, 0.f);
        prepare_commands.memory_offset = 0;
        prepare_commands.commands.clear();
    }

    auto forward_renderer::present(const glm::mat4 &proj, const glm::mat4 &view) -> void {
        UNUSED(proj), UNUSED(view);

        prepare_commands << video::gl::clear_op{};

        prepare_commands << video::gl::bind_program_op{ambient_light_shader.pid};

        glm::mat4 projection_view = proj * view;

        for (size_t i = 0; i < draws.size(); i++) {
            //application::debug(application::log_category::render, "draw array % %\n", sources[i].array.id, draws[i].count);

            prepare_commands << video::gl::send_uniform{video::gl::get_uniform_location(ambient_light_shader, "projection_view_matrix"), projection_view};
            prepare_commands << video::gl::send_uniform{video::gl::get_uniform_location(ambient_light_shader, "model_matrix"), matrices[i]};

            prepare_commands << video::gl::send_uniform{video::gl::get_uniform_location(ambient_light_shader, "ambient_color"), materials[i].kd};
            //prepare_commands << video::gl::send_uniform{video::gl::get_uniform_location(ambient_light_shader, "ambient_intensity"), };

            // bind_texture
            // bind_sampler

            prepare_commands << video::gl::bind_vertex_array_op{sources[i].array.id};
            prepare_commands << video::gl::draw_elements_op{draws[i].count};
        }

        video::present({&prepare_commands});
        reset();
    }
} // namespace renderer

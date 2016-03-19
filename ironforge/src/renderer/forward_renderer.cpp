#include "forward_renderer.hpp"

namespace renderer {
    forward_renderer::forward_renderer() {
        application::debug(application::log_category::render, "% % with % %\n", "Create forward render", "version 1.00", video::gl::api_name, video::gl::api_version);

        emission_shader = video::get_shader("emission-shader");

        sources.reserve(max_sources);
        draws.reserve(max_draws);
        matrices.reserve(max_matrices);
        materials.reserve(max_materials);

        reset();
    }

    forward_renderer::~forward_renderer() {
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

        prepare_commands.clear_color = glm::vec4(0.f, 0.f, 0.f, 0.f);
    }

    auto forward_renderer::present(const glm::mat4 &proj, const glm::mat4 &view) -> void {
        UNUSED(proj), UNUSED(view);

        prepare_commands << video::gl::clear_op{};

        for (size_t i = 0; i < draws.size(); i++) {
            application::debug(application::log_category::render, "draw array % %\n", sources[i].array.id, draws[i].count);
        }

        video::present({&prepare_commands});
        reset();
    }
} // namespace renderer

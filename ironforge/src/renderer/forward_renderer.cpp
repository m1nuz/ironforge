#include "forward_renderer.hpp"

namespace renderer {
    forward_renderer::forward_renderer() {
        application::debug(application::log_category::render, "% %\n", "Create forward render", "version 1.00");

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
        UNUSED(material);
        // do nothing
    }

    auto forward_renderer::reset() -> void {
        prepare_commands.clear_color = glm::vec4(0.f, 0.f, 0.f, 0.f);
    }

    auto forward_renderer::present(const glm::mat4 &proj, const glm::mat4 &view) -> void {
        UNUSED(proj), UNUSED(view);

        prepare_commands << video::gl::clear{};

        video::present({&prepare_commands});
        reset();
    }
} // namespace renderer

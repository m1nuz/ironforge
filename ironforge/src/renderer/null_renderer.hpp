#pragma once

#include <video/video.hpp>
#include <core/application.hpp>
#include <renderer/renderer.hpp>
#include <video/commands.hpp>

namespace renderer {
    struct null_instance : public instance {
        null_instance() {
            application::debug(application::log_category::render, "%\n", "Create null render");

            reset();
        }

        ~null_instance() {
            application::debug(application::log_category::render, "%\n", "Destroy null render");
        }

        virtual auto append(const phong::ambient_light &light) -> void {
            UNUSED(light);
            // do nothing
        }

        virtual auto append(const phong::directional_light &light) -> void {
            UNUSED(light);
            // do nothing
        }

        virtual auto append(const phong::point_light &light) -> void {
            UNUSED(light);
            // do nothing
        }

        virtual auto append(const phong::material &material) -> void {
            UNUSED(material);
            // do nothing
        }

        virtual auto append(const video::vertices_source &source, const video::vertices_draw &draw) -> void {
            UNUSED(source), UNUSED(draw);
        }

        virtual auto append(const glm::mat4 &model) -> void {
            UNUSED(model);
        }

        virtual auto append(const video::texture &cubemap, uint32_t flags) -> void {
            UNUSED(cubemap), UNUSED(flags);
        }

        virtual auto reset() -> void {
            commands.clear_color = glm::vec4(1.f, 1.f, 1.f, 1.f);
        }

        virtual auto present(const glm::mat4 &proj, const glm::mat4 &view) -> void {
            UNUSED(proj), UNUSED(view);            

            commands << vcs::clear{};

            video::present({&commands});
            reset();
        }

        video::gl::command_buffer commands;
    };
} // namespace renderer

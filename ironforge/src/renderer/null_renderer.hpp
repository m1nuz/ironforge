#pragma once

#include <video/video.hpp>
#include <core/journal.hpp>
#include <renderer/renderer.hpp>
#include <video/commands.hpp>

namespace renderer {
    struct null_instance : public instance {
        null_instance() {
            game::journal::debug(game::journal::_RENDER, "%", "Create null render");

            reset();
        }

        ~null_instance() {
            game::journal::debug(game::journal::_RENDER, "%", "Destroy null render");
        }

        virtual auto set_flags(const uint32_t flags) -> void {
            (void)flags;
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

        virtual auto append(const video::texture &, const uint32_t flags) -> void {
            (void)flags;
        }

        auto dispath(video::instance_t &vi, const ui::draw_command_t &c) -> void {
            UNUSED(vi), UNUSED(c);
        }

        virtual auto reset() -> void {
            commands.clear_color = glm::vec4(0.f, 0.f, 0.f, 0.f);
        }

        virtual auto present(video::instance_t &vi, const glm::mat4 &proj, const glm::mat4 &view) -> void {
            UNUSED(proj), UNUSED(view);            

            commands << vcs::clear{};

            video::present(vi, {&commands});
            reset();
        }

        virtual auto presented_texture() -> video::texture {
            return {};
        }

        video::gl::command_buffer commands;
    };
} // namespace renderer

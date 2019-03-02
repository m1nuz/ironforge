#pragma once

#include <video/video.hpp>
#include <core/journal.hpp>
#include <renderer/renderer.hpp>
#include <video/commands.hpp>

namespace renderer {
    struct null_instance final : public instance {
        null_instance() {
            game::journal::debug(game::journal::_RENDER, "%", "Create null render");

            reset();
        }

        ~null_instance() override {
            game::journal::debug(game::journal::_RENDER, "%", "Destroy null render");
        }

        virtual auto append(const phong::ambient_light &light) -> void override {
            UNUSED(light);
            // do nothing
        }

        virtual auto append(const phong::directional_light &light) -> void override {
            UNUSED(light);
            // do nothing
        }

        virtual auto append(const phong::point_light &light) -> void override {
            UNUSED(light);
            // do nothing
        }

        virtual auto append(const phong::material &material) -> void override {
            UNUSED(material);
            // do nothing
        }

        virtual auto append(const video::vertices_source &source, const video::vertices_draw &draw, const glm::mat4 &model) -> void override {
            UNUSED(source), UNUSED(draw), UNUSED(model);
        }

        virtual auto append(const video::texture &, const uint32_t flags) -> void override {
            (void)flags;
        }

        auto dispath(video::instance_t &vi, const ui::draw_command_t &c) -> void override {
            UNUSED(vi), UNUSED(c);
        }

        virtual auto reset() -> void override {
            commands.clear_color = glm::vec4(0.f, 0.f, 0.f, 0.f);
        }

        virtual auto present(video::instance_t &vi, const glm::mat4 &proj, const glm::mat4 &view) -> void override {
            UNUSED(proj), UNUSED(view);            

            commands << vcs::clear{};

            video::present(vi, {&commands});
            reset();
        }

        virtual auto presented_texture() -> video::texture override {
            return {};
        }

        video::gl::command_buffer commands;
    };
} // namespace renderer

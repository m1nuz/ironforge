#pragma once

#include <video/video.hpp>
#include <core/application.hpp>
#include <renderer/renderer.hpp>

namespace renderer {
    struct null_instance : public instance {
        null_instance() {
            application::debug(application::log_category::render, "%\n", "Create null render");

            reset();
        }

        ~null_instance() {
            application::debug(application::log_category::render, "%\n", "Destroy null render");
        }

        virtual auto set(const phong::ambient_light &light) -> void {
            UNUSED(light);
            // do nothing
        }

        virtual auto set(const phong::directional_light &light) -> void {
            UNUSED(light);
            // do nothing
        }

        virtual auto append(const phong::point_light &light) -> void {
            UNUSED(light);
            // do nothing
        }

        virtual auto reset() -> void {
            // do nothing
        }

        virtual auto present(const glm::mat4 &proj, const glm::mat4 &view) -> void {
            UNUSED(proj), UNUSED(view);

            glClearColor(1, 1, 1, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            video::present();
            reset();
        }
    };
} // namespace renderer

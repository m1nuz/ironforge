#pragma once

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

        }

        virtual auto set(const phong::directional_light &light) -> void {

        }

        virtual auto append(const phong::point_light &light) -> void {

        }

        virtual auto reset() -> void {
        }

        virtual auto present(const glm::mat4 &proj, const glm::mat4 &view) -> void {

            reset();
        }
    };
} // namespace renderer

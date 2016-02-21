#pragma once

#include <core/application.hpp>
#include <renderer/renderer.hpp>

namespace renderer {
    struct null_instance : public instance {
        null_instance() {
            application::debug(application::log_category::render, "%\n", "Create null render");
        }

        ~null_instance() {
            application::debug(application::log_category::render, "%\n", "Destroy null render");
        }
    };
} // namespace renderer

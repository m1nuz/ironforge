

#include <core/application.hpp>

namespace application {

    log_priority categories[];
    const char *category_names[] = {
        "Application",
        "System",
        "Audio",
        "Video",
        "Render",
        "Input",
    };

    _log_init_categories log_init_categories;

    auto init(const std::string &title) -> result {
        return result::failure;
    }

    auto exec() -> result {
        return result::failure;
    }

    auto cleanup() -> void {

    }
} // namespace application

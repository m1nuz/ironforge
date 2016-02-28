#include <core/application.hpp>

#include "model.hpp"

namespace scene {
    auto create_model(const model_info &info) -> model_instance* {
        application::debug(application::log_category::scene, "Create model %\n", info.name);

        return nullptr;
    }
} // namespace scene

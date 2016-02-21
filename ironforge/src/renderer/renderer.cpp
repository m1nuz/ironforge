#include <renderer/renderer.hpp>
#include "null_renderer.hpp"

namespace renderer {
    auto create_null_renderer() -> std::unique_ptr<renderer::instance> {
        return make_unique<null_instance>();
    }
} // namespace renderer

#include <renderer/renderer.hpp>
#include "null_renderer.hpp"
#include "forward_renderer.hpp"

namespace renderer {
    auto create_null_renderer() -> std::unique_ptr<renderer::instance> {
        return make_unique<null_instance>();
    }

    auto create_forward_renderer() -> std::unique_ptr<instance> {
        return make_unique<forward_renderer>();
    }
} // namespace renderer

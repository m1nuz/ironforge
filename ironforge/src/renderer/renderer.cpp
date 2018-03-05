
#include <renderer/renderer.hpp>
#include "null_renderer.hpp"
#include "forward_renderer.hpp"

namespace renderer {
    auto create_renderer(std::string_view type, video::instance_t &vi, const json &info) -> std::unique_ptr<instance> {
        using namespace game;

        if (type.empty())
            return std::make_unique<null_instance>();

        if (type == "null")
            return std::make_unique<null_instance>();

        if (type == "forward")
            return std::make_unique<forward_renderer>(vi, info);

        if (type == "deffered") {
            journal::warning(journal::_RENDER, "%", "Unsupported");
            return std::make_unique<null_instance>();
        }

        journal::warning(journal::_RENDER, "%", "Unknown render type");

        // TODO: replace with error_code
        return std::make_unique<null_instance>();
    }
} // namespace renderer

#pragma once

#include <memory>
#include <ironforge_common.hpp>

namespace renderer {
    struct instance {
        // interface
        virtual ~instance() = default;
    };

    auto create_null_renderer() -> std::unique_ptr<instance>;
    //auto create_forward_renderer() -> std::unique_ptr<instance>;
    //auto create_deffered_renderer() -> std::unique_ptr<instance>;
} // namespace renderer

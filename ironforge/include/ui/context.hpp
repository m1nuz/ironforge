#pragma once

#include <vector>
#include <ui/command.hpp>
#include <ui/widgets.hpp>

namespace ui {
    struct context {
        std::vector<draw_command_t> commands;
    };
} // namespace ui

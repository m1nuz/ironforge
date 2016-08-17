#pragma once

#include <queue>
#include <ui/command.hpp>
#include <ui/widgets.hpp>

namespace ui {
    struct context {
        std::priority_queue<command> commands;
    };
} // namespace ui

#pragma once

#include <queue>
#include <list>
#include <ui/command.hpp>
#include <ui/widgets.hpp>

namespace ui {
    struct context {
        std::list<command> commands;
        //std::priority_queue<command> commands;
    };
} // namespace ui

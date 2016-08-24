#pragma once

#include <queue>
#include <list>
#include <ui/command.hpp>
#include <ui/widgets.hpp>

namespace ui {
    struct context {
        //std::priority_queue<command> commands;
        std::list<command> commands;
    };
} // namespace ui

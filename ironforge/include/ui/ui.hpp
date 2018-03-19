#pragma once

#include <memory>
#include <SDL2/SDL_events.h>
#include <ui/context.hpp>
#include <ui/command.hpp>
#include <ui/widgets.hpp>

namespace video {
    struct instance_type;
    typedef instance_type instance_t;
}

namespace ui {
    struct context;
    struct renderer;

    /*auto create_context() -> std::unique_ptr<context>;

    auto append(std::unique_ptr<context> &ctx, const draw_command_t &com) -> void;

    auto process_event(context &ctx, const SDL_Event &event) -> void;
    auto present(std::unique_ptr<context> &ctx, const video::instance_t &vi, const std::function<void (const command &c)> &dispath) -> void;*/
} // namespace ui

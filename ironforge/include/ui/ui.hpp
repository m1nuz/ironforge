#pragma once

#include <memory>
#include <SDL2/SDL_events.h>
#include <ui/context.hpp>
#include <ui/command.hpp>
#include <ui/widgets.hpp>

namespace ui {
    struct context;
    struct renderer;

    auto create_context() -> std::unique_ptr<context>;

    auto append(std::unique_ptr<context> &ctx, const command &com) -> void;

    auto process_event(context &ctx, const SDL_Event &event) -> void;
    auto present(std::unique_ptr<context> &ctx, const std::function<void (const command &c)> &dispath) -> void;
} // namespace ui

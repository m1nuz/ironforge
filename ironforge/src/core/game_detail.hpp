#pragma once

#include <memory>
#include <vector>
#include <string>

#include <SDL2/SDL_events.h>

namespace game {
    struct instance {
        instance() : current{0} {
        }

        auto current_scene() -> std::unique_ptr<scene::instance>& {
            if (scenes.empty())
                scenes.push_back(scene::empty());

            return scenes[current];
        }

        video::instance_t vi;
        std::vector<std::unique_ptr<scene::instance>>   scenes;
        std::unique_ptr<renderer::instance>             render;
        std::unique_ptr<ui::context>                    uic;
        unsigned int                                    current = 0;
        bool                                            running = true;
    };

    constexpr auto get_string(const result r) -> const char *;
} // namespace game

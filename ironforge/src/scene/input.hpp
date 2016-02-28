#pragma once

#include <SDL2/SDL_events.h>

namespace scene {
    struct input_action {
        const char *key_down;
        const char *key_up;

        SDL_Keycode key;
    };

    struct input_instance {

    };
} // namespace scene

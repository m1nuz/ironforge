#pragma once

#include <core/assets.hpp>
#include <SDL2/SDL_rwops.h>
#include <core/common.hpp>

[[nodiscard]] auto read_text(SDL_RWops *rw, assets::text_data& text) -> int32_t;
[[nodiscard]] auto read_shader_text(SDL_RWops *rw, assets::text_data& text) -> int32_t;

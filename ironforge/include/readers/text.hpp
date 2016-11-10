#pragma once

#include <core/assets.hpp>
#include <SDL2/SDL_rwops.h>
#include <ironforge_common.hpp>

__must_ckeck auto read_text(SDL_RWops *rw, assets::text_data& text) -> int32_t;
__must_ckeck auto read_shader_text(SDL_RWops *rw, assets::text_data& text) -> int32_t;

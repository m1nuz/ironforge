#pragma once

#include <core/assets.hpp>
#include <SDL2/SDL_rwops.h>
#include <ironforge_common.hpp>

__must_ckeck auto read_targa(SDL_RWops *rw, assets::image_data& image) -> int32_t;

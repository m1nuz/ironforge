#pragma once

#include <core/assets.hpp>
#include <SDL2/SDL_rwops.h>
#include <core/common.hpp>

[[nodiscard]] auto read_targa(SDL_RWops *rw, assets::image_data& image) -> int32_t;

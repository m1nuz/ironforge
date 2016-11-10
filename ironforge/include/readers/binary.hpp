#pragma once

#include <core/assets.hpp>
#include <SDL2/SDL_rwops.h>
#include <ironforge_common.hpp>

__must_ckeck auto read_binary(SDL_RWops *rw, assets::binary_data &data) -> int32_t;

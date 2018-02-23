#pragma once

#include <core/assets.hpp>
#include <SDL2/SDL_rwops.h>
#include <core/common.hpp>

[[nodiscard]] auto read_binary(SDL_RWops *rw, assets::binary_data &data) -> int32_t;

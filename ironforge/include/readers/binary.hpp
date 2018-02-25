#pragma once

#include <core/assets.hpp>
#include <SDL2/SDL_rwops.h>

[[nodiscard]] auto read_binary(assets::instance_t &inst, SDL_RWops *rw) -> std::optional<assets::binary_data_t>;

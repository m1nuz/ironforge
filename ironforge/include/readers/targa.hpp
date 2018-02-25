#pragma once

#include <SDL2/SDL_rwops.h>
#include <core/assets.hpp>

[[nodiscard]] auto read_targa(assets::instance_t &inst, SDL_RWops *rw) -> std::optional<assets::image_data_t>;

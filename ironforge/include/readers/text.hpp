#pragma once

#include <core/assets.hpp>
#include <SDL2/SDL_rwops.h>
#include <core/common.hpp>

[[nodiscard]] auto read_text(assets::instance_t &inst, SDL_RWops *rw) -> std::optional<assets::text_data_t>;
[[nodiscard]] auto read_shader_text(assets::instance_t &inst, SDL_RWops *rw) -> std::optional<assets::text_data_t>;

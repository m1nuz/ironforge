#pragma once

#include <vector>
#include <string>

#include <SDL2/SDL_rect.h>
#include <video/video.hpp>
#include <video/atlas.hpp>

namespace assets {
    struct instance_type;
    typedef instance_type instance_t;
}

namespace video {
    struct instance_type;
    typedef instance_type instance_t;

    struct font_info {
        font_info() = default;

        font_info(const std::string &font_name, const int sz, const std::string &charset)
            : name{font_name}, size{sz}, cache{charset} {

        }

        std::string name;
        int         size;
        std::string cache;
    };

    struct glyph {
        glyph() = default;
        uint16_t    ch = 0;
        SDL_Rect    rc = {};
        int         advance = 0;
        int         type = 0;
    };

    struct glyph_group {
        glyph_group() = default;
        int         type = 0;
        int         size = 0;
        int         lineskip = 0;
    };

    auto glyph_cache_build(instance_t &vi, assets::instance_t &asset, const std::vector<font_info> &fonts, atlas &_atlas) -> bool;
    auto glyph_cache_find(uint16_t ch, int type) -> glyph;

    auto glyph_cache_get_font_lineskip(int type) -> int;
    auto glyph_cache_get_font_size(int type) -> int;

    auto default_charset() -> const char *;
} // namespace video

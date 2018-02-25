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
    struct font_info {
        std::string name;
        int         size;
        std::string cache;
    };

    struct glyph {
        uint16_t    ch;
        SDL_Rect    rc;
        int         advance;
        int         type;
    };

    auto glyph_cache_build(assets::instance_t &asset, const std::vector<font_info> &fonts, atlas &_atlas) -> bool;
    auto glyph_cache_find(uint16_t ch, int type) -> glyph;

    auto glyph_cache_get_font_lineskip(int type) -> int;
    auto glyph_cache_get_font_size(int type) -> int;

    auto default_charset() -> const char *;
} // namespace video

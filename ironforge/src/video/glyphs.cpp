#include <vector>
#include <algorithm>

#include <SDL2/SDL_ttf.h>
#include <core/assets.hpp>
#include <video/glyphs.hpp>

namespace video {


    std::vector<glyph>          glyphs;
    std::vector<glyph_group>    glyph_groups;

    static bool glyph_compare(const glyph &a, const glyph &b) {
        if (a.ch == b.ch)
            return a.type < b.type;

        return a.ch < b.ch;
    }

    static auto glyph_cache_append(assets::instance_t &asset, const font_info &info, atlas &_atlas) -> void {
        const SDL_Color White = {255, 255, 255, 255};
        auto fb = assets::get_binary(asset, info.name);

        if (!fb)
            return; // TODO: error

        auto rw = SDL_RWFromConstMem(&fb.value()[0], fb.value().size());

        auto font = TTF_OpenFontRW(rw, SDL_TRUE, info.size);
        if (!font)
            return;

        // TODO: check font for null

        auto count = info.cache.size(); // TODO: make support utf8
        for (size_t i = 0; i < count; i++) {
            char ch[2] = {info.cache[i], 0};

            int minx, maxx, miny, maxy, advance;

            if (TTF_GlyphMetrics(font, ch[0], &minx, &maxx, &miny, &maxy, &advance) != -1) {
                //SDL_Surface *glyph = TTF_RenderUNICODE_Blended(font, (Uint16*)&ch[0], White);
                SDL_Surface *glyph = TTF_RenderUTF8_Blended(font, ch, White);

                SDL_Surface *rgba_glyph = SDL_ConvertSurfaceFormat(glyph, SDL_PIXELFORMAT_RGBA8888, 0);

                glyphs.push_back({static_cast<Uint16>(ch[0]), insert_surface(_atlas, rgba_glyph), advance, static_cast<int>(glyph_groups.size())});

                SDL_FreeSurface(glyph);
                SDL_FreeSurface(rgba_glyph);
            }
        }

        glyph_groups.push_back({static_cast<int>(glyph_groups.size()), TTF_FontHeight(font), TTF_FontLineSkip(font)});
    }

    auto glyph_cache_build(instance_t &vi, assets::instance_t &asset, const std::vector<font_info> &fonts, atlas &_atlas) -> bool {
        size_t glyph_cache_size = 0;
        int group_count = 0;

        for (const auto &f : fonts) {
            glyph_cache_size += f.cache.size();
            group_count++;
        }

        glyphs.reserve(glyph_cache_size);
        glyph_groups.reserve(group_count);

        for (const auto &fn : fonts)
            glyph_cache_append(asset, fn, _atlas);

        std::sort(glyphs.begin(), glyphs.end(), glyph_compare);

        return true;
    }

    auto glyph_cache_find(uint16_t ch, int type) -> glyph {
        glyph v = {ch, {0, 0, 0, 0}, 0, type};

        /*auto it = std::find_if(glyphs.cbegin(), glyphs.cend(), [&v] (const glyph &g) {
            return v.ch == g.ch && v.type == g.type;
        });*/

        auto it = std::lower_bound(glyphs.cbegin(), glyphs.cend(), v, glyph_compare);

        if (it != glyphs.end())
            return *it;

        return {0, {0, 0, 0, 0}, 0, 0};
    }

    auto glyph_cache_get_font_lineskip(int type) -> int {
        for (const auto &g : glyph_groups)
            if (g.type == type)
                return g.lineskip;

        return 0;
    }

    auto glyph_cache_get_font_size(int type) -> int {
        for (const auto &g : glyph_groups)
            if (g.type == type)
                return g.size;

        return 0;
    }

    auto default_charset() -> const char * {
        return " !\"#$%&'()*+,-./0123456789:;<=>?"
               "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
               "`abcdefghijklmnopqrstuvwxyz{|}~";
    }
} // namespace video

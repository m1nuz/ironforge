#include <vector>
#include <algorithm>

#include <SDL2/SDL_ttf.h>
#include <core/assets.hpp>
#include <video/glyphs.hpp>
#include <video/atlas.hpp>

namespace video {
    struct glyph_group {
        int         type;
        int         size;
        int         lineskip;
    };

    std::vector<glyph>          glyphs;
    std::vector<glyph_group>    glyph_groups;

    atlas                       glyphs_atlas;

    static bool glyph_compare(const glyph &a, const glyph &b) {
        if (a.ch == b.ch)
            return a.type < b.type;

        return a.ch < b.ch;
    }

    static auto glyph_cache_append(const font_info &info) -> void {
        const SDL_Color White = {255, 255, 255, 255};
        auto fb = assets::get_binary(info.name);

        auto rw = SDL_RWFromConstMem(fb.raw_memory, fb.size);

        auto font = TTF_OpenFontRW(rw, SDL_TRUE, info.size);

        auto count = info.cache.size(); // TODO: make support utf8
        for (size_t i = 0; i < count; i++) {
            char ch[2] = {info.cache[i], 0};

            int minx, maxx, miny, maxy, advance;

            if (TTF_GlyphMetrics(font, ch[0], &minx, &maxx, &miny, &maxy, &advance) != -1) {
                //SDL_Surface *glyph = TTF_RenderUNICODE_Blended(font, (Uint16*)&ch[0], White);
                SDL_Surface *glyph = TTF_RenderText_Blended(font, ch, White);

                SDL_Surface *rgba_glyph = SDL_ConvertSurfaceFormat(glyph, SDL_PIXELFORMAT_RGBA8888, 0);

                glyphs.push_back({static_cast<Uint16>(ch[0]), insert_surface(glyphs_atlas, rgba_glyph), advance, static_cast<int>(glyph_groups.size())});

                SDL_FreeSurface(glyph);
                SDL_FreeSurface(rgba_glyph);
            }
        }

        glyph_groups.push_back({static_cast<int>(glyph_groups.size()), TTF_FontHeight(font), TTF_FontLineSkip(font)});
    }

    auto glyph_cache_build(const std::vector<font_info> &fonts, int w, int h) -> bool {
        glyphs_atlas = create_atlas(w, h, 1);

        size_t glyph_cache_size = 0;
        int group_count = 0;

        for (const auto &f : fonts) {
            glyph_cache_size += f.cache.size();
            group_count++;
        }

        glyphs.reserve(glyph_cache_size);
        glyph_groups.reserve(group_count);

        for (const auto &fn : fonts)
            glyph_cache_append(fn);

        std::sort(glyphs.begin(), glyphs.end(), glyph_compare);

        make_texture_2d("glyphs-map", get_atlas_texture(glyphs_atlas));

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

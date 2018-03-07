#include <vector>
#include <algorithm>

#include <SDL2/SDL_ttf.h>
#include <core/assets.hpp>
#include <video/video.hpp>
#include <video/glyphs.hpp>

namespace video {
    static auto append_font(assets::instance_t &asset, std::unordered_map<std::string, font_t> &fonts, const font_info &info, atlas &_atlas) -> bool {
        const SDL_Color White = {255, 255, 255, 255};
        auto fb = assets::get_binary(asset, info.filename);

        if (!fb)
            return false;

        auto rw = SDL_RWFromConstMem(&fb.value()[0], fb.value().size());

        auto font = TTF_OpenFontRW(rw, SDL_TRUE, info.size);
        if (!font)
            return false;

        const auto count = info.cache.size(); // TODO: make support utf8

        std::unordered_map<uint16_t, glyph_rect_t> glyph_rects;
        glyph_rects.reserve(count);

        for (size_t i = 0; i < count; i++) {
            char ch[2] = {info.cache[i], 0};

            int minx, maxx, miny, maxy, advance;

            if (TTF_GlyphMetrics(font, ch[0], &minx, &maxx, &miny, &maxy, &advance) != -1) {
                //SDL_Surface *glyph = TTF_RenderUNICODE_Blended(font, (Uint16*)&ch[0], White);
                SDL_Surface *glyph = TTF_RenderUTF8_Blended(font, ch, White);

                SDL_Surface *rgba_glyph = SDL_ConvertSurfaceFormat(glyph, SDL_PIXELFORMAT_RGBA8888, 0);

                const auto rc = insert_surface(_atlas, rgba_glyph);

                glyph_rects.insert({static_cast<Uint16>(ch[0]), {rc.x, rc.y, rc.w, rc.h, advance}});

                SDL_FreeSurface(glyph);
                SDL_FreeSurface(rgba_glyph);
            }
        }

        fonts.insert({info.fontname, {TTF_FontHeight(font), TTF_FontLineSkip(font), glyph_rects}});

        return true;
    }

    auto build_fonts(instance_t &vi, assets::instance_t &asset, const std::vector<font_info> &fonts_info, atlas &_atlas) -> bool {
        if (fonts_info.empty())
            return false;

        std::unordered_map<std::string, font_t> fonts;
        fonts.reserve(fonts_info.size());

        for (const auto &f : fonts_info) {
            append_font(asset, fonts, f, _atlas);
        }

        if (fonts.empty())
            return false;

        vi.fonts = fonts;

        return true;
    }

    auto get_glyph_rect(const font_t &font, const uint16_t ch) -> std::optional<glyph_rect> {
        if (auto it = font.glyphs.find(ch); it != font.glyphs.end())
            return it->second;

        return {};
    }

    auto get_text_length(const font_t &font, std::string_view text) -> std::tuple<float, float> {
        const int adv_y = font.lineskip;
        const float spt = 2.f / video::screen.width;

        float px = 0, py = 0;

        for (size_t i = 0; i < text.size(); i++) {
            auto ch = text[i];
            if (ch == '\n') {
                px = 0;
                py += spt * adv_y * video::screen.aspect;
                continue;
            }

            auto glyph = get_glyph_rect(font, static_cast<uint16_t>(ch));
            if (!glyph) {
                game::journal::warning(game::journal::_RENDER, "%", "Glyph not found");
                continue;
            }

            px += glyph.value().advance * spt;
        }

        return {px, py == 0 ? (float)adv_y / video::screen.height * 2.f : py};
    }

    auto default_charset() noexcept -> const char * {
        return " !\"#$%&'()*+,-./0123456789:;<=>?"
               "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
               "`abcdefghijklmnopqrstuvwxyz{|}~";
    }
} // namespace video

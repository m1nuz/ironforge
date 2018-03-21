#include <vector>
#include <algorithm>

#include <SDL2/SDL_ttf.h>
#include <core/assets.hpp>
#include <video/video.hpp>
#include <video/glyphs.hpp>

#include <utility/utf.hpp>

namespace video {
    static auto make_font(assets::instance_t &asset, const font_info &info, atlas &_atlas) -> std::optional<font_t> {
        const SDL_Color White = {255, 255, 255, 255};
        auto fb = assets::get_binary(asset, info.filename);

        if (!fb)
            return {};

        auto rw = SDL_RWFromConstMem(&fb.value()[0], fb.value().size());

        auto font = TTF_OpenFontRW(rw, SDL_TRUE, info.size);
        if (!font)
            return {};

        const auto charset = utility::to_utf16(info.cache);

        std::unordered_map<uint16_t, glyph_rect_t> glyph_rects;
        glyph_rects.reserve(charset.size());

        for (const auto c : charset) {
            Uint16 ch[2] = {c, 0};

            int minx, maxx, miny, maxy, advance;

            if (TTF_GlyphMetrics(font, ch[0], &minx, &maxx, &miny, &maxy, &advance) != -1) {
                SDL_Surface *glyph = TTF_RenderUNICODE_Blended(font, (Uint16*)&ch[0], White);
                //SDL_Surface *glyph = TTF_RenderUTF8_Blended(font, ch, White);

                SDL_Surface *rgba_glyph = SDL_ConvertSurfaceFormat(glyph, SDL_PIXELFORMAT_RGBA8888, 0);

                const auto rc = insert_surface(_atlas, rgba_glyph);

                glyph_rects.insert({static_cast<Uint16>(ch[0]), {rc.x, rc.y, rc.w, rc.h, advance}});

                SDL_FreeSurface(glyph);
                SDL_FreeSurface(rgba_glyph);
            }
        }

        return font_t{TTF_FontHeight(font), TTF_FontLineSkip(font), glyph_rects};
    }

    auto build_fonts(instance_t &vi, assets::instance_t &asset, const std::vector<font_info> &fonts_info, atlas &_atlas) -> bool {
        if (fonts_info.empty())
            return false;

        std::unordered_map<std::string, size_t> fonts_map;
        std::vector<font_t> fonts;
        fonts_map.reserve(fonts_info.size());
        fonts.reserve(fonts_info.size());

        for (const auto &fi : fonts_info) {
            const auto f = make_font(asset, fi, _atlas);

            if (f)
            {
                fonts_map.emplace(fi.fontname, fonts.size());
                fonts.push_back(f.value());
            }
        }

        if (fonts_map.empty())
            return false;

        vi.fonts = fonts;
        vi.fonts_mapping = fonts_map;

        return true;
    }

    auto get_glyph_rect(const font_t &font, const uint16_t ch) -> std::optional<glyph_rect> {
        if (auto it = font.glyphs.find(ch); it != font.glyphs.end())
            return it->second;

        return {};
    }

    auto get_text_length(const font_t &font, std::string_view text) -> std::tuple<float, float> {
        const int adv_y = font.lineskip;
        const float spt = 1.f / video::screen.height;
        const auto correction = video::screen.aspect;

        float px = 0, py = 0;

        const auto unicode_text = utility::to_utf16(std::string{text});

        for (const auto ch : unicode_text) {
            if (ch == '\n') {
                px = 0;
                py += spt * adv_y * correction;
                continue;
            }

            auto glyph = get_glyph_rect(font, ch);
            if (!glyph) {
                game::journal::warning(game::journal::_RENDER, "Glyph % not found", ch);
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

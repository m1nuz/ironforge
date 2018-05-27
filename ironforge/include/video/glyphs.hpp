#pragma once

#include <vector>
#include <unordered_map>
#include <string>

#include <SDL2/SDL_rect.h>
#include <video/atlas.hpp>

namespace assets {
    struct instance_type;
    typedef instance_type instance_t;
} // namespace assets

namespace video {

    struct instance_type;
    typedef instance_type instance_t;

    struct font_info {
        font_info() = default;

        font_info(const std::string &_filename, const std::string &_fontname, const int sz, const std::string &charset)
            : filename{_filename}, fontname{_fontname}, size{sz}, cache{charset} {

        }

        std::string filename;
        std::string fontname;
        int         size;
        std::string cache;
    };

    typedef struct glyph_rect {
        glyph_rect() = default;
        int         x = 0;
        int         y = 0;
        int         w = 0;
        int         h = 0;
        int         advance = 0;
    } glyph_rect_t;

    struct font_type {
        int                                         size = 0;
        int                                         lineskip = 0;
        float                                       correction = 1.f;
        float                                       spt;
        std::unordered_map<uint16_t, glyph_rect_t>  glyphs;
    };

    typedef font_type font_t;

    ///
    /// \brief build_fonts
    /// \param vi
    /// \param asset
    /// \param fonts_info
    /// \param _atlas
    /// \return
    ///
    [[nodiscard]] auto build_fonts(instance_t &vi, assets::instance_t &asset, const std::vector<font_info> &fonts_info, atlas &_atlas) -> bool;

    ///
    /// \brief get_glyph_rect
    /// \param font
    /// \param ch
    /// \return
    ///
    [[nodiscard]] auto get_glyph_rect(const font_t &font, const uint16_t ch) -> std::optional<glyph_rect>;

    ///
    /// \brief get_text_length
    /// \param font
    /// \param text
    /// \return
    ///
    [[nodiscard]] auto get_text_length(const font_t &font, std::string_view text) -> std::tuple<float, float>;

    ///
    /// \brief default_charset
    ///
    [[nodiscard]] auto default_charset() noexcept -> const char *;

} // namespace video

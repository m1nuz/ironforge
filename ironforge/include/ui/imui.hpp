#pragma once

#include <cstdint>
#include <optional>
#include <variant>
#include <vector>
#include <functional>
#include <string>
#include <array>
#include <stack>

#include <SDL2/SDL_events.h>

#include <core/journal.hpp>
#include <ui/command.hpp>

namespace ui {
    enum layout_type {
        layout_none,
        layout_horizontal,
        layout_vertical
    };
} // namespace ui

namespace imui {

    using glm::vec2;

    enum special_keys {
        key_mouse_left      = 1 << 0,
        key_mouse_middle    = 1 << 1,
        key_mouse_right     = 1 << 2,
        key_enter           = 1 << 3,
        key_down            = 1 << 4,
        key_up              = 1 << 5,
        key_left            = 1 << 6,
        key_right           = 1 << 7,
        key_backspace       = 1 << 8,
    };

    enum frame_resizing_type {
        resizing_fixed,
        resizing_free,
        resizing_vertical_only,
        resizing_horizontal_only
    };

    enum frame_flags : uint32_t {
        frame_drag_flag             = 1 << 0,
        frame_dragging_flag         = 1 << 1,
        frame_header_flag           = 1 << 2,
        frame_collapse_flag         = 1 << 3,
    };

    typedef struct frame_state_type {
        frame_state_type() = default;

        float x = 0.f;
        float y = 0.f;
        float width = 0.f;
        float height = 0.f;
        float widget_x = 0.f;
        float widget_y = 0.f;
        ui::layout_type layout = ui::layout_vertical;
        uint32_t    flags = 0;
    } frame_state_t;

    struct box_style {
        box_style() = default;

        float width = 0.f;
        float height = 0.f;
        float border_width = 0.001;

        std::array<ui::color_t, 4> border_color = {0, 0, 0, 0};
        ui::color_t background_color = 0;
        ui::color_t foreground_color = 0;
        ui::color_t active_color = 0;
        ui::color_t focused_color = 0;
        ui::color_t text_color = 0;
        uint32_t align = 0;
        uint32_t font = 0;
    };

    ///
    /// \brief UI Context
    ///
    typedef struct context_type {
        context_type() = default;

        uint32_t    active_item = 0;
        uint32_t    hot_item = 0;
        uint32_t    focused_item = 0;

        int32_t     mouse_x = 0;
        int32_t     mouse_y = 0;
        uint32_t    all_keys = 0;
        char32_t    character = 0;
        std::string input_text;

        uint32_t    widget_id = 0;

        uint32_t    screen_width = 0;
        uint32_t    screen_height = 0;
        float       aspect_ratio = 0;

        std::stack<frame_state_t*> frames;
        ui::layout_type frame_layout = ui::layout_none;

        std::vector<uint32_t> fonts;
        std::unordered_map<std::string, box_style> styles;

        std::function<std::tuple<float, float> (const uint32_t font, const std::string &text)> get_text_lenght;
        std::function<void (const ui::draw_command_t &c)> dispather;
        std::vector<ui::draw_command_t> commands;
    } context_t;

    inline auto resize(context_t &ctx, int32_t size_w, int32_t size_h) {
        ctx.screen_width = static_cast<uint32_t>(size_w);
        ctx.screen_height = static_cast<uint32_t>(size_h);
        ctx.aspect_ratio = static_cast<float>(ctx.screen_width) / static_cast<float>(ctx.screen_height);
    }

    inline auto handle_input(context_t &ctx, const SDL_Event &event) -> void {
        switch (event.type) {
        case SDL_WINDOWEVENT:
            switch (event.window.event) {
            case SDL_WINDOWEVENT_SHOWN:
            {
                auto window = SDL_GetWindowFromID(event.window.windowID);
                auto drawable_w = 0;
                auto drawable_h = 0;
                SDL_GL_GetDrawableSize(window, &drawable_w, &drawable_h);

                resize(ctx, drawable_w, drawable_h);

            }
                break;

            case SDL_WINDOWEVENT_RESIZED:
                resize(ctx, event.window.data1, event.window.data2);
                break;
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT)
                ctx.all_keys |= key_mouse_left;

            if (event.button.button == SDL_BUTTON_MIDDLE)
                ctx.all_keys |= key_mouse_middle;

            if (event.button.button == SDL_BUTTON_RIGHT)
                ctx.all_keys |= key_mouse_right;

            //context.mouse_x = event.button.x;
            //context.mouse_y = event.button.y;
            break;

        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT)
                ctx.all_keys &= ~key_mouse_left;

            if (event.button.button == SDL_BUTTON_MIDDLE)
                ctx.all_keys &= ~key_mouse_middle;

            if (event.button.button == SDL_BUTTON_RIGHT)
                ctx.all_keys &= ~key_mouse_right;

            //context.mouse_x = event.button.x;
            //context.mouse_y = event.button.y;
            break;

        case SDL_MOUSEMOTION:
            ctx.mouse_x = event.motion.x;
            ctx.mouse_y = event.motion.y;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_RETURN:
                ctx.all_keys |= key_enter;
                break;
            case SDLK_BACKSPACE:
                ctx.all_keys |= key_backspace;
                break;
            }
            break;

        case SDL_KEYUP:
            switch (event.key.keysym.sym) {
            case SDLK_RETURN:
                ctx.all_keys &= ~key_enter;
                break;
            case SDLK_BACKSPACE:
                ctx.all_keys &= ~key_backspace;
                break;
            }
            break;

        case SDL_TEXTINPUT:
            ctx.input_text.clear();
            ctx.input_text = event.text.text;
            break;
        }
    }

    inline auto mouse_in_rect(const context_t &ctx, const float x, const float y, const float w, const float h) -> bool {
        const auto mx = - 0.5f + static_cast<float>(ctx.mouse_x) / ctx.screen_width;
        const auto my =   0.5f - static_cast<float>(ctx.mouse_y) / ctx.screen_height;

        return (mx >= x) && (mx <= w + x) && (my >= y) && (my <= ctx.aspect_ratio * h + y);
    }

    inline auto is_button_down(const context_t &ctx, special_keys key) -> bool {
        return ctx.all_keys & key;
    }

    inline auto create(const video::instance_t &vi, const std::unordered_map<std::string, box_style> &styles) -> std::optional<context_t> {
        std::vector<uint32_t> fonts;
        for (size_t i = 0; i < vi.fonts.size(); i++)
            fonts.push_back(i);

        context_t ctx;
        ctx.fonts = fonts;
        ctx.styles = styles;

        ctx.input_text.reserve(32);

        return ctx;
    }

    inline auto present(context_t &ctx) -> void {
        while (!ctx.commands.empty()) {
            const auto c = ctx.commands.back();

            ctx.dispather(c);

            ctx.commands.pop_back();
        }

        ctx.commands.clear();

        ctx.widget_id = 0;
        ctx.hot_item = 0;

        if (ctx.focused_item == 0)
            ctx.input_text.clear();
    }

    namespace detail {

    } // namespace detail

    namespace draw {

        inline auto box_only(context_t &ctx, const vec2 &pos, const vec2 &size, const box_style &boxstyle) -> void {
            const auto aspect_ratio = static_cast<float>(ctx.screen_width) / static_cast<float>(ctx.screen_height);
            const auto bx = pos.x;
            const auto by = pos.y;
            const auto bw = size.x;
            const auto bh = size.y * aspect_ratio;

            ui::draw_commands::draw_rect rect;
            rect.color = boxstyle.background_color;
            rect.x = bx;
            rect.y = by;
            rect.w = bw;
            rect.h = bh;

            ctx.commands.push_back(rect);

            if (boxstyle.border_width > 0.f) {
                ui::draw_commands::draw_line line;
                line.color = boxstyle.border_color[0];
                line.w = boxstyle.border_width;

                line.x0 = bx;
                line.x1 = bx + bw;
                line.y0 = by;
                line.y1 = by;

                ctx.commands.push_back(line);

                line.color = boxstyle.border_color[1];
                line.x0 = bx;
                line.x1 = bx;
                line.y0 = by + bh;
                line.y1 = by;

                ctx.commands.push_back(line);

                line.color = boxstyle.border_color[2];
                line.x0 = bx;
                line.x1 = bx + bw;
                line.y0 = by + bh;
                line.y1 = by + bh;

                ctx.commands.push_back(line);

                line.color = boxstyle.border_color[3];
                line.x0 = bx + bw;
                line.x1 = bx + bw;
                line.y0 = by + bh;
                line.y1 = by;

                ctx.commands.push_back(line);
            }
        }

        inline auto text_only(context_t &ctx, const vec2 &pos, const vec2 &size, const std::string &text, const box_style &boxstyle) -> void {
            const auto aspect_ratio = static_cast<float>(ctx.screen_width) / static_cast<float>(ctx.screen_height);
            const auto bx = pos.x;
            const auto by = pos.y;
            const auto bw = size.x;
            const auto bh = size.y * aspect_ratio;

            ui::draw_commands::draw_text t;
            t.x = bx;
            t.y = by;
            t.w = bw;
            t.h = bh;
            t.font = boxstyle.font;
            t.text = text;
            t.color = boxstyle.text_color;
            t.align = boxstyle.align;

            ctx.commands.push_back(t);
        }

        inline auto text_box(context_t &ctx, const vec2 &pos, const vec2 &size, const std::string &text, const box_style &boxstyle) -> void {
            const auto aspect_ratio = static_cast<float>(ctx.screen_width) / static_cast<float>(ctx.screen_height);
            const auto bx = pos.x;
            const auto by = pos.y;
            const auto bw = size.x;
            const auto bh = size.y * aspect_ratio;

            ui::draw_commands::draw_text textbox;
            textbox.x = bx;
            textbox.y = by;
            textbox.w = bw;
            textbox.h = bh;
            textbox.font = boxstyle.font;
            textbox.text = text;
            textbox.color = boxstyle.text_color;
            textbox.align = boxstyle.align;

            ctx.commands.push_back(textbox);

            box_only(ctx, pos, size, boxstyle);
        }
    } // namespace draw

    namespace widget {
        inline auto get_id(context_t &ctx) -> uint32_t {
            return ++ctx.widget_id;
        }

        inline auto init_widget(context_t &ctx, const box_style& style) -> std::tuple<uint32_t, bool, float, float, float, float> {
            auto frm = ctx.frames.top();

            const auto padding = 0.05f;
            const auto w = frm->width * (1.f - padding);
            const auto h = style.height;
            const auto x = frm->widget_x + frm->width * padding * 0.5f;
            const auto y = frm->widget_y - (h + frm->height * padding * 0.5f - frm->height) * ctx.aspect_ratio;

            frm->widget_y += - (h + frm->height * padding * 0.5f) * ctx.aspect_ratio;

            const auto id = get_id(ctx);

            const auto inside = mouse_in_rect(ctx, x, y, w, h);

            auto result = false;

            if (inside) {
                ctx.hot_item = id;
            } else if (ctx.focused_item == id && is_button_down(ctx, key_mouse_left))
                ctx.focused_item = 0;

            if (ctx.active_item == id && !is_button_down(ctx, key_mouse_left)) {
                if (inside) {
                    result = true;
                    ctx.hot_item = id;
                } else {
                    ctx.hot_item = 0;
                }

                ctx.active_item = 0;
            } else if (ctx.hot_item == id) {
                if (is_button_down(ctx, key_mouse_left)) {
                    ctx.focused_item = id;
                    ctx.active_item = id;
                }
            }

            return {id, result, x, y, w, h};
        }

        inline auto frame_begin(context_t &ctx, frame_state_t *fs, const std::string& header_style_name) -> void {
            //const auto spt_x = 1.f / ctx.screen_width;
            //const auto spt_y = 1.f / ctx.screen_height;

            if (fs->height == 0.f /*&& !(fs->flags & frame_dragging_flag)*/) {
                fs->y = -0.499f;
                fs->height = 2.f * 0.499f / ctx.aspect_ratio;
            }

            fs->widget_x = fs->x;
            fs->widget_y = fs->y;

            const auto padding = 0.05f - 0.006f;
            const auto header_size = 0.024f;
            const auto hw = fs->width/* * (1.f - padding)*/;
            const auto hh = header_size;
            const auto hx = fs->widget_x /*+ fs->width * padding * 0.5f*/;
            const auto hy = fs->widget_y - (hh + /*fs->height * padding * 0.5f*/ - fs->height) * ctx.aspect_ratio;

//            const auto marker_size = 0.01f;
//            const auto mkx = fs->x + fs->width - marker_size;
//            const auto mky = fs->y;
//            const auto mkw = marker_size;
//            const auto mkh = marker_size * ctx.aspect_ratio;
//            const auto inside_marker = mouse_in_rect(ctx, mkx, mky, mkw, mkh);
            //game::journal::info(game::journal::_UI, "% % % %", mkx, mky, mkw, mkh);

            const auto inside_header = mouse_in_rect(ctx, hx, hy, hw, hh);

            if (inside_header && is_button_down(ctx, key_mouse_left))
                fs->flags |= frame_dragging_flag;

            if (!is_button_down(ctx, key_mouse_left))
                fs->flags &= ~frame_dragging_flag;

            if (fs->flags & frame_dragging_flag)
            {
                const auto mx = - 0.5f + static_cast<float>(ctx.mouse_x) / ctx.screen_width;
                const auto my =   0.5f - static_cast<float>(ctx.mouse_y) / ctx.screen_height;
                fs->x = mx - fs->width * 0.5f;
                fs->y = my - (fs->height - header_size * 0.5f) * ctx.aspect_ratio;
                fs->widget_x = fs->x;
                fs->widget_y = fs->y;
            }

            /*if (!is_button_down(ctx, key_mouse_left))
                fs->flags &= ~frame_dragging_flag;*/

            if (fs->flags & frame_header_flag)
            {
                auto it = ctx.styles.find(header_style_name);
                if (it != ctx.styles.end())
                {
                    const auto& style = it->second;

                    //const auto _hw = fs->width * (1.f - padding);
                    //const auto _hh = header_size;
                    const auto _hx = fs->widget_x /*+ fs->width * padding * 0.5f*/;
                    const auto _hy = fs->widget_y - (hh /*+ fs->height * padding * 0.5f*/ - fs->height) * ctx.aspect_ratio;

                    box_style hs;

                    hs.font = ctx.fonts[0];
                    hs.text_color = style.text_color;
                    hs.border_width = 0.0;
                    hs.background_color = style.background_color;
                    hs.border_color = {0, 0, 0, 0};

                    hs.align = ui::align_horizontal_left | ui::align_vertical_center;
                    draw::text_only(ctx, {_hx + padding * 0.25, _hy}, {hw - padding * 0.5, hh}, "▼", hs);

                    hs.align = ui::align_center;
                    draw::text_box(ctx, {_hx, _hy}, {hw, hh}, "TITLE", hs);

                    fs->widget_y += - (hh + fs->height * padding * 0.5f) * ctx.aspect_ratio;
                }
            }

            ctx.frames.push(fs);
        }

        inline auto frame_end(context_t &ctx, const std::string &style_name) -> void {
            if (ctx.frames.empty())
                return;

            auto it = ctx.styles.find(style_name);
            if (it == ctx.styles.end())
                return;

            const auto& style = it->second;

            const auto marker_size = 0.01f;

            auto* fs = ctx.frames.top();

            ui::draw_commands::draw_rect rect;
            rect.color = 0x3e4249ff;
            rect.x = fs->x + fs->width - marker_size;
            rect.y = fs->y;
            rect.w = marker_size;
            rect.h = marker_size * ctx.aspect_ratio;

            ctx.commands.push_back(rect);

            box_style boxstyle;
            boxstyle.border_width = style.border_width;
            boxstyle.background_color = style.background_color;
            boxstyle.border_color = style.border_color;

            draw::box_only(ctx, {fs->x, fs->y}, {fs->width, fs->height}, boxstyle);

            ctx.frames.pop();
        }

        ///
        /// \brief Provides button widget
        /// \param[in] ctx UI Context
        /// \param[in] text Text shown with button
        /// \param[in] style_name Style name
        /// \return Return true in case activated, otherwise false.
        ///
        [[nodiscard]] inline auto button(context_t &ctx, const std::string &text, const std::string &style_name) -> bool {
            const auto it = ctx.styles.find(style_name);
            if (it == ctx.styles.cend())
                return false;

            const auto& style = it->second;

            auto [id, result, bx, by, bw, bh] = init_widget(ctx, style);

            const auto background_color = ctx.active_item == id ? style.active_color : (ctx.hot_item == id ? style.focused_color : style.background_color);

            box_style boxstyle;
            boxstyle.align = ui::align_center;
            boxstyle.font = ctx.fonts[0];
            boxstyle.text_color = style.text_color;
            boxstyle.border_width = style.border_width;
            boxstyle.background_color = background_color;
            boxstyle.border_color = style.border_color;

            draw::text_box(ctx, {bx, by}, {bw, bh}, text, boxstyle);

            return result;
        }

        ///
        /// \brief Provides edit box widget
        /// \param[in] ctx UI Context
        /// \param[inout] text Text edited
        /// \param[in] style_name Style name
        /// \return Return true in case activated, otherwise false.
        ///
        [[nodiscard]] inline auto edit_box(context_t &ctx, std::string &text, const std::string &style_name) -> bool {
            const auto it = ctx.styles.find(style_name);
            if (it == ctx.styles.cend())
                return false;

            const auto& style = it->second;

            auto [id, result, bx, by, bw, bh] = init_widget(ctx, style);

            if (ctx.focused_item == id) {

                if (!ctx.input_text.empty()) {
                    text += ctx.input_text;
                    ctx.input_text.clear();
                }

                if (ctx.all_keys & key_backspace) {
                    if (!text.empty()) {
                        text.pop_back();
                        ctx.all_keys &= ~key_backspace;
                    }
                }
            }

            auto [tw, th] = ctx.get_text_lenght(ctx.fonts[0], text);

            (void)th;

            const auto background_color = ctx.active_item == id ? style.active_color : (ctx.hot_item == id ? style.focused_color : style.background_color);

            box_style boxstyle;
            boxstyle.align = ui::align_horizontal_left;
            boxstyle.font = ctx.fonts[0];
            boxstyle.text_color = style.text_color;
            boxstyle.border_width = style.border_width;
            boxstyle.background_color = background_color;
            boxstyle.border_color = style.border_color;

            draw::text_box(ctx, {bx, by}, {bw, bh}, text, boxstyle);

            const auto carret_size = 0.0015;
            const auto carret_color = 0xffffffff;

            if (ctx.focused_item == id) {
                box_style carret_style;
                carret_style.background_color = carret_color;

                draw::box_only(ctx, {bx + tw * 0.5 + carret_size, by}, {carret_size, bh * 0.9}, carret_style);
            }

            return result;
        }


        ///
        /// \brief Provides slider widget
        /// \param[in] ctx UI Context
        /// \param[inout] value Value changed
        /// \param[in] max_value Maximum value
        /// \param[in] text Shown text
        /// \param[in] style_name Style name
        /// \return Return true in case activated, otherwise false.
        ///
        [[nodiscard]] inline auto slider(context_t &ctx, int32_t& value, const int32_t max_value, const std::string &text, const std::string &style_name) -> bool {
            const auto it = ctx.styles.find(style_name);
            if (it == ctx.styles.cend())
                return false;

            const auto& style = it->second;

            const auto [id, result, bx, by, bw, bh] = init_widget(ctx, style);

            const auto mw = bw * 0.15;
            const auto mx = bx + ((bw - mw) / max_value) * value;

            const auto inside_marker = mouse_in_rect(ctx, mx, by, mw, bh);

            if (ctx.active_item == id && is_button_down(ctx, key_mouse_left))
            {
                const auto mouse_x = - 0.5f + static_cast<float>(ctx.mouse_x) / ctx.screen_width;
                value = (mouse_x - bx) * max_value / bw;
                value = std::clamp(value, 0, max_value);
            }

            box_style boxstyle;
            boxstyle.align = ui::align_center;
            boxstyle.font = ctx.fonts[0];
            boxstyle.text_color = style.text_color;
            boxstyle.border_width = style.border_width;
            boxstyle.background_color = style.background_color;
            boxstyle.border_color = style.border_color;

            box_style markerstyle;
            markerstyle.align = ui::align_center;
            markerstyle.font = ctx.fonts[0];
            markerstyle.text_color = style.text_color;
            markerstyle.border_width = style.border_width;
            markerstyle.background_color = inside_marker ? style.active_color : 0x3e4249ff;
            markerstyle.border_color = style.border_color;

            draw::text_box(ctx, {bx, by}, {bw, bh}, text, boxstyle);
            draw::box_only(ctx, {mx, by}, {mw, bh}, markerstyle);

            return result;
        }

        ///
        /// \brief Provides a horizontal or vertical progress bar.
        /// \param[in] ctx UI Context
        /// \param[in] value Current progress bar value
        /// \param[in] max_value Maximum progress bar value
        /// \param[in] text Text shown with the progress bar
        /// \param[in] style_name Style name
        /// \return Return true in case activated, otherwise false.
        ///
        [[nodiscard]] inline auto progress_bar(context_t &ctx, const int32_t value, const int32_t max_value, const std::string &text, const std::string &style_name) -> bool {
            const auto it = ctx.styles.find(style_name);
            if (it == ctx.styles.cend())
                return false;

            const auto& style = it->second;

            const auto [id, result, bx, by, bw, bh] = init_widget(ctx, style);

            const auto mw = (static_cast<float>(value) / max_value) * bw;

            box_style boxstyle;
            boxstyle.align = ui::align_center;
            boxstyle.font = ctx.fonts[0];
            boxstyle.text_color = style.text_color;
            boxstyle.border_width = style.border_width;
            boxstyle.background_color = style.background_color;
            boxstyle.border_color = style.border_color;

            const auto progress_bar_color = ctx.active_item == id ? style.active_color : style.foreground_color;

            box_style progressstyle;
            progressstyle.align = 0;
            progressstyle.font = ctx.fonts[0];
            progressstyle.text_color = style.text_color;
            progressstyle.border_width = style.border_width;
            progressstyle.background_color = progress_bar_color;
            progressstyle.border_color = style.border_color;

            draw::text_box(ctx, {bx, by}, {bw, bh}, text, boxstyle);
            draw::box_only(ctx, {bx, by}, {mw, bh}, progressstyle);

            return result;
        }

        ///
        /// \brief Provides list box widget
        /// \param ctx UI Context
        /// \param elements Items for shown list
        /// \param max_elements Maximum shown elements
        /// \param pos Selected item
        /// \param scroll Scroll position
        /// \param style_name Style name
        /// \return Return true in case activated, otherwise false.
        ///
        [[nodiscard]] inline auto list_box(context_t &ctx, const std::vector<std::string> elements, const size_t max_elements, size_t& pos, size_t& scroll, const std::string &style_name) -> bool {
            const auto it = ctx.styles.find(style_name);
            if (it == ctx.styles.cend())
                return false;

            (void)pos;

            const auto& style = it->second;

            const size_t start_pos = scroll < elements.size() - max_elements ? scroll : elements.size() - max_elements;
            const size_t end_pos = start_pos + max_elements > elements.size() ? elements.size() : start_pos + max_elements;

            bool res = false;
            for (size_t item = start_pos; item < end_pos; item++)
            {
                const auto &item_text = elements[item];

                const auto [id, result, bx, by, bw, bh] = init_widget(ctx, style);

                res &= result;

                box_style item_style;
                item_style.align = ui::align_horizontal_left;
                item_style.font = ctx.fonts[0];
                item_style.text_color = style.text_color;
                item_style.border_width = style.border_width;
                item_style.background_color = style.active_color;
                item_style.border_color = style.border_color;

                if (ctx.active_item == id)
                    pos = item;

                if (pos == item)
                    draw::text_box(ctx, {bx, by}, {bw, bh}, item_text, item_style);
                else
                    draw::text_only(ctx, {bx, by}, {bw, bh}, item_text, item_style);
            }

            return res;
        }

    } // namespace widget

} // namespace imgui

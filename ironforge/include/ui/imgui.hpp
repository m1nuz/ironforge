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
#include <video/glyphs.hpp>

namespace ui {
    enum layout_type {
        layout_none,
        layout_horizontal,
        layout_vertical
    };
} // namespace ui

namespace imgui {

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
        frame_drag_flag      = 1 << 0,
        frame_dragging_flag  = 1 << 1,
        frame_header_flag    = 1 << 2,
    };

    struct frame_state {
        frame_state() = default;

        float x = 0.f;
        float y = 0.f;
        float width = 0.f;
        float height = 0.f;
        float widget_x = 0.f;
        float widget_y = 0.f;
        ui::layout_type layout = ui::layout_vertical;
        uint32_t    flags = 0;
    };

    struct box_style {
        box_style() = default;

        float width = 0.f;
        float height = 0.f;
        float border_width = 0.001;

        std::array<ui::color_t, 4> border_color = {0, 0, 0, 0};
        ui::color_t background_color = 0;
        ui::color_t text_color = 0;
        uint32_t align = 0;
        uint32_t font = 0;
    };

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

        std::stack<frame_state*> frames;
        ui::layout_type frame_layout = ui::layout_none;

        std::vector<uint32_t> fonts;
        std::unordered_map<std::string, box_style> styles;

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
            //game::journal::info(game::journal::_UI, "INPUT %", std::string{event.text.text});
            //ctx.character = event.text.text[3] << 24 | event.text.text[2] << 16 | event.text.text[1] << 8 | event.text.text[0];
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

    inline auto create(const video::instance_t &vi) -> std::optional<context_t> {
        std::vector<uint32_t> fonts;
        for (uint32_t i = 0; i < vi.fonts.size(); i++)
            fonts.push_back(i);

        context_t ctx;
        ctx.fonts = fonts;

        box_style bs;
        bs.height = 0.015f;

        ctx.styles.emplace("#button", bs);

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
    }

    namespace draw {

        inline auto box(context_t &ctx, const vec2 &pos, const vec2 &size, const box_style &boxstyle) -> void {
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

            box(ctx, pos, size, boxstyle);
        }
    } // namespace draw

    namespace widget {
        inline auto get_id(context_t &ctx) -> uint32_t {
            return ++ctx.widget_id;
        }

        /*inline auto start_widget() -> bool {
            return true;
        }*/

        inline auto frame_begin(context_t &ctx, frame_state *fs) -> void {
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

            if (inside_header)
                fs->flags |= frame_dragging_flag;

            if (!is_button_down(ctx, key_mouse_left))
                fs->flags &= ~frame_dragging_flag;

            if ((fs->flags & frame_dragging_flag) && is_button_down(ctx, key_mouse_left))
            {
                const auto mx = - 0.5f + static_cast<float>(ctx.mouse_x) / ctx.screen_width;
                const auto my =   0.5f - static_cast<float>(ctx.mouse_y) / ctx.screen_height;
                fs->x = mx - fs->width * 0.5f;
                fs->y = my - (fs->height - header_size * 0.5f) * ctx.aspect_ratio;
                fs->widget_x = fs->x;
                fs->widget_y = fs->y;
            }

            if (fs->flags & frame_header_flag)
            {
                //const auto _hw = fs->width * (1.f - padding);
                //const auto _hh = header_size;
                const auto _hx = fs->widget_x /*+ fs->width * padding * 0.5f*/;
                const auto _hy = fs->widget_y - (hh /*+ fs->height * padding * 0.5f*/ - fs->height) * ctx.aspect_ratio;

                box_style header_style;
                header_style.align = ui::align_center;
                header_style.font = ctx.fonts[0];
                header_style.text_color = 0x1a1a1aff;
                header_style.border_width = 0.0;
                header_style.background_color = 0xeecceeff;
                header_style.border_color = {0, 0, 0, 0};

                draw::text_box(ctx, {_hx, _hy}, {hw, hh}, "TITLE", header_style);

                fs->widget_y += - (hh + fs->height * padding * 0.5f) * ctx.aspect_ratio;
            }

            ctx.frames.push(fs);
        }

        inline auto frame_end(context_t &ctx) -> void {
            if (ctx.frames.empty())
                return;

            const auto marker_size = 0.01f;

            auto* fs = ctx.frames.top();

            const auto border_color = 0xffffffff;
            const auto background_color = 0x6f6fafff;

            ui::draw_commands::draw_rect rect;
            rect.color = border_color;
            rect.x = fs->x + fs->width - marker_size;
            rect.y = fs->y;
            rect.w = marker_size;
            rect.h = marker_size * ctx.aspect_ratio;

            ctx.commands.push_back(rect);

            box_style boxstyle;
            boxstyle.border_width = 0.003;
            boxstyle.background_color = background_color;
            boxstyle.border_color = {border_color, border_color, border_color, border_color};

            draw::box(ctx, {fs->x, fs->y}, {fs->width, fs->height}, boxstyle);

            ctx.frames.pop();
        }

        [[nodiscard]] inline auto button(context_t &ctx, const std::string &text, const std::string &style_name) -> bool {
            auto it = ctx.styles.find(style_name);
            if (it == ctx.styles.end())
                return false;

            const auto& style = it->second;

            const auto padding = 0.05f;
            const auto bw = ctx.frames.top()->width * (1.f - padding);
            const auto bh = style.height;
            const auto bx = ctx.frames.top()->widget_x + ctx.frames.top()->width * padding * 0.5f;
            const auto by = ctx.frames.top()->widget_y - (bh + ctx.frames.top()->height * padding * 0.5f - ctx.frames.top()->height) * ctx.aspect_ratio;

            ctx.frames.top()->widget_y += - (bh + ctx.frames.top()->height * padding * 0.5f) * ctx.aspect_ratio;

            const auto id = get_id(ctx);

            const auto inside = mouse_in_rect(ctx, bx, by, bw, bh);

            bool result = false;

            if (inside) {
                ctx.hot_item = id;
            }

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

            const auto active_color = 0xffff00ff;
            const auto hot_color = 0xafafafff;
            const auto border_color = 0xffffffff;
            const auto background_color = ctx.active_item == id ? active_color : (ctx.hot_item == id ? hot_color : 0x4f4f4fff);

            box_style boxstyle;
            boxstyle.align = ui::align_center;
            boxstyle.font = ctx.fonts[0];
            boxstyle.text_color = 0x1a1a1aff;
            boxstyle.border_width = 0.003;
            boxstyle.background_color = background_color;
            boxstyle.border_color = {border_color, border_color, border_color, border_color};

            draw::text_box(ctx, {bx, by}, {bw, bh}, text, boxstyle);

            return result;
        }

        inline auto edit_box(context_t &ctx, std::string &text, const std::string &style_name) -> bool {
            auto it = ctx.styles.find(style_name);
            if (it == ctx.styles.end())
                return false;

            const auto& style = it->second;

            const auto padding = 0.05f;
            const auto bw = ctx.frames.top()->width * (1.f - padding);
            const auto bh = style.height;
            const auto bx = ctx.frames.top()->widget_x + ctx.frames.top()->width * padding * 0.5f;
            const auto by = ctx.frames.top()->widget_y - (bh + ctx.frames.top()->height * padding * 0.5f - ctx.frames.top()->height) * ctx.aspect_ratio;

            ctx.frames.top()->widget_y += - (bh + ctx.frames.top()->height * padding * 0.5f) * ctx.aspect_ratio;

            const auto id = get_id(ctx);

            const auto inside = mouse_in_rect(ctx, bx, by, bw, bh);

            bool result = false;

            if (inside) {
                ctx.hot_item = id;
            }

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

            const auto active_color = 0xffff00ff;
            const auto hot_color = 0xafafafff;
            const auto border_color = 0xffffffff;
            const auto background_color = ctx.active_item == id ? active_color : (ctx.hot_item == id ? hot_color : 0x4f4f4fff);

            box_style boxstyle;
            boxstyle.align = ui::align_center;
            boxstyle.font = ctx.fonts[0];
            boxstyle.text_color = 0x1a1a1aff;
            boxstyle.border_width = 0.003;
            boxstyle.background_color = background_color;
            boxstyle.border_color = {border_color, border_color, border_color, border_color};

            draw::text_box(ctx, {bx, by}, {bw, bh}, text, boxstyle);

            return result;
        }

    } // namespace widget

    /*namespace chaning {
        struct frame;

        struct button {
            button() = delete;
            button(context_t &ctx, frame &frm) : _ctx(ctx), _frame{frm} {
            }

            inline auto background(const ui::color_t color) -> button& {
                _background_color = color;

                return *this;
            }

            inline auto text(const std::string &s) -> button& {
                _text = s;

                return *this;
            }

            inline auto on_click(std::function<void ()> cb) -> button& {
                _on_click = cb;

                return *this;
            }

            inline auto size(const float w, const float h) -> button& {
                _w = w;
                _h = h;

                return *this;
            }

            inline auto width(const float w) -> button& {
                _w = w;

                return *this;
            }

            inline auto height(const float h) -> button& {
                _h = h;

                return *this;
            }

            auto complite() -> void {
                const auto padding = 0.05f;
                const auto bw = _ctx.frames.top().size.x * (1.f - padding);
                const auto bh = _h;//0.05f;
                const auto bx = _ctx.frames.top().widget_pos.x + _ctx.frames.top().size.x * padding * 0.5f;
                const auto by = _ctx.frames.top().widget_pos.y - (bh + _ctx.frames.top().size.y * padding * 0.5f - _ctx.frames.top().size.y) * _ctx.aspect_ratio;

                _ctx.frames.top().widget_pos.y += - (bh + _ctx.frames.top().size.y * padding * 0.5f) * _ctx.aspect_ratio;

                const auto id = widget::get_id(_ctx);

                const auto inside = mouse_in_rect(_ctx, bx, by, bw, bh);

                bool result = false;

                if (inside) {
                    _ctx.hot_item = id;
                }

                if (_ctx.active_item == id && !is_button_down(_ctx, key_mouse_left)) {
                    if (inside) {
                        result = true;
                        _ctx.hot_item = id;
                    } else {
                        _ctx.hot_item = 0;
                    }

                    _ctx.active_item = 0;
                } else if (_ctx.hot_item == id) {
                    if (is_button_down(_ctx, key_mouse_left)) {
                        _ctx.focused_item = id;
                        _ctx.active_item = id;
                    }
                }

                const auto active_color = 0xffff00ff;
                const auto hot_color = 0xafafafff;
                const auto border_color = 0xffffffff;
                const auto background_color = _ctx.active_item == id ? active_color : (_ctx.hot_item == id ? hot_color : _background_color);

                text_style textstyle;
                textstyle.align = ui::align_center;
                textstyle.font = _ctx.fonts[0];
                textstyle.text_color = 0x1a1a1aff;

                box_style boxstyle;
                boxstyle.border_width = 0.003;
                boxstyle.background_color = background_color;
                boxstyle.border_color = {border_color, border_color, border_color, border_color};

                draw::text_box(_ctx, {bx, by}, {bw, bh}, _text, boxstyle, textstyle);

                if (result)
                    _on_click();
            }

            auto end() -> frame& {
                complite();
                return _frame;
            }

            static auto show(context_t &ctx, frame &frm) -> button {
                return button{ctx, frm};
            }

            ui::color_t _background_color = 0;
            std::string _text;
            std::function<void ()> _on_click;
            float _x = 0.f;
            float _y = 0.f;
            float _w = 0.15f;
            float _h = 0.05f;

            context_t &_ctx;
            frame &_frame;
        };

        struct frame {
            frame() = delete;
            frame(context_t &ctx) : _ctx{ctx} {
            }

            static auto show(context_t &ctx) -> frame {
                return frame{ctx};
            }

            inline auto width(const float w) -> frame& {
                _width = w;

                return *this;
            }

            inline auto height(const float h) -> frame& {
                _height = h;

                return *this;
            }

            auto layout() -> frame& {
                return *this;
            }

            auto prepare() -> frame& {
                frame_state fs;
                fs.size = {_width, (_height == 0.f ?  2.f * 0.499f / _ctx.aspect_ratio : _height)};
                //fs.position = {_x, _height == 0.f ? -0.499f : _y};
                fs.position = {_x, _y};
                fs.widget_pos = fs.position;
                fs.layout = _layout;

                const auto mkx = _x + fs.size.x - marker_size;
                const auto mky = _y;
                const auto mkw = marker_size;
                const auto mkh = marker_size * _ctx.aspect_ratio;

                game::journal::info(game::journal::_UI, "% % % %", mkx, mky, mkw, mkh);

                const auto inside_marker = mouse_in_rect(_ctx, mkx, mky, mkw, mkh);
                if (!inside_marker && is_button_down(_ctx, key_mouse_left))
                {
                    const auto mx = - 0.5f + static_cast<float>(_ctx.mouse_x) / _ctx.screen_width;
                    const auto my =   0.5f - static_cast<float>(_ctx.mouse_y) / _ctx.screen_height;
                    _x = mx - fs.size.x;
                    _y = my;

                    fs.position = {_x, _y};
                    fs.widget_pos = fs.position;
                }

                _ctx.frames.push(fs);

                return *this;
            }

            auto header(const std::string &text) -> frame& {
                const auto padding = 0.05f - 0.006f;
                const auto bw = _ctx.frames.top().size.x * (1.f - padding);
                const auto bh = 0.015f;
                const auto bx = _ctx.frames.top().widget_pos.x + _ctx.frames.top().size.x * padding * 0.5f;
                const auto by = _ctx.frames.top().widget_pos.y - (bh + _ctx.frames.top().size.y * padding * 0.5f - _ctx.frames.top().size.y) * _ctx.aspect_ratio;

                text_style textstyle;
                textstyle.align = ui::align_center;
                textstyle.font = _ctx.fonts[0];
                textstyle.text_color = 0x1a1a1aff;

                auto border_color = 0u;

                box_style boxstyle;
                boxstyle.border_width = 0.0;
                boxstyle.background_color = 0xeecceeff;
                boxstyle.border_color = {border_color, border_color, border_color, border_color};

                draw::text_box(_ctx, {bx, by}, {bw, bh}, text, boxstyle, textstyle);

                _ctx.frames.top().widget_pos.y += - (bh + _ctx.frames.top().size.y * padding * 0.5f) * _ctx.aspect_ratio;

                return *this;
            }

            auto btn() -> button {
                return button::show(_ctx, *this);
            }

            auto complite() -> void {
                if (_ctx.frames.empty())
                    return;

                const auto border_color = 0xffffffff;
                const auto background_color = 0x6f6fafff;

                ui::draw_commands::draw_rect rect;
                rect.color = border_color;
                rect.x = _x + _width - marker_size;
                rect.y = _y;
                rect.w = marker_size;
                rect.h = marker_size * _ctx.aspect_ratio;

                _ctx.commands.push_back(rect);

                box_style boxstyle;
                boxstyle.border_width = 0.003;
                boxstyle.background_color = background_color;
                boxstyle.border_color = {border_color, border_color, border_color, border_color};

                draw::box(_ctx, _ctx.frames.top().position, _ctx.frames.top().size, boxstyle);

                _ctx.frames.pop();
            }

            float _x = 0.f;
            float _y = 0.f;
            float _width = 0.2f;
            float _height = 0.2f;
            float marker_size = 0.05;

            ui::layout_type _layout = ui::layout_vertical;
            frame_resizing_type resizing_type = resizing_free;

            context_t &_ctx;
        };
    } // namespace chaning*/

} // namespace imgui

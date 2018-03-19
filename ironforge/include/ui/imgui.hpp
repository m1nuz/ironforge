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

    struct frame_state {
        vec2        frame_pos = {0, 0};
        vec2        frame_size = {0, 0};
        vec2        widget_pos = {0, 0};
        ui::layout_type frame_layout = ui::layout_none;
    };

    typedef struct context_type {
        context_type() = default;

        uint32_t    active_item = 0;
        uint32_t    hot_item = 0;
        uint32_t    focused_item = 0;

        int32_t     mouse_x = 0;
        int32_t     mouse_y = 0;
        uint32_t    all_keys = 0;

        uint32_t    widget_id = 0;

        uint32_t    screen_width;
        uint32_t    screen_height;

        std::stack<frame_state> frames;
        ui::layout_type frame_layout = ui::layout_none;

        std::vector<uint32_t> fonts;

        std::function<void (const ui::draw_command_t &c)> dispather;
        std::vector<ui::draw_command_t> commands;
    } context_t;

    inline auto handle_input(context_t &context, const SDL_Event &event) -> void {
        switch (event.type) {
        case SDL_WINDOWEVENT:
            switch (event.window.event) {
            case SDL_WINDOWEVENT_SHOWN:
            {
                auto window = SDL_GetWindowFromID(event.window.windowID);
                auto drawable_w = 0;
                auto drawable_h = 0;
                SDL_GL_GetDrawableSize(window, &drawable_w, &drawable_h);

                context.screen_width = drawable_w;
                context.screen_height = drawable_h;
            }
                break;

            case SDL_WINDOWEVENT_RESIZED:
                context.screen_width = event.window.data1;
                context.screen_height = event.window.data2;
                break;
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT)
                context.all_keys |= key_mouse_left;

            if (event.button.button == SDL_BUTTON_MIDDLE)
                context.all_keys |= key_mouse_middle;

            if (event.button.button == SDL_BUTTON_RIGHT)
                context.all_keys |= key_mouse_right;

            //context.mouse_x = event.button.x;
            //context.mouse_y = event.button.y;
            break;

        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT)
                context.all_keys &= ~key_mouse_left;

            if (event.button.button == SDL_BUTTON_MIDDLE)
                context.all_keys &= ~key_mouse_middle;

            if (event.button.button == SDL_BUTTON_RIGHT)
                context.all_keys &= ~key_mouse_right;

            //context.mouse_x = event.button.x;
            //context.mouse_y = event.button.y;
            break;

        case SDL_MOUSEMOTION:
            context.mouse_x = event.motion.x;
            context.mouse_y = event.motion.y;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_RETURN:
                context.all_keys |= key_enter;
                break;
            case SDLK_BACKSPACE:
                context.all_keys |= key_backspace;
                break;
            }
            break;

        case SDL_KEYUP:
            switch (event.key.keysym.sym) {
            case SDLK_RETURN:
                context.all_keys &= ~key_enter;
                break;
            case SDLK_BACKSPACE:
                context.all_keys &= ~key_backspace;
                break;
            }
            break;
        }
    }

    inline auto in_rect(const context_t &ctx, const float x, const float y, const float w, const float h) -> bool {
        const auto mx = - 0.5f + static_cast<float>(ctx.mouse_x) / ctx.screen_width;
        const auto my =   0.5f - static_cast<float>(ctx.mouse_y) / ctx.screen_height;
        const auto aspect_ration = static_cast<float>(ctx.screen_width) / static_cast<float>(ctx.screen_height);

        return (mx >= x) && (mx <= w + x) && (my >= y) && (my <= aspect_ration*h + y);
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

    struct box_style {
        std::array<ui::color_t, 4> border_color;
        ui::color_t background_color;
        float border_width = 0.001;
    };

    struct text_style {
        ui::color_t text_color;
        uint32_t align = 0;
        uint32_t font = 0;
    };

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

        inline auto text_box(context_t &ctx, const vec2 &pos, const vec2 &size, const std::string &text, const box_style &boxstyle, const text_style &textstyle) -> void {
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
            textbox.font = textstyle.font;
            textbox.text = text;
            textbox.color = textstyle.text_color;
            textbox.align = textstyle.align;

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

        inline auto frame_begin(context_t &ctx, const float width, const float height, ui::layout_type layout = ui::layout_none) -> void {
            (void)ctx, (void)width, (void)height, (void)layout;

            const auto aspect_ratio = static_cast<float>(ctx.screen_width) / static_cast<float>(ctx.screen_height);
            //const auto spt_x = 1.f / ctx.screen_width;
            //const auto spt_y = 1.f / ctx.screen_height;

            frame_state fs;
            fs.frame_size = {width, (height == 0.f ?  2.f * 0.499f / aspect_ratio : height)};
            fs.frame_pos = {0.f, height == 0.f ? -0.499f : 0.f};
            fs.widget_pos = fs.frame_pos;

            ctx.frames.push(fs);
        }

        inline auto frame_end(context_t &ctx) -> void {
            (void)ctx;

            if (ctx.frames.empty())
                return;

            const auto border_color = 0xffffffff;
            const auto background_color = 0x6f6fafff;

            box_style boxstyle;
            boxstyle.border_width = 0.003;
            boxstyle.background_color = background_color;
            boxstyle.border_color = {border_color, border_color, border_color, border_color};

            draw::box(ctx, ctx.frames.top().frame_pos, ctx.frames.top().frame_size, boxstyle);

            ctx.frames.pop();
        }

        inline auto button(context_t &ctx, const std::string &text) -> bool {
            const auto aspect_ratio = static_cast<float>(ctx.screen_width) / static_cast<float>(ctx.screen_height);
            const auto padding = 0.05f;
            const auto bw = ctx.frames.top().frame_size.x * (1.f - padding);
            const auto bh = 0.05f;
            const auto bx = ctx.frames.top().widget_pos.x + ctx.frames.top().frame_size.x * padding * 0.5f;
            const auto by = ctx.frames.top().widget_pos.y - (bh + ctx.frames.top().frame_size.y * padding * 0.5f - ctx.frames.top().frame_size.y) * aspect_ratio;

            ctx.frames.top().widget_pos.y += - (bh + ctx.frames.top().frame_size.y * padding * 0.5f) * aspect_ratio;

            const auto id = get_id(ctx);

            const auto inside = in_rect(ctx, bx, by, bw, bh);

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

            text_style textstyle;
            textstyle.align = ui::align_center;
            textstyle.font = ctx.fonts[0];
            textstyle.text_color = 0x1a1a1aff;

            box_style boxstyle;
            boxstyle.border_width = 0.003;
            boxstyle.background_color = background_color;
            boxstyle.border_color = {border_color, border_color, border_color, border_color};

            draw::text_box(ctx, {bx, by}, {bw, bh}, text, boxstyle, textstyle);

            return result;
        }
    } // namespace widget

} // namespace imgui

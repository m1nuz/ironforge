#include <ironforge_common.hpp>
#include <ui/ui.hpp>
#include <ui/context.hpp>
#include <ui/widgets.hpp>
#include <video/screen.hpp>
#include <core/application.hpp>

#include "button.hpp"

#define set_normal_state(s) ((s) |= ui::ws_normal), clr_hovered_state(s), clr_pressed_state(s)
#define set_hovered_state(s) ((s) |= ui::ws_hovered), clr_normal_state(s), clr_pressed_state(s)
#define set_pressed_state(s) ((s) |= ui::ws_pressed), clr_normal_state(s), clr_hovered_state(s)
#define clr_normal_state(s) ((s) &= ~ui::ws_normal)
#define clr_hovered_state(s) ((s) &= ~ui::ws_hovered)
#define clr_pressed_state(s) ((s) &= ~ui::ws_pressed)

namespace ui {
    template <typename T>
    inline auto in_rect(T x, T y, T w, T h, T px, T py) -> bool {
        return (px) >= (x) && (px) <= (w) + (x) && (py) >= (y) && (py) <= (h) + (y);
    }

    auto push(std::unique_ptr<context> &ctx, const draw_rect_command &dr, int32_t level) -> void {
        command c;
        c.type = command_type::rect;
        c.rect = dr;
        c.level = level;

        ctx->commands.push(c);
    }

    auto push(std::unique_ptr<context> &ctx, const draw_line_command &dl, int32_t level) -> void {
        command c;
        c.type = command_type::line;
        c.line = dl;
        c.level = level;

        ctx->commands.push(c);
    }

    auto push(std::unique_ptr<context> &ctx, const draw_text_command &dt, int32_t level) -> void {
        command c;
        c.type = command_type::text;
        c.text = dt;
        c.level = level;

        ctx->commands.push(c);
    }

    auto append(std::unique_ptr<context> &ctx, const command &com) -> void {
        ctx->commands.push(com);
    }

    std::vector<button> buttons;

    static int32_t button_id = 1;

    auto create_button(button_info &info) -> int32_t {
        button b;
        b.id = button_id++;
        b.x = info.x;
        b.y = info.y;
        b.w = info.w;
        b.h = info.h;
        b.border_width = info.border_width;
        b.background_color = info.background_color;
        b.border_color = info.border_color;
        b.text = info.text;
        b.text_size = info.text_size;
        b.text_color = info.text_color;
        b.font = info.font;
        b.align = info.align;
        b.flags = info.flags;
        b.on_click = info.on_click;

        buttons.push_back(b);

        return b.id;
    }

    static auto push_widget(std::unique_ptr<context> &ctx, const button &b) -> void {
        if (!(b.flags & wf_visible))
            return;

        draw_rect_command dr;
        dr.color = b.background_color;

        uint32_t text_color = 0;
        if (b.state & ws_pressed)
        {
            dr.color = 0xffff00ff;
            text_color = 0x808080ff;
        }
        else if (b.state & ws_hovered)
        {
            dr.color = 0xff0000ff;
            text_color = 0x2222222ff;
        }
        else
        {
            dr.color = 0x111111ff;
            text_color = 0xff00ffff;
        }

        dr.x = b.x;
        dr.y = b.y;
        dr.w = b.w;
        dr.h = b.h;

        push(ctx, dr, b.level);

        draw_line_command dl;
        dl.color = b.border_color;
        dl.w = b.border_width;

        dl.x0 = b.x;
        dl.x1 = b.x + b.w;
        dl.y0 = b.y;
        dl.y1 = b.y;

        push(ctx, dl, b.level);

        dl.x0 = b.x;
        dl.x1 = b.x;
        dl.y0 = b.y + b.h;
        dl.y1 = b.y;

        push(ctx, dl, b.level);

        dl.x0 = b.x;
        dl.x1 = b.x + b.w;
        dl.y0 = b.y + b.h;
        dl.y1 = b.y + b.h;

        push(ctx, dl, b.level);

        dl.x0 = b.x + b.w;
        dl.x1 = b.x + b.w;
        dl.y0 = b.y + b.h;
        dl.y1 = b.y;

        push(ctx, dl, b.level);

        if (b.text_size > 0) {
            draw_text_command dt;
            dt.color = text_color; // b.text_color;
            dt.align = b.align;
            dt.w = 0;
            dt.h = 0;
            dt.x = b.x;
            dt.y = b.y;
            dt.text = b.text;
            dt.size = b.text_size;
            dt.font = b.font;

            push(ctx, dt, b.level);
        }
    }

    auto create_label(label_info &info) -> int32_t {
        return 0;
    }

    auto create_window(window_info &info) -> int32_t {
        return 0;
    }

    auto create_context() -> std::unique_ptr<context> {
        return make_unique<context>();
    }

    auto process_event(std::unique_ptr<context> &ctx, const SDL_Event &event) -> void {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            float px = - 0.5f + (float)event.button.x / video::screen.width;
            float py =   0.5f - (float)event.button.y / video::screen.height;

            //application::info(application::log_category::ui, "% %\n", px, py);

            for (auto &b : buttons)
                if (in_rect(b.x, b.y, b.w, b.h, px, py)) {
                    if (b.on_click)
                        b.on_click(b.id);

                    set_pressed_state(b.state);
                }
        }

        if (event.type == SDL_MOUSEBUTTONUP) {
            float px = - 0.5f + (float)event.button.x / video::screen.width;
            float py =   0.5f - (float)event.button.y / video::screen.height;

            for (auto &b : buttons)
                if (b.state & ws_pressed) {
                    if (in_rect(b.x, b.y, b.w, b.h, px, py))
                        set_hovered_state(b.state);
                    else
                        set_normal_state(b.state);
                }
        }

        if (event.type == SDL_MOUSEMOTION) {
            float px = -0.5f + (float)event.motion.x / video::screen.width;
            float py =  0.5f - (float)event.motion.y / video::screen.height;

            for (auto &b : buttons)
                if (in_rect(b.x, b.y, b.w, b.h, px, py))
                {
                    if (!(b.state & ws_pressed))
                        set_hovered_state(b.state);
                } else {
                    set_normal_state(b.state);
                }
        }
    }

    auto present(std::unique_ptr<context> &ctx, const std::function<void (const command &)> &dispath) -> void {
        for (const auto &b : buttons)
            push_widget(ctx, b);

        // TODO: sort commands by level
        while (!ctx->commands.empty()) {
            auto c = ctx->commands.top();

            dispath(c);

            ctx->commands.pop();
        }
    }
} // namespace ui

#include <algorithm>

#include <core/common.hpp>
#include <ui/ui.hpp>
#include <ui/context.hpp>
#include <ui/widgets.hpp>
#include <video/screen.hpp>
#include <video/glyphs.hpp>

#include "button.hpp"
#include "label.hpp"
#include "window.hpp"

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

        append(ctx, c);
    }

    auto push(std::unique_ptr<context> &ctx, const draw_line_command &dl, int32_t level) -> void {
        command c;
        c.type = command_type::line;
        c.line = dl;
        c.level = level;

        append(ctx, c);
    }

    auto push(std::unique_ptr<context> &ctx, const draw_text_command &dt, int32_t level) -> void {
        command c;
        c.type = command_type::text;
        c.text = dt;
        c.level = level;

        append(ctx, c);
    }

    auto append(std::unique_ptr<context> &ctx, const command &com) -> void {
        ctx->commands.push_back(com);
    }

    std::vector<button> buttons;
    std::vector<window> windows;

    static int32_t button_id = 1;
    static int32_t window_id = 1;

    static auto find_button(const int32_t id) -> button* {
        auto w = std::find_if(buttons.begin(), buttons.end(), [id](const button &b) {
            return b.id == id;
        });

        if (w != buttons.end())
            return &(*w);

        return nullptr;
    }

    static auto find_window(const int32_t id) -> window* {
        auto ws = std::find_if(windows.begin(), windows.end(), [id](const window &w) {
            return w.id == id;
        });

        if (ws != windows.end())
            return &(*ws);

        return nullptr;
    }

    static auto find_widget(const int32_t id) -> widget* {
        widget *w = find_button(id);

        if (w)
            return w;

        w = find_window(id);

        if (w)
            return w;

        return nullptr;
    }

    auto create_button(const button_info &info) -> int32_t {
        button b;
        b.id = button_id++;
        b.x = info.x;
        b.y = info.y;
        b.w = info.w;
        b.h = info.h;
        b.initial_x = info.x;
        b.initial_y = info.y;
        b.initial_w = info.w;
        b.initial_h = info.h;
        b.translate_x = info.translate_x;
        b.translate_y = info.translate_y;
        b.border_width = info.border_width;
        b.background_color = info.background_color;
        b.border_color = info.border_color;
        b.text = info.text;
        b.text_size = info.text_size;
        b.text_color = info.text_color;
        b.font = info.font;
        b.align = info.align;
        b.level = info.level;
        b.flags = info.flags;
        b.margins = info.margins;
        b.on_click = info.on_click;

        if (buttons.capacity() == 0)
            buttons.reserve(100);

        buttons.push_back(b);

        return b.id;
    }

    static auto push_widget(std::unique_ptr<context> &ctx, const video::instance_t &vi, const button &b) -> void {
        (void)vi;

        if (!(b.flags & wf_visible))
            return;        

        uint32_t text_color = 0;
        if (!(b.flags & wf_nobackground)) {
            draw_rect_command dr;
            dr.color = b.background_color;

            if (b.state & ws_pressed)
            {
                dr.color = 0xffff00ff;
                text_color = 0x808080ff;
            }
            else if (b.state & ws_hovered)
            {
                dr.color = 0xff0000ff;
                text_color = 0x222222ff;
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
        }

        if (!(b.flags & wf_noborder)) {
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
        }

        if (b.text_size > 0) {
            draw_text_command dt;
            dt.color = text_color; // b.text_color;
            dt.align = b.align;
            dt.w = b.w;
            dt.h = b.h;
            dt.x = b.x * 2;
            dt.y = b.y * 2;
            dt.text = b.text;
            dt.size = b.text_size;
            dt.font = b.font;

            push(ctx, dt, b.level - 1);
        }
    }

    static auto push_widget(std::unique_ptr<context> &ctx, const window &w) -> void {
        if (!(w.flags & wf_visible))
            return;

        if (!(w.flags & wf_nobackground)) {
            draw_rect_command dr;
            dr.color = w.background_color;
            dr.x = w.x;
            dr.y = w.y;
            dr.w = w.w;
            dr.h = w.h;

            push(ctx, dr, w.level);
        }

        if (!(w.flags & wf_noborder)) {
            draw_line_command dl;
            dl.color = w.border_color;
            dl.w = w.border_width;

            dl.x0 = w.x;
            dl.x1 = w.x + w.w;
            dl.y0 = w.y;
            dl.y1 = w.y;

            push(ctx, dl, w.level);

            dl.x0 = w.x;
            dl.x1 = w.x;
            dl.y0 = w.y + w.h;
            dl.y1 = w.y;

            push(ctx, dl, w.level);

            dl.x0 = w.x;
            dl.x1 = w.x + w.w;
            dl.y0 = w.y + w.h;
            dl.y1 = w.y + w.h;

            push(ctx, dl, w.level);

            dl.x0 = w.x + w.w;
            dl.x1 = w.x + w.w;
            dl.y0 = w.y + w.h;
            dl.y1 = w.y;

            push(ctx, dl, w.level);
        }
    }

    auto create_label(const label_info &info) -> int32_t {
        (void)info;
        return 0;
    }

    auto create_window(const window_info &info) -> int32_t {
        window w;
        w.id = window_id++;
        w.x = info.x;
        w.y = info.y;
        w.w = info.w;
        w.h = info.h;
        w.background_color = info.background_color;
        w.border_color = info.border_color;
        w.border_width = info.border_width;
        w.flags = info.flags;
        w.level = info.level;
        w.padding = info.padding;

        if (windows.capacity() == 0)
            windows.reserve(100);

        windows.push_back(w);

        return w.id;
    }

    auto window_append(const int32_t win_id, const int32_t id) -> bool {
        auto w = find_widget(id);
        if (!w)
            return false;

        auto win = find_window(win_id);
        if (!win)
            return false;

        //w->level = win->level - 1;
        w->parent = win_id;
        win->grows.push_back(1);
        win->wflags.push_back(0);
        win->widgets.push_back(w);

        return false;
    }

    auto create_context() -> std::unique_ptr<context> {
        return std::make_unique<context>();
    }

    window *current_window = nullptr;

    auto process_event(context &ctx, const SDL_Event &event) -> void {
        UNUSED(ctx);

        if (event.type == SDL_MOUSEBUTTONDOWN) {
            const float px = - 0.5f + (float)event.button.x / video::screen.width;
            const float py =   0.5f - (float)event.button.y / video::screen.height;

            //game::journal::info(application::log_category::ui, "% %", px, py);

            for (auto &b : buttons)
                if (in_rect(b.x, b.y, b.w, b.h, px, py)) {
                    if (b.on_click)
                        b.on_click(b.id);

                    set_pressed_state(b.state);
                }

            current_window = nullptr;
            for (auto &w : windows)
                if ((w.flags & wf_movable) && in_rect(w.x, w.y, w.w, w.h, px, py))
                    current_window = &w;
        }

        if (event.type == SDL_MOUSEBUTTONUP) {
            const float px = - 0.5f + (float)event.button.x / video::screen.width;
            const float py =   0.5f - (float)event.button.y / video::screen.height;

            for (auto &b : buttons)
                if (b.state & ws_pressed) {
                    if (in_rect(b.x, b.y, b.w, b.h, px, py))
                        set_hovered_state(b.state);
                    else
                        set_normal_state(b.state);
                }

            current_window = nullptr;
        }

        if (event.type == SDL_MOUSEMOTION) {
            const float px = -0.5f + (float)event.motion.x / video::screen.width;
            const float py =  0.5f - (float)event.motion.y / video::screen.height;

            for (auto &b : buttons)
                if (in_rect(b.x, b.y, b.w, b.h, px, py))
                {
                    if (!(b.state & ws_pressed))
                        set_hovered_state(b.state);
                } else {
                    set_normal_state(b.state);
                }

            if (current_window) {
                const float w = current_window->w;
                const float h = current_window->h;
                const float dx = -w * 0.5f;
                const float dy = -h * 0.5f;
                const float lx = px + dx;
                const float ly = py + dy;

                current_window->x = lx;
                current_window->y = ly;
            }
        }
    }

    auto present(std::unique_ptr<context> &ctx, const video::instance_t &vi, const std::function<void (const command &)> &dispath) -> void {
        for (const auto &wnd : windows) {
            for (size_t i = 0; i < wnd.widgets.size(); i++) {
                //auto pv = i == 0 ? nullptr : wnd.widgets[i - 1];
                auto w = wnd.widgets[i];
                const auto coeff = wnd.grows[i] / wnd.widgets.size();
                const auto coeff2 = (i == 0 ? 0 : wnd.grows[i - 1] / wnd.widgets.size());

                const auto padding_left = wnd.padding[3] * wnd.w;
                const auto padding_bottom = wnd.padding[2] * wnd.h * video::screen.aspect;
                const auto padding_right = wnd.padding[1] * wnd.w;
                const auto padding_top = wnd.padding[0] * wnd.h * video::screen.aspect;
                const auto margin_left = wnd.w * w->margins[3];
                const auto margin_bottom = wnd.h * w->margins[2] * video::screen.aspect;
                const auto margin_right = wnd.w * w->margins[1];
                const auto margin_top = wnd.h * w->margins[0] * video::screen.aspect;
                const auto base_x = wnd.x + padding_left;
                const auto base_y = wnd.y + padding_bottom;
                const auto base_w = wnd.w - padding_left - padding_right;
                const auto base_h = wnd.h - padding_top - padding_bottom;

                if (wnd.flags & wf_row) {
                    w->w = base_w * coeff * w->initial_w - margin_left - margin_right;
                    w->h = base_h * w->initial_h - margin_bottom - margin_top;
                    w->x = base_x + coeff2 * base_w * i + w->translate_x * w->initial_w + margin_left;
                    w->y = base_y + margin_bottom;
                } else if (wnd.flags & wf_column) {
                    w->w = base_w * w->initial_w - margin_left - margin_right;
                    w->h = base_h * coeff * w->initial_h - margin_bottom - margin_top;
                    w->x = base_x + margin_left;
                    w->y = base_y + coeff2 * base_h * i + w->translate_y * w->initial_h + margin_bottom;
                }
            }
        }

        for (auto &b : buttons) {
            auto w = find_window(b.parent);

            if (w) {
                if (!(w->flags & wf_flexible)) {
                    const auto padding_left = w->padding[3] * w->w;
                    const auto padding_bottom = w->padding[2] * w->w * video::screen.aspect;
                    const auto padding_right = w->padding[1] * w->w;
                    const auto padding_top = w->padding[0] * w->h * video::screen.aspect;
                    const auto base_x = w->x + padding_left;
                    const auto base_y = w->y + padding_bottom;
                    const auto base_w = w->w - padding_left - padding_right;
                    const auto base_h = w->h - padding_top - padding_bottom;
                    b.x = base_x + b.initial_x * base_w + b.translate_x * b.w;
                    b.y = base_y + b.initial_y * base_h + b.translate_y * b.h;
                    b.w = base_w * b.initial_w;
                    b.h = base_h * b.initial_h;
                }
            }
        }

        for (const auto &b : buttons)
            push_widget(ctx, vi, b);

        for (const auto &w : windows)
            push_widget(ctx, w);

        ctx->commands.sort();

        while (!ctx->commands.empty()) {
            const auto c = ctx->commands.back();

            //game::journal::debug(application::log_category::ui, "% %", static_cast<int>(c.type), c.level);

            dispath(c);

            ctx->commands.pop_back();
        }
    }
} // namespace ui

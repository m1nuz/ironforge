#include <vector>
#include <memory>
#include <algorithm>
#include <functional>

#include <video/video.hpp>
#include <core/application.hpp>
#include <core/settings.hpp>
#include <core/game.hpp>
#include <renderer/renderer.hpp>
#include <scene/scene.hpp>
#include <ui/ui.hpp>

// FIXME: remove this
namespace scene {
    auto do_script(const std::string &name) -> bool;
} // namespace scene

namespace game {

    std::vector<std::unique_ptr<scene::instance>>   scenes;
    std::unique_ptr<renderer::instance>             render;
    std::unique_ptr<ui::context>                    uis;
    std::string some_text{"Lorem ipsum"};
    std::string some_text1{"AAA"};
    std::string some_text2{"BBB"};
    std::string some_text3{"CCC"};

    auto get_current() -> std::unique_ptr<scene::instance>& {
        // TODO: optimize this shit
        auto s = std::find_if(scenes.begin(), scenes.end(), [](std::unique_ptr<scene::instance>& sc) {
            if (sc->state & static_cast<uint32_t>(scene::state_flags::current))
                return true;
            return false;
        });

        if (s == scenes.end() && scenes.empty()) {
            scenes.push_back(scene::empty());
            return scenes.back();
        }

        /*for (size_t i = 0; i < scenes.size(); ++i)
            if (scenes[i]->state & static_cast<uint32_t>(scene::state_flags::current))
                return scenes[i];*/

        return *s;
    }

    auto load_scene(const std::string &name) -> bool {
        scenes.push_back(scene::load(name, static_cast<uint32_t>(scene::state_flags::start) | static_cast<uint32_t>(scene::state_flags::current)));
        return true;
    }

    __must_ckeck auto init(const std::string &title, const std::string &startup_script) -> result {
        int w = application::int_value("video_width", 800);
        int h = application::int_value("video_height", 600);
        bool fullscreen = application::bool_value("video_fullscreen", false);
        bool vsync = application::bool_value("video_vsync", false);

        auto video_result = video::result::failure;
        if ((video_result = video::init(title, w, h, fullscreen, vsync)) != video::result::success) {
            application::error(application::log_category::video, "%\n", video::get_string(video_result));

            return game::result::error_init_video;
        }

        application::info(application::log_category::video, "%\n", video::get_info());

        //render = renderer::create_null_renderer();
        render = renderer::create_forward_renderer();

        scene::init_all(); // TODO: get result

        // TODO: call startup script here
        scene::do_script(startup_script);

        //scenes.push_back(scene::load("scene02.scene", static_cast<uint32_t>(scene::state_flags::start) | static_cast<uint32_t>(scene::state_flags::current)));

        if (!render)
            return result::error_empty_render;

        if (scenes.empty())
            application::warning(application::log_category::game, "%\n", "No scene loaded");

        uis = ui::create_context();
        uis->commands.size();
        ui::button_info bi;
        bi.w = 1.0;
        bi.h = 1.0;
        bi.x = 0.0;
        //bi.translate_x = -0.5;
        bi.y = 0.0;
        //bi.translate_y = -0.5;
        bi.text = some_text1.c_str();
        bi.text_size = some_text1.size();
        bi.font = 0;
        bi.align = ui::align_horizontal_center | ui::align_vertical_center;
        bi.text_color = 0xffffffff;
        bi.border_width = 0.002;
        bi.background_color = 0x111111ff;
        bi.border_color = 0x00ffffff;
        bi.level = 0;
        bi.on_click = [](int32_t id) {
            application::info(application::log_category::game, "Click %\n", id);
        };
        int btn1 = ui::create_button(bi);

        bi.y = 0.1;
        bi.text = some_text2.c_str();
        bi.text_size = some_text2.size();
        bi.level = 0;
        int btn2 = ui::create_button(bi);

        bi.y = 0.0;
        bi.text = some_text3.c_str();
        bi.text_size = some_text3.size();
        bi.level = 0;
        int btn3 = ui::create_button(bi);

        ui::window_info wi;
        wi.x = 0.0;
        wi.y = 0.0;
        wi.w = 0.4;
        wi.h = 0.4;
        wi.border_width = 0.002;
        wi.background_color = 0x333333ff;
        wi.border_color = 0x00ffffff;
        wi.level = 1;
        wi.flags = ui::wf_visible | ui::wf_movable | ui::wf_flexible | ui::wf_column;
        wi.padding = {0.1, 0.1, 0.1, 0.1};
        auto wnd = ui::create_window(wi);
        ui::window_append(wnd, btn1);
        ui::window_append(wnd, btn2);
        ui::window_append(wnd, btn3);

        return result::success;
    }

    auto cleanup() -> void {
        scene::cleanup_all();

        for (auto& s : scenes)
            s.reset(nullptr);

        render.reset(nullptr);

        video::cleanup();
    }

    auto process_event(const SDL_Event &e) -> void {
        if (e.type == SDL_KEYDOWN)
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                application::quit();
            }

        ui::process_event(uis, e);
        scene::process_event(get_current(), e);
    }

    auto update(float dt) -> void {
        scene::update(get_current(), dt);
        video::process();
    }

    auto present(float interpolation) -> void {
        /*ui::command c;
        c.type = ui::command_type::line;
        c.line.color = 0xff0000ff;
        c.line.w = 0.01;
        c.line.x0 = 0;
        c.line.y0 = 0;
        c.line.x1 = 0.5;
        c.line.y1 = 0.5;

        ui::append(uis, c);*/

        using std::placeholders::_1;
        ui::present(uis, std::bind(&renderer::instance::dispath, render.get(), _1));
        scene::present(get_current(), render, interpolation);
    }

    auto get_string(result r) -> const char * {
        switch (r) {
        case result::success:
            return "Success";
            break;
        case result::failure:
            return "Failure";
            break;
        case result::error_init_video:
            return "Can't init video";
            break;
        case result::error_empty_render:
            return "Empty render";
            break;
        default:
            return "Unknown error";
            break;
        }

        return nullptr;
    }
} // namespace game

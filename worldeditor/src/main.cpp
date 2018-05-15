#include <functional>

#include <core/journal.hpp>
#include <core/game.hpp>
#include <core/assets.hpp>
#include <core/input.hpp>
#include <scene/scene.hpp>
#include <video/video.hpp>

#include <SDL2/SDL.h>

#include <ui/imui.hpp>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace editor {
    namespace journal = game::journal;

    struct pause_state {

    };

    struct play_state {

    };

    using editor_state = std::variant<play_state, pause_state>;

    imui::context_t ui_context;

    editor_state current_state = pause_state{};

    auto process_events(game::instance_t &app) -> void {
        SDL_Event ev = {};

        while (SDL_PollEvent(&ev)) {
            std::visit(overloaded {
                           [&app, &ui_context, ev](pause_state &state) {
                               if (ev.type == SDL_KEYDOWN) {
                                   if (ev.key.keysym.sym == SDLK_ESCAPE)
                                       app.running = false;

                                   if (ev.key.keysym.sym == SDLK_SPACE)
                                       current_state = play_state{};
                               }

                               imui::handle_input(ui_context, ev);

                               //journal::debug(journal::_GAME, "% % %", ui_context.mouse_x, ui_context.mouse_y, ui_context.all_keys);
                           },
                           [&app, ev](play_state &state) {
                               if (ev.type == SDL_KEYDOWN) {
                                   if (ev.key.keysym.sym == SDLK_SPACE)
                                       current_state = pause_state{};
                               }

                               game::input::process_event(app, ev);
                               scene::process_event(app.current_scene(), ev);
                           }
                       }, current_state);
        }
    }

    auto cleanup_all(game::instance_t &app) -> void {
        app.render.reset(nullptr);

        video::cleanup(app.vi);
        scene::cleanup_all(app.scenes);
        assets::cleanup(app.asset_instance);
    }

    auto update(game::instance_t &app, const float dt) -> void {
        std::visit(overloaded {
                       [&app](pause_state &state) {
                       },
                       [&app, dt](play_state &state) {
                           game::input::update(app);
                           assets::process(app.asset_instance);
                           scene::update(app.current_scene(), dt);
                           video::process(app.asset_instance, app.vi);
                       }
                   }, current_state);
    }

    auto present(game::instance_t &app, const float interpolation) -> void {
        using namespace game;

        static imui::frame_state_t fs1;
        static imui::frame_state_t fs2;
        static imui::frame_state_t fs3;

        fs1.y = -0.3;
        fs1.width = 0.2f;
        fs1.height = 0.4f;
        fs1.flags |= imui::frame_drag_flag | imui::frame_header_flag;

        //fs3.width = 0.2f;
        //fs3.height = 0.2f;
        fs3.layout = ui::layout_horizontal;

        imui::frame_begin(ui_context, &fs1, "#frame_header");

        if (imui::input_button(ui_context, "❄ Button1 ❄", "#button"))
            journal::info(journal::_GAME, "%", "Click");

        static std::string text1;

        if (imui::input_text(ui_context, text1, "#button"))
            journal::info(journal::_GAME, "%", "EDIT");

        const int max_value = 5;
        static int value1 = 0;

        std::string value_text1;
        utility::format(value_text1, "Value: %", value1);

        if (imui::input_range(ui_context, value1, max_value, value_text1, "#button"))
            journal::info(journal::_GAME, "VALUE %", value1);

        if (imui::progress_bar(ui_context, value1, max_value, "", "#progress"))
            journal::info(journal::_GAME, "PROGRESS %", value1);

        static size_t list1_pos = 0;
        static size_t value_pos = 0;
        value_pos = value1;
        static std::vector<std::string> list1{"item 1", "item 2", "item 3", "item 4", "item 5"};

        if (imui::list_box(ui_context, list1, 3, list1_pos, value_pos, "#button"))
            ;

        imui::frame_end(ui_context, "#frame");


        fs2.width = 0.4f;
        fs2.height = 0.07f;
        fs2.flags |= imui::frame_drag_flag | imui::frame_header_flag;
        fs2.layout = ui::layout_horizontal;

        imui::frame_begin(ui_context, &fs2, "#frame_header");

        if (imui::input_button(ui_context, "❄ Button1 ❄", "#button"))
            journal::info(journal::_GAME, "%", "Click");

        if (imui::input_button(ui_context, "❄ Button2 ❄", "#button"))
            journal::info(journal::_GAME, "%", "CLICK");

        imui::frame_end(ui_context, "#frame");

        imui::present(ui_context);
        scene::present(app.vi, app.current_scene(), app.render, interpolation);
    }

    auto launch(game::instance_t &app) -> int {
        app.current_time = 0ull;
        app.last_time = 0ull;
        app.timesteps = 0ull;
        app.delta_accumulator = 0.0f;

        ui_context = app.imui;

        using std::placeholders::_1;
        ui_context.dispather = std::bind(&renderer::instance::dispath, app.render.get(), app.vi, _1);
        ui_context.get_text_lenght = [&app] (const uint32_t font, const std::string &text) {
            if (app.vi.fonts.size() < font)
                return std::make_tuple<float, float>(0.f, 0.f);

            const auto &f = app.vi.fonts[font];

            return video::get_text_length(f, text);
        };

        while (app.running) {
            process_events(app);

            app.last_time = app.current_time;
            app.current_time = SDL_GetPerformanceCounter();

            const auto freq = SDL_GetPerformanceFrequency();
            const auto dt = static_cast<float>(static_cast<double>(app.current_time - app.last_time) / static_cast<double>(freq));

            app.delta_accumulator += glm::clamp(dt, 0.f, 0.2f);

            while (app.delta_accumulator >= game::timestep) {
                app.delta_accumulator -= game::timestep;

                update(app, game::timestep);

                app.timesteps++;
            }

            present(app, app.delta_accumulator / game::timestep);
        }

        cleanup_all(app);

        return EXIT_SUCCESS;
    }
}

extern int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    using namespace std;
    using namespace editor;

    // Get executable name
    const auto app_name = strrchr(argv[0], '/') ? strrchr(argv[0], '/') + 1 : nullptr;
    game::journal::setup_default(string{app_name} + ".log");

    //set_verbosity(verbosity::error);
    //set_verbosity(_SCENE, verbosity::verbose);

    // Create game application
    auto res = game::create("../assets/editor/editor.conf");
    if (!is_ok(res)) {
        game::journal::error(game::journal::_GAME, "%", what(res));
        return EXIT_FAILURE;
    }

    auto app = move(get<game::instance_t>(res));

    // Launch the game
    return editor::launch(app);
}

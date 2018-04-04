#include <functional>

#include <core/journal.hpp>
#include <core/game.hpp>
#include <core/assets.hpp>
#include <core/input.hpp>
#include <scene/scene.hpp>
#include <video/video.hpp>

#include <SDL2/SDL.h>

#include <ui/imgui.hpp>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace editor {
    namespace journal = game::journal;

    struct pause_state {

    };

    struct play_state {

    };

    using editor_state = std::variant<play_state, pause_state>;

    imgui::context_t ui_context;

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

                               imgui::handle_input(ui_context, ev);

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

        static imgui::frame_state fs1;
        fs1.width = 0.2f;
        fs1.height = 0.2f;
        fs1.flags |= imgui::frame_drag_flag | imgui::frame_header_flag;

        imgui::widget::frame_begin(ui_context, &fs1);

        if (imgui::widget::button(ui_context, "❄ Button1 ❄", "#button"))
            journal::info(journal::_GAME, "%", "Click");

        if (imgui::widget::button(ui_context, "❄ Button2 ❄", "#button"))
            journal::info(journal::_GAME, "%", "CLICK");

        static std::string text1;

        if (imgui::widget::edit_box(ui_context, text1, "#button"))
            journal::info(journal::_GAME, "%", "EDIT");

        imgui::widget::frame_end(ui_context);

        /*using namespace imgui::chaning;

        frame::show(ui_context)
                .width(0.4)
                .prepare()
                .header("Window")
                .btn()
                .text("❄ Button1 ❄")
                .background(0xff0000ff)
                .on_click([] () { journal::info(journal::_GAME, "%", "Click 1"); })
                .end()
                .btn()
                .text("❄ Button2 ❄")
                .background(0xff0000ff)
                .on_click([] () { journal::info(journal::_GAME, "%", "Click 2"); })
                .end()
                .complite();*/

        //imgui::widget::frame_end(ui_context);

        imgui::present(ui_context);
        scene::present(app.vi, app.current_scene(), app.render, interpolation);
    }

    auto launch(game::instance_t &app) -> int {
        app.current_time = 0ull;
        app.last_time = 0ull;
        app.timesteps = 0ull;
        app.delta_accumulator = 0.0f;

        auto ui_ctx = imgui::create(app.vi);
        if (!ui_ctx)
            return EXIT_FAILURE;

        ui_context = ui_ctx.value();

        using std::placeholders::_1;
        ui_context.dispather = std::bind(&renderer::instance::dispath, app.render.get(), app.vi, _1);

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

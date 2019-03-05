#include <functional>

#include <core/journal.hpp>
#include <core/game.hpp>
#include <core/assets.hpp>
#include <core/input.hpp>
#include <scene/scene.hpp>
#include <video/video.hpp>

#include <glcore_330.h>
#include <SDL2/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl_gl3.h>
#include <imgui_dock.h>
#include <nfd.h>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

namespace game {

    namespace detail {

        auto read_ui_styles( assets::instance_t& asset, json ui_json) -> std::unordered_map<std::string, imui::box_style>;

        ///
        /// \brief Returns base application path.
        /// Base path mean where executable file located.
        ///
        auto get_base_path() noexcept -> const std::string&;

        ///
        /// \brief Returns prefered application path.
        /// Prefered path mean where application could write data.0
        ///
        auto get_pref_path() noexcept -> const std::string&;

    } // namespace detail

} // namespace game

namespace editor {
    namespace journal = game::journal;

    struct pause_state {

    };

    struct play_state {

    };

    struct instace_type {
        scene::instance_t _scene;
        assets::instance_t _asset;
        video::instance_t _video;
    };

    using instace_t = instace_type;

    using editor_state = std::variant<play_state, pause_state>;

    imui::context_t ui_context;

    editor_state current_state = pause_state{};

    auto process_events(game::instance_t &app) -> void {
        SDL_Event ev = {};

        while (SDL_PollEvent(&ev)) {
            ImGui_ImplSdlGL3_ProcessEvent(&ev);

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
                           video::process_resources(app.asset_instance, app.vi);
                       }
                   }, current_state);
    }

    auto present(game::instance_t &app, const float interpolation) -> void {
        using namespace game;

        /*static imui::frame_state_t fs1;
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

        imui::present(ui_context);*/
        scene::present(app.vi, app.current_scene(), app.render, interpolation);
    }

    /*auto launch(game::instance_t &app) -> int {
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

            ImGui_ImplSdlGL3_NewFrame(app.vi.window);

            ImGui::Text("Hello, world!");

            ImGui::Render();
            ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());

            video::gl::texture imgui_tex;
            imgui_tex.id = ImGuiTexture();
            imgui_tex.target = GL_TEXTURE_2D;
            app.render->append(imgui_tex, renderer::UI_TEXTURE_BIT);

            present(app, app.delta_accumulator / game::timestep);
        }

        ImGui_ImplSdlGL3_Shutdown();
        ImGui::DestroyContext();

        cleanup_all(app);

        return EXIT_SUCCESS;
    }*/

    [[nodiscard]] auto init(std::string_view conf_path, const bool fullpath_only = false) -> std::optional<instace_t> {
        using namespace std;

        instace_t inst;

        // if releative and not fullpath_only add base_path
        const auto cpath = (conf_path.find("..") == string::npos) && !fullpath_only ? string{conf_path} : game::detail::get_base_path() + string{conf_path};
        auto contents = assets::get_config(cpath);

        if (!contents) {
            journal::error("editor", "%s", "Config not found");
            return {};
        }

        auto j = json::parse(contents.value());
        {
            auto asset_inst = assets::create_instance(assets::create_default_readers());
            if (holds_alternative<error_code>(asset_inst)) {
                journal::error("editor", "%", "Can't append readers");
                return {};
            }

            inst._asset = move(get<assets::instance_t>(asset_inst));

            if (j.find("assets") != j.end()) {
                for (auto &a : j["assets"]) {
                    const auto asset_name = a.get<string>();

                    journal::debug("editor", "%", asset_name);

                    if (!assets::open(inst._asset, game::detail::get_base_path() + a.get<string>()))
                        journal::error("editor", "%", "Can't open asset %", asset_name);
                }
            }
        }

        inst._video.texture_level = 1;

        return inst;
    }

    auto open_scene(const std::string_view scene_path) {

    }

    auto quit( ) noexcept {
        SDL_Event ev;
        ev.type = SDL_QUIT;

        SDL_PushEvent( &ev );
    }

    auto pick_file( ) {
        nfdchar_t *out_path = nullptr;
        auto result = NFD_OpenDialog( "scene", nullptr, &out_path );
        if ( result == NFD_OKAY ) {
            std::string p = out_path;
            free( out_path );
            return p;
        } else if ( result == NFD_CANCEL ) {
            // nothing
        } else {
            journal::error( "editor", "%", NFD_GetError( ) );
        }
    }

    bool show_app_property_editor = false;

    auto show_scene_properties( instace_t& app ) {
        if (!show_app_property_editor)
            return;

        auto show_scene_entities = []( scene::instance_t &sc ) {
            for ( auto &[entity_name, entity_id] : sc.names ) {
                ImGui::PushID( entity_id );
                ImGui::AlignTextToFramePadding( );

                bool node_open = ImGui::TreeNode( "Object", "%s %u", entity_name.c_str( ), entity_id );
                if ( node_open ) {
                    ImGui::NextColumn( );
                    ImGui::Text( "Properties:" );
                    ImGui::TreePop( );
                    ImGui::NextColumn( );
                }

                ImGui::PopID( );
            }
        };

        if ( !ImGui::Begin( "Scene properties", &show_app_property_editor ) ) {
            ImGui::End( );
            return;
        }

//        ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 2, 2 ) );
//        ImGui::Columns( 2 );
//        ImGui::Separator( );

//        show_scene_entities( app._scene );

        //        ImGui::PopStyleVar( );

        static int selected = 0;
        static uint32_t selected_entity = 0;
        int index = 0;
        ImGui::BeginChild( "left pane", ImVec2( 150, 0 ), true );
        for ( auto &[entity_name, entity_id] : app._scene.names ) {
            char label[128];
            sprintf( label, "%s #%d", entity_name.c_str( ), entity_id );
            if ( ImGui::Selectable( label, selected == index ) ) {
                selected_entity = entity_id;
                selected = index;
            }
            index++;
        }
        ImGui::EndChild( );
        ImGui::SameLine( );

        ImGui::BeginGroup( );
        ImGui::BeginChild( "item view", ImVec2( 0, -ImGui::GetFrameHeightWithSpacing( ) ) );

        auto entity_name = scene::get_entity_name( app._scene, selected_entity );
        if ( entity_name ) {
            ImGui::Text( " %s : %d", entity_name.value( ).data( ), selected_entity );
        }

        auto entity_material = scene::get_entity_material( app._scene, selected_entity );
        if ( entity_material ) {
            if ( ImGui::CollapsingHeader( "Material" ) ) {
            }
        }

        auto entity_model = scene::get_entity_model( app._scene, selected_entity );
        if ( entity_model ) {
            if ( ImGui::CollapsingHeader( "Model" ) ) {
            }
        }

        auto entity_camera = scene::get_entity_camera( app._scene, selected_entity );
        if ( entity_camera ) {
            if ( ImGui::CollapsingHeader( "Camera" ) ) {
            }
        }

        auto entity_script = scene::get_entity_script( app._scene, selected_entity );
        if ( entity_script ) {
            if ( ImGui::CollapsingHeader( "Script" ) ) {
            }
        }

        auto entity_body = scene::get_entity_body( app._scene, selected_entity );
        if ( entity_body ) {
            if ( ImGui::CollapsingHeader( "Body" ) ) {
            }
        }

        auto entity_light = scene::get_entity_light( app._scene, selected_entity );
        if ( entity_light ) {
            if ( ImGui::CollapsingHeader( "Light" ) ) {
            }
        }

        ImGui::Separator( );

        ImGui::EndChild( );
        ImGui::EndGroup( );

        ImGui::End( );
    }

    auto show_file_menu( instace_t &app ) {
        if ( ImGui::MenuItem( "New" ) ) {
        }

        if ( ImGui::MenuItem( "Open", "Ctrl+O" ) ) {
            auto fp = pick_file( );
            auto res = scene::load( app._asset, app._video, fp, true );
            if ( !scene::is_ok( res ) ) {
                return;
            }

            app._scene = std::move( std::get<scene::instance_t>( res ) );

            show_app_property_editor = true;
        }

        if ( ImGui::BeginMenu( "Open Recent" ) ) {
            ImGui::MenuItem( "fish_hat.c" );
            ImGui::MenuItem( "fish_hat.inl" );
            ImGui::MenuItem( "fish_hat.h" );
            if ( ImGui::BeginMenu( "More.." ) ) {
                ImGui::MenuItem( "Hello" );
                ImGui::MenuItem( "Sailor" );
                ImGui::EndMenu( );
            }
            ImGui::EndMenu( );
        }

        if ( ImGui::MenuItem( "Save", "Ctrl+S" ) ) {
        }

        if ( ImGui::MenuItem( "Save As.." ) ) {
        }

        if ( ImGui::MenuItem( "Quit", "Alt+F4" ) ) {
            quit( );
        }
    }

    auto show_main_menu( instace_t& app ) {
        if ( ImGui::BeginMainMenuBar( ) ) {
            if ( ImGui::BeginMenu( "File" ) ) {
                show_file_menu( app );
                ImGui::EndMenu( );
            }
            if ( ImGui::BeginMenu( "Edit" ) ) {
                if ( ImGui::MenuItem( "Undo", "CTRL+Z" ) ) {
                }
                if ( ImGui::MenuItem( "Redo", "CTRL+Y", false, false ) ) {
                } // Disabled item
                ImGui::Separator( );
                if ( ImGui::MenuItem( "Cut", "CTRL+X" ) ) {
                }
                if ( ImGui::MenuItem( "Copy", "CTRL+C" ) ) {
                }
                if ( ImGui::MenuItem( "Paste", "CTRL+V" ) ) {
                }
                ImGui::Separator( );
                if ( ImGui::MenuItem( "Settings" ) ) {
                }
                ImGui::EndMenu( );
            }
            ImGui::EndMainMenuBar( );
        }
    }

} // namespace editor

#include <SDL2/SDL.h>

extern int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    using namespace std;

    // Get executable name
    const auto app_name = strrchr(argv[0], '/') ? strrchr(argv[0], '/') + 1 : nullptr;
    editor::journal::setup_default( string{app_name} + ".log" );

    auto app = editor::init( "../assets/editor/editor.conf" );
    if ( !app ) {
        editor::journal::error( editor::journal::_GAME, "%", "Couldn't init editor");
    }

    // Init SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        editor::journal::error(editor::journal::_GAME, "Unable to initialize SDL: %", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Setup window
    constexpr char EDITOR_TITLE[] = "IRONFORGE World Editor";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG | SDL_GL_CONTEXT_DEBUG_FLAG);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    SDL_Window* window = SDL_CreateWindow(EDITOR_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(1);

    auto drawable_w = 0;
    auto drawable_h = 0;
    SDL_GL_GetDrawableSize(window, &drawable_w, &drawable_h);

    glLoadFunctions();
    glLoadExtensions();

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    ImGui_ImplSdlGL3_Init(window);

    //ImGui::InitDock();

    bool done = false;
    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSdlGL3_ProcessEvent(&event);

            if (event.type == SDL_QUIT)
                done = true;
        }

        ImGui_ImplSdlGL3_NewFrame(window);

        editor::show_main_menu( app.value() );
        editor::show_scene_properties( app.value() );

        //ImGui::Text("Hello, world!");
        //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        glViewport(0, 0, drawable_w, drawable_h);
        glClearColor(0.3, 0.3, 0.3, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        ImGui_ImplSdlGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplSdlGL3_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    //set_verbosity(verbosity::error);
    //set_verbosity(_SCENE, verbosity::verbose);

    // Create game application
    /*auto res = game::create("../assets/editor/editor.conf");
    if (!is_ok(res)) {
        game::journal::error(game::journal::_GAME, "%", what(res));
        return EXIT_FAILURE;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    auto app = move(get<game::instance_t>(res));

    ImGui_ImplSdlGL3_Init(app.vi.window);

    // Launch the game
    return editor::launch(app);*/

    return 0;
}

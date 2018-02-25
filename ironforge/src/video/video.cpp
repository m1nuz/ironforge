#include <SDL2/SDL_video.h>
#include <glcore_330.h>
#include <video/video.hpp>

#include <GL/ext_texture_filter_anisotropic.h>

int max_supported_anisotropy = 0;

namespace video {
    int32_t max_uniform_components = 0;

    _screen screen;
    _config config;

    //static SDL_Window *window;
    //static SDL_GLContext context;

    auto setup_debug() -> void;

    /*auto init(instance_t &inst, const std::string &title, const int32_t w, const int32_t h, const bool fullscreen, const bool vsync, const bool debug) -> result {
        auto flags = static_cast<uint32_t>(SDL_WINDOW_BORDERLESS | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

        if (fullscreen)
            flags |= SDL_WINDOW_FULLSCREEN;

        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        if (screen.msaa) {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, screen.msaa);
        }

        if (screen.srgb_capable)
            SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl::major_version);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl::minor_version);

        auto context_flags = static_cast<int>(SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
        if (debug)
            context_flags |= SDL_GL_CONTEXT_DEBUG_FLAG;

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, context_flags);

        if (!(window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, flags)))
            return result::error_create_window;

        if (!(context = SDL_GL_CreateContext(window)))
            return result::error_create_context;

        if (vsync)
            SDL_GL_SetSwapInterval(1);

        SDL_ShowWindow(window);

        glLoadFunctions();
        glLoadExtensions();

        if (screen.msaa)
            glEnable(GL_MULTISAMPLE);

        if (screen.srgb_capable)
            glEnable(GL_FRAMEBUFFER_SRGB);

        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        if (debug)
            setup_debug();

        // get constants
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &max_uniform_components);


        // get real window size
        auto drawable_w = 0;
        auto drawable_h = 0;
        SDL_GL_GetDrawableSize(window, &drawable_w, &drawable_h);

        // setup default screen
        screen.width = drawable_w;
        screen.height = drawable_h;
        screen.vsync = vsync;
        screen.aspect = (float)screen.width / (float)screen.height;

        // setup default config
        config.filtering = texture_filtering::trilinear;                

        return result::success;
    }*/

    auto reset(instance_t &inst, const int32_t w, const int32_t h, const bool fullscreen, const bool vsync, const bool debug) -> result {
        /*SDL_SetWindowSize(window, w, h);
        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

        if (fullscreen)
            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

        if (vsync)
            SDL_GL_SetSwapInterval(1);

        // get real window size
        auto drawable_w = 0;
        auto drawable_h = 0;
        SDL_GL_GetDrawableSize(window, &drawable_w, &drawable_h);

        screen.width = drawable_w;
        screen.height = drawable_h;
        screen.vsync = vsync;
        screen.aspect = (float)screen.width / (float)screen.height;*/

        return result::success;
    }

    auto present(instance_t &in, const std::vector<gl::command_buffer *> &buffers) -> void {
        assert(buffers.size() != 0);

        for (auto &buf : buffers) {
            for (auto &c : buf->commands) {

                video::gl::set_color_blend_state(&buf->blend);
                video::gl::set_rasterizer_state(&buf->rasterizer);
                video::gl::set_depth_stencil_state(&buf->depth);

                video::gl::dispath_command(c, *buf);

                video::gl::clear_depth_stencil_state();
                video::gl::clear_rasterizer_state();
                video::gl::clear_color_blend_state();
            }
        }

        SDL_GL_SwapWindow(in.window);
    }

    auto get_string(const result r) -> const char * {
        switch (r) {
        case result::success:
            return "Success";
            break;
        case result::failure:
            return "Failure";
            break;
        case result::error_create_window:
        case result::error_create_context:
            return SDL_GetError();
            break;
        default:
            break;
        }

        return nullptr;
    }

    auto init_once(assets::instance_t &asset, const json &info) -> video_result {
        using namespace std;

        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        const int msaa = (info.find("msaa") != info.end()) ? info["msaa"].get<int>() : 0;
        if (msaa) {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, msaa);
        }

        const bool srgb_capable = (info.find("srgb_capable") != info.end()) ? info["srgb_capable"].get<bool>() : false;
        if (srgb_capable)
            SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

        const bool debug = (info.find("debug") != info.end()) ? info["debug"].get<bool>() : false;
        const auto context_flags = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG | (debug ? SDL_GL_CONTEXT_DEBUG_FLAG : 0);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, context_flags);

        const auto fullscreen = (info.find("fullscreen") != info.end()) ? info["fullscreen"].get<bool>() : false;
        const auto borders = (info.find("borders") != info.end()) ? info["borders"].get<bool>() : false;
        const auto window_flags = static_cast<Uint32>(SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | (!borders ? SDL_WINDOW_BORDERLESS : 0) | (fullscreen ? SDL_WINDOW_FULLSCREEN : 0));
        const auto window_title = (info.find("title") != info.end()) ? info["title"].get<string>() : "Game";
        const auto window_width = (info.find("width") != info.end()) ? info["width"].get<int>() : 1024;
        const auto window_height = (info.find("height") != info.end()) ? info["height"].get<int>() : 768;

        auto window = SDL_CreateWindow(window_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, window_flags);
        if (!window)
            return make_error_code(errc::init_sdl);

        auto graphic = SDL_GL_CreateContext(window);
        if (!graphic)
            return make_error_code(errc::init_opengl);

        const auto vsync = (info.find("vsync") != info.end()) ? info["vsync"].get<bool>() : false;
        if (vsync)
            SDL_GL_SetSwapInterval(1);

        glLoadFunctions();
        glLoadExtensions();

        if (msaa)
            glEnable(GL_MULTISAMPLE);

        if (srgb_capable)
            glEnable(GL_FRAMEBUFFER_SRGB);

        if (debug)
            setup_debug();

        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        // get constants
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &max_uniform_components);
        glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_supported_anisotropy);

        instance_t ctx;
        ctx.window = window;
        ctx.graphic_context = graphic;
        ctx.w = window_width;
        ctx.h = window_height;

        // get real window size
        auto drawable_w = 0;
        auto drawable_h = 0;
        SDL_GL_GetDrawableSize(window, &drawable_w, &drawable_h);

        // setup default screen
        screen.width = drawable_w;
        screen.height = drawable_h;
        screen.vsync = vsync;
        screen.aspect = (float)screen.width / (float)screen.height;

        const auto tf = info.find("texture_filtering") != info.end() ? static_cast<texture_filtering>(info["texture_filtering"].get<uint32_t>()) : texture_filtering::trilinear;

        // setup default config
        const auto tex_filtering = tf >= texture_filtering::max_filtering ? texture_filtering::trilinear : tf;
        config.filtering = tex_filtering;
        ctx.tex_filtering = tex_filtering;
        video::init_resources(ctx, asset);

        return ctx;
    }

    auto cleanup_once(instance_t &in) -> void {
        cleanup_resources();

        SDL_GL_DeleteContext(in.graphic_context);
        SDL_DestroyWindow(in.window);
    }
} // namespace video

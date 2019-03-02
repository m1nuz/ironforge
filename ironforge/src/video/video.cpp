#include <SDL2/SDL_video.h>
#include <glcore_330.h>
#include <video/video.hpp>
#include <video/debug.hpp>
#include <video/glyphs.hpp>
#include <video/journal.hpp>

#include <GL/ext_texture_filter_anisotropic.h>

namespace video {

    ///
    /// \brief Get vendor, renderer, version and shading language version
    /// \param[inout] vi
    /// \return Info string
    ///
    auto get_info(instance_t &vi) -> std::string;

    auto create_resources(instance_t &inst, assets::instance_t &asset, const std::vector<font_info> &fonts) -> void;
    auto cleanup_resources(instance_t &in) -> void;
    auto load_font_infos(const json &info) -> std::vector<font_info>;

    auto load_font_infos(const json &info) -> std::vector<font_info> {
        using namespace std;

        constexpr auto default_size = 12;
        vector<font_info> fonts;

        if (info.find("fonts") != info.end()) {
            for (const auto& f : info["fonts"]) {
                const auto filename = f.find("filename") != f.end() ? f["filename"].get<string>() : string{};
                const auto fontname = f.find("fontname") != f.end() ? f["fontname"].get<string>() : string{};
                const auto size = f.find("size") != f.end() ? f["size"].get<int>() : default_size;
                const auto charset = f.find("charset") != f.end() ? f["charset"].get<string>() : default_charset();

                if (!filename.empty() && !charset.empty() && size > 0)
                    fonts.emplace_back(filename, fontname.empty() ? filename : fontname, size, charset == "default" ? default_charset() : charset);
            }
        }

        return fonts;
    }

    auto init(assets::instance_t &asset, const json &info) -> video_result {
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

        // get real window size
        auto drawable_w = 0;
        auto drawable_h = 0;
        SDL_GL_GetDrawableSize(window, &drawable_w, &drawable_h);

        instance_t ctx;
        ctx.window = window;
        ctx.graphic_context = graphic;
        ctx.w = drawable_w;
        ctx.h = drawable_h;
        ctx.fsaa = info.find("fsaa") != info.end() ? info["fsaa"].get<int>() : 0;
        ctx.aspect_ratio = static_cast<float>(ctx.w) / static_cast<float>(ctx.h);

        if (msaa)
            glEnable(GL_MULTISAMPLE);

        if (srgb_capable)
            glEnable(GL_FRAMEBUFFER_SRGB);

        setup_debug();

        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        // get constants
        glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &ctx.max_uniform_components);
        glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &ctx.max_supported_anisotropy);

        const auto tf = info.find("texture_filtering") != info.end() ? static_cast<texture_filtering>(info["texture_filtering"].get<uint32_t>()) : texture_filtering::trilinear;
        const auto tl = info.find("texture_level") != info.end() ? info["texture_level"].get<uint32_t>() : 0;

        // setup default config
        const auto tex_filtering = tf >= texture_filtering::max_filtering ? texture_filtering::trilinear : tf;
        ctx.texture_filter = tex_filtering;
        ctx.texture_level = tl;

        const auto fonts = load_font_infos(info);
        video::create_resources(ctx, asset, fonts);

        journal::info("%", get_info(ctx));

        return ctx;
    }

    auto present(instance_t &vi, const std::vector<command_queue *> &buffers) -> void {
        assert(buffers.size() != 0);

        for (auto &buf : buffers) {
            for (auto &c : buf->commands) {

                gl::set_color_blend_state(buf->blend);
                gl::set_rasterizer_state(buf->rasterizer);
                gl::set_depth_stencil_state(buf->depth);

                gl::dispath_command(c, *buf);

                gl::clear_depth_stencil_state();
                gl::clear_rasterizer_state();
                gl::clear_color_blend_state();
            }
        }

        SDL_GL_SwapWindow(vi.window);
    }

    auto cleanup(instance_t &vi) -> void {
        cleanup_resources(vi);

        SDL_GL_DeleteContext(vi.graphic_context);
        SDL_DestroyWindow(vi.window);
    }

} // namespace video

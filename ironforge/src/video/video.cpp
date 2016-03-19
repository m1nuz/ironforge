#include <SDL2/SDL_video.h>
#include <glcore_330.h>
#include <sstream>
#include <core/application.hpp>
#include <video/video.hpp>

namespace video {
    auto init_resources() -> void;
    auto cleanup_resources() -> void;

    static SDL_Window *window;
    static SDL_GLContext context;
    static bool debug = true;

    static void APIENTRY debug_output(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
        (void)source, (void)type, (void)id, (void)severity, (void)length, (void)userParam;
        /*auto type_name = static_cast<const char*>(nullptr);
        switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            type_name = "ERROR";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            type_name = "DEPRECATED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            type_name = "UNDEFINED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            type_name = "PORTABILITY";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            type_name = "PERFORMANCE";
            break;
        case GL_DEBUG_TYPE_OTHER:
            type_name = "OTHER";
            break;
        }*/

        application::error(application::log_category::video, "%\n", message);
    }

    static auto enable_debug() -> void {
        auto glDebugMessageCallback = static_cast<PFNGLDEBUGMESSAGECALLBACKPROC>(nullptr);

        auto ARB_debug_output = is_extension_supported("GL_ARB_debug_output");
        auto KHR_debug = is_extension_supported("GL_KHR_debug");
        auto debug_output_synchronous = static_cast<GLenum>(0);

        if (ARB_debug_output) {
            glDebugMessageCallback = reinterpret_cast<PFNGLDEBUGMESSAGECALLBACKPROC>(nativeGetProcAddress("glDebugMessageCallbackARB"));
            debug_output_synchronous = GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB;
        } else if (KHR_debug) {
            glDebugMessageCallback = reinterpret_cast<PFNGLDEBUGMESSAGECALLBACKPROC>(nativeGetProcAddress("glDebugMessageCallback"));
            debug_output_synchronous = GL_DEBUG_OUTPUT_SYNCHRONOUS;
        }

        if (glDebugMessageCallback) {
            glDebugMessageCallback(debug_output, nullptr);
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(debug_output_synchronous);
        }
    }

    auto init(const std::string &title, int32_t w, int32_t h, bool vsync) -> result {
        auto flags = static_cast<uint32_t>(SDL_WINDOW_BORDERLESS | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        /*if (screen.msaa) {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, screen.msaa);
        }*/

        /*if (screen.srgb_capable)
            SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);*/

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

        /*if (msaa)
            glEnable(GL_MULTISAMPLE);

        if (srgb_capable)
            glEnable(GL_FRAMEBUFFER_SRGB);*/

        if (debug)
            enable_debug();

        init_resources();

        return result::success;
    }

    auto cleanup() -> void {
        cleanup_resources();

        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
    }

    auto present(std::vector<gl::command_buffer *>&& buffers) -> void {
        for (auto &buf : buffers) {
            for (auto &c : buf->commands)
                video::gl::dispath_command(c, *buf);
        }

        SDL_GL_SwapWindow(window);
    }

    auto get_string(result r) -> const char * {
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

    auto is_extension_supported(const char *extension) -> bool {
#ifdef GL_VERSION_3_0
        if (glGetStringi == NULL) {
            if (strstr((const char *)glGetString(GL_EXTENSIONS), extension) != NULL)
                return true;

            return false;
        }

        auto num_extensions = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);

        for (auto i = 0; i < num_extensions; i++)
            if (strcmp((const char *)glGetStringi(GL_EXTENSIONS, i), extension) == 0)
                return true;
#else
        if (strstr((const char *)glGetString(GL_EXTENSIONS), extension) != NULL)
            return true;
#endif
        return false;
    }

    auto get_info() -> const char * {
        static auto vendor = static_cast<const GLubyte *>(nullptr);
        static auto renderer = static_cast<const GLubyte *>(nullptr);
        static auto version = static_cast<const GLubyte *>(nullptr);
        static auto shading_language_version = static_cast<const GLubyte *>(nullptr);
        static std::string info;

        if (!vendor || !renderer || !version || !shading_language_version) {
            vendor = glGetString(GL_VENDOR);
            renderer = glGetString(GL_RENDERER);
            version = glGetString(GL_VERSION);
            shading_language_version = glGetString(GL_SHADING_LANGUAGE_VERSION);

            std::stringstream s;
            s << std::endl
              << "\tVendor: " << vendor << std::endl
              << "\tRender: " << renderer << std::endl
              << "\tVersion: " << version << std::endl
              << "\tShading language version: " << shading_language_version;

            info = s.str();
        }

        return info.c_str();
    }

    auto is_debugging() -> bool {
        return debug;
    }
} // namespace video

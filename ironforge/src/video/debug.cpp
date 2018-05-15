#include <glcore_450.h>

#include <core/journal.hpp>
#include <video/video.hpp>
#include <video/debug.hpp>
#include <ui/types.hpp>

namespace video {
    static void APIENTRY debug_output(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
        (void)source, (void)type, (void)id, (void)severity, (void)length, (void)userParam;

        using namespace game;

        switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            //type_name = "ERROR";
            journal::error(journal::_VIDEO, "%", message);
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            //type_name = "DEPRECATED_BEHAVIOR";
            journal::warning(journal::_VIDEO, "%", message);
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            //type_name = "UNDEFINED_BEHAVIOR";
            journal::warning(journal::_VIDEO, "%", message);
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            journal::warning(journal::_VIDEO, "%", message);
            //type_name = "PORTABILITY";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            journal::warning(journal::_VIDEO, "%", message);
            //type_name = "PERFORMANCE";
            break;
        case GL_DEBUG_TYPE_OTHER:
            journal::warning(journal::_VIDEO, "%", message);
            //type_name = "OTHER";
            break;
        default:
            journal::debug(journal::_VIDEO, "%", message);
        }
    }

    auto setup_debug() -> void {
        union {
            PFNGLDEBUGMESSAGECALLBACKPROC glDebugMessageCallback;
            void *ptr;
        } cb = {nullptr};

        auto ARB_debug_output = is_extension_supported("GL_ARB_debug_output");
        auto KHR_debug = is_extension_supported("GL_KHR_debug");
        auto debug_output_synchronous = static_cast<GLenum>(0);

        if (ARB_debug_output) {
            cb.ptr = nativeGetProcAddress("glDebugMessageCallbackARB");
            debug_output_synchronous = GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB;
        } else if (KHR_debug) {
            cb.ptr = nativeGetProcAddress("glDebugMessageCallback");
            debug_output_synchronous = GL_DEBUG_OUTPUT_SYNCHRONOUS;
        }

        if (cb.glDebugMessageCallback) {
            cb.glDebugMessageCallback(debug_output, nullptr);
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(debug_output_synchronous);
        }
    }

    auto debug_text(video::instance_t &vi, std::unique_ptr<renderer::instance> &render, const float pos_x, const float pos_y, std::string_view text, uint32_t color) -> void {
        ui::draw_commands::draw_text dt;
        dt.align = 0;
        dt.w = 0.f;
        dt.h = 0.f;
        dt.color = color;
        dt.font = video::get_font(vi, "roboto");
        dt.x = pos_x;
        dt.y = pos_y;
        dt.text = text;

        render->dispath(vi, dt);
    }
} // namespace video

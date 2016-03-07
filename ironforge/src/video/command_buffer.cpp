#include <glcore_330.h>
#include <core/application.hpp>
#include <video/video.hpp>
#include <video/command_buffer.hpp>

namespace video {
    namespace gl330 {
        auto dispath_command(const command &c, command_buffer &buf) -> void {
            switch (c.type) {
            case command_type::clear:
                glClearColor(buf.clear_color.x, buf.clear_color.y, buf.clear_color.z, buf.clear_color.w);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                break;
            default:
                application::warning(application::log_category::video, "Unknown command %\n", static_cast<uint32_t>(c.type));
                break;
            }
        }
    } // namespace gl330
} // namespace video

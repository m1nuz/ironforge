#include <glcore_330.h>
#include <core/application.hpp>
#include <video/buffer.hpp>

namespace video {
    namespace gl330 {
        inline uint32_t get_buffer_target(buffer_target target) {
            switch (target) {
            case buffer_target::array:
                return GL_ARRAY_BUFFER;
            case buffer_target::element_array:
                return GL_ELEMENT_ARRAY_BUFFER;
            case buffer_target::texture:
                return GL_TEXTURE_BUFFER;
            case buffer_target::uniform:
                return GL_UNIFORM_BUFFER;
            }

            return GL_NONE;
        }

        inline uint32_t get_buffer_usage(buffer_usage usage) {
            switch (usage) {
            case buffer_usage::stream_draw:
                return GL_STREAM_DRAW;
            case buffer_usage::stream_read:
                return GL_STREAM_READ;
            case buffer_usage::stream_copy:
                return GL_STREAM_COPY;
            case buffer_usage::static_draw:
                return GL_STATIC_DRAW;
            case buffer_usage::static_read:
                return GL_STATIC_READ;
            case buffer_usage::static_copy:
                return GL_STATIC_COPY;
            case buffer_usage::dynamic_draw:
                return GL_DYNAMIC_DRAW;
            case buffer_usage::dynamic_read:
                return GL_DYNAMIC_READ;
            case buffer_usage::dynamic_copy:
                return GL_DYNAMIC_COPY;
            }

            return GL_NONE;
        }

        auto create_buffer(buffer_target target, size_t size, const void *ptr, buffer_usage usage) -> buffer {
            GLuint buf;
            glGenBuffers(1, &buf);

            uint32_t bt = get_buffer_target(target);
            uint32_t bu = get_buffer_usage(usage);

            glBindBuffer(bt, buf);
            glBufferData(bt, size, ptr, bu);

            return buffer{buf, bt, bu};
        }

        auto destroy_buffer(buffer &buf) -> void {
            if (glIsBuffer(buf.id))
                application::warning(application::log_category::video, "Trying delete not buffer %\n", buf.id);

            glDeleteBuffers(1, &buf.id);
            buf.id = 0;
        }
    } // namespace gl330
} // namespace video

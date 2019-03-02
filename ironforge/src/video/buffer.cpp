#include <algorithm>
#include <map>

#include <glcore_330.h>
#include <video/journal.hpp>
#include <video/buffer.hpp>

namespace video {

    namespace gl330 {

        constexpr char ARRAY_TARGET_NAME[] = "array";
        constexpr char ELEMENT_TARGET_ARRAY_NAME[] = "element_array";
        constexpr char TEXTURE_TARGET_NAME[] = "texture";
        constexpr char UNIFORM_TARGET_NAME[] = "uniform";

        inline auto get_buffer_target(const buffer_target target) -> uint32_t {
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

        inline auto get_buffer_target_name(const uint32_t target) -> const char * {
            switch (target) {
            case GL_ARRAY_BUFFER:
                return ARRAY_TARGET_NAME;
            case GL_ELEMENT_ARRAY_BUFFER:
                return ELEMENT_TARGET_ARRAY_NAME;
            case GL_TEXTURE_BUFFER:
                return TEXTURE_TARGET_NAME;
            case GL_UNIFORM_BUFFER:
                return UNIFORM_TARGET_NAME;
            }

            return "unknown";
        }

        constexpr inline uint32_t get_buffer_usage(const buffer_usage usage) {
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

        auto create_buffer(const buffer_target target, const size_t size, const void *ptr, const buffer_usage usage) -> buffer {
            GLuint buf;
            glGenBuffers(1, &buf);

            const uint32_t buf_target = get_buffer_target(target);
            const uint32_t buf_usage = get_buffer_usage(usage);

            glBindBuffer(buf_target, buf);
            glBufferData(buf_target, static_cast<GLsizeiptr>(size), ptr, buf_usage);

            journal::debug("Create % buffer %", get_buffer_target_name(buf_target), buf);

            return buffer{buf, buf_target, buf_usage};
        }

        auto destroy_buffer(buffer &buf) -> void {
            if (!glIsBuffer(buf.id))
                journal::warning("Trying delete not buffer %", buf.id);

            journal::debug("Destroy % buffer %", get_buffer_target_name(buf.target), buf.id);

            glDeleteBuffers(1, &buf.id);
            buf.id = 0;
        }

        auto bind_buffer(buffer &buf) -> void {
            glBindBuffer(buf.target, buf.id);
        }

        auto unbind_buffer(buffer &buf) -> void {
            glBindBuffer(buf.target, 0);
        }

        auto update_buffer(buffer &buf, const size_t offset, const void *data, const size_t size) -> void {
            glBindBuffer(buf.target, buf.id);
            glBufferSubData(buf.target, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size), data);
        }

        constexpr inline auto get_buffer_access(buffer_access access) -> uint32_t {
            switch (access) {
            case buffer_access::read_only:
                return GL_READ_ONLY;
            case buffer_access::write_only:
                return GL_WRITE_ONLY;
            case buffer_access::read_write:
                return GL_READ_WRITE;
            }

            return GL_NONE;
        }

        auto map_buffer(buffer &buf, const buffer_access access) -> void* {
            glBindBuffer(buf.target, buf.id);
            return glMapBuffer(buf.target, get_buffer_access(access));
        }

        auto unmap_buffer(buffer &buf) -> bool {
            return glUnmapBuffer(buf.target) == GL_TRUE;
        }

    } // namespace gl330

} // namespace video

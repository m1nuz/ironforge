#include <glcore_330.h>
#include <core/application.hpp>
#include <video/renderbuffer.hpp>

#include "texture_format.inl"

namespace video {
    namespace gl330 {
        auto create_renderbuffer(const renderbuffer_info &info) -> renderbuffer {
            GLuint buf = 0;
            glGenRenderbuffers(1, &buf);
            glBindRenderbuffer(GL_RENDERBUFFER, buf);

            auto internalformat = static_cast<GLint >(0);
            auto format = static_cast<GLenum>(0);
            auto type = static_cast<GLenum>(0);

            get_texture_format_from_pixelformat(info.pf, internalformat, format, type);

            if (info.samples == 0)
                glRenderbufferStorage(GL_RENDERBUFFER, internalformat, info.width, info.height);
            else
                glRenderbufferStorageMultisample(GL_RENDERBUFFER, info.samples, internalformat, info.width, info.height);

            return {buf, info.width, info.height, info.samples};
        }

        auto destroy_renderbuffer(renderbuffer &buf) -> void {
            if (!glIsRenderbuffer(buf.id))
                application::debug(application::log_category::video, "Trying delete not renderbuffer %\n", buf.id);

            glDeleteRenderbuffers(1, &buf.id);
            buf.id = 0;
        }

    } // namespace gl330
} // namespace video

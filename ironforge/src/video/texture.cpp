#include <glcore_330.h>
#include <core/application.hpp>
#include <core/assets.hpp>
#include <video/video.hpp>

#include "texture_format.inl"

namespace video {
    namespace gl330 {
        auto create_texture_2d(const texture_info &info) -> texture {
            auto tex = static_cast<GLuint>(0);
            glGenTextures(1, &tex);
            glBindTexture(GL_TEXTURE_2D, tex);

            auto internalformat = static_cast<GLint >(0);
            auto format = static_cast<GLenum>(0);
            auto type = static_cast<GLenum>(0);
            auto w = static_cast<GLsizei>(info.width);
            auto h = static_cast<GLsizei>(info.height);
            void *pixels = info.pixels;
            auto flags = info.flags;

            get_texture_format_from_pixelformat(info.format, internalformat, format, type);

            glTexImage2D(GL_TEXTURE_2D, 0, internalformat, w, h, 0, format, type, pixels);

            if (flags & static_cast<uint32_t>(texture_flags::auto_mipmaps))
                glGenerateMipmap(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, 0);

            application::debug(application::log_category::video, "Create texture %\n", tex);

            return {(uint32_t)tex, GL_TEXTURE_2D, flags, w, h, 0};
        }

        auto create_texture_2d(const assets::image_data &data) -> texture {
            uint32_t flags = 0;
            switch (config.filtering) {
            case texture_filtering::bilinear:
                break;
            case texture_filtering::trilinear:
            case texture_filtering::anisotropic:
                flags |= static_cast<uint32_t>(texture_flags::auto_mipmaps);
                break;
            }

            return create_texture_2d({data.pixelformat, 0, flags, static_cast<int32_t>(data.width), static_cast<int32_t>(data.height), 0, data.pixels});
        }

        auto destroy_texture(texture &tex) -> void {
            if (glIsTexture(tex.id)) {
                application::debug(application::log_category::video, "Delete texture %\n", tex.id);
                glDeleteTextures(1, &tex.id);
            }

            tex.id = 0;
        }
    } // namespace gl330
} // namespace video


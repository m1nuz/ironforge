#include <glcore_330.h>
#include <core/application.hpp>
#include <core/assets.hpp>
#include <video/video.hpp>

namespace video {
    namespace gl330 {
        // TODO: process result or make error
        inline auto get_texture_format_from_pixelformt(pixel_format pf, GLint *internalformat, GLenum *format, GLenum *type) -> void {
            // TODO: make assert when any ptr is null
            switch (pf) {
            case pixel_format::bgr8:
                *internalformat = GL_RGB8;
                *format = GL_BGR;
                *type = GL_UNSIGNED_BYTE;
                break;
            case pixel_format::bgra8:
                *internalformat = GL_RGBA8;
                *format = GL_BGRA;
                *type = GL_UNSIGNED_BYTE;
                break;
            case pixel_format::rgb8:
                *internalformat = GL_RGB8;
                *format = GL_RGB;
                *type = GL_UNSIGNED_BYTE;
                break;
            case pixel_format::rgba8:
                *internalformat = GL_RGBA8;
                *format = GL_RGBA;
                *type = GL_UNSIGNED_BYTE;
                break;
            case pixel_format::rgb16f:
                *internalformat = GL_RGB16F;
                *format = GL_RGBA;
                *type = GL_HALF_FLOAT;
                break;
            case pixel_format::rgba16f:
                *internalformat = GL_RGBA16F;
                *format = GL_RGBA;
                *type = GL_HALF_FLOAT;
                break;
            case pixel_format::rgb32f:
                *internalformat = GL_RGB32F;
                *format = GL_RGB;
                *type = GL_FLOAT;
                break;
            case pixel_format::rgba32f:
                *internalformat = GL_RGBA32F;
                *format = GL_RGBA;
                *type = GL_FLOAT;
                break;
            default:
                application::error(application::log_category::video, "% %\n", "Unknown texture format for pixel format", static_cast<uint32_t>(pf));
                break;
            }
        }

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

            get_texture_format_from_pixelformt(info.format, &internalformat, &format, &type);

            glTexImage2D(GL_TEXTURE_2D, 0, internalformat, w, h, 0, format, type, pixels);

            if (flags & static_cast<uint32_t>(texture_flags::auto_mipmaps))
                glGenerateMipmap(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, 0);

            application::debug(application::log_category::video, "Create texture %\n", tex);

            return {(uint32_t)tex, GL_TEXTURE_2D, flags, w, h, 0};
        }

        auto create_texture_2d(const assets::image_data &data) -> texture {
            return create_texture_2d({data.pixelformat, 0, 0, static_cast<int32_t>(data.width), static_cast<int32_t>(data.height), 0, data.pixels});
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


namespace video {
    namespace gl330 {
        inline auto get_texture_format_from_pixelformat(pixel_format pf, GLint &internalformat, GLenum &format, GLenum &type) -> void {
            switch (pf) {
            case pixel_format::bgr8:
                internalformat = GL_RGB8;
                format = GL_BGR;
                type = GL_UNSIGNED_BYTE;
                break;
            case pixel_format::bgra8:
                internalformat = GL_RGBA8;
                format = GL_BGRA;
                type = GL_UNSIGNED_BYTE;
                break;
            case pixel_format::rgb8:
                internalformat = GL_RGB8;
                format = GL_RGB;
                type = GL_UNSIGNED_BYTE;
                break;
            case pixel_format::rgba8:
                internalformat = GL_RGBA8;
                format = GL_RGBA;
                type = GL_UNSIGNED_BYTE;
                break;
            case pixel_format::rgb16f:
                internalformat = GL_RGB16F;
                format = GL_RGB;
                type = GL_HALF_FLOAT;
                break;
            case pixel_format::rgba16f:
                internalformat = GL_RGBA16F;
                format = GL_RGBA;
                type = GL_HALF_FLOAT;
                break;
            case pixel_format::rgb32f:
                internalformat = GL_RGB32F;
                format = GL_RGB;
                type = GL_FLOAT;
                break;
            case pixel_format::rgba32f:
                internalformat = GL_RGBA32F;
                format = GL_RGBA;
                type = GL_FLOAT;
                break;
            case pixel_format::depth:
                internalformat = GL_DEPTH_COMPONENT;
                format = GL_DEPTH_COMPONENT;
                type = GL_UNSIGNED_SHORT;
                break;
            default:
                game::journal::error(game::journal::category::video, "% %\n", "Unknown texture format for pixel format", static_cast<uint32_t>(pf));
                break;
            }
        }
    } // namespace gl330
} // namespace video

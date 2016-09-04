#include <sstream>

#include <video/video.hpp>
#include <glcore_330.h>

namespace video {
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
} // namespace video

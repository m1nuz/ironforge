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

    auto get_info(instance_t &vi) -> std::string {
        if (vi.vendor.empty())
            vi.vendor = reinterpret_cast<const char *>(glGetString(GL_VENDOR));

        if (vi.renderer.empty())
            vi.renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

        if (vi.version.empty())
            vi.version = reinterpret_cast<const char *>(glGetString(GL_VERSION));

        if (vi.shading_language_version.empty())
            vi.shading_language_version = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));

        std::stringstream s;
        s << std::endl
          << "\tVendor: " << vi.vendor << std::endl
          << "\tRender: " << vi.renderer << std::endl
          << "\tVersion: " << vi.version << std::endl
          << "\tShading language version: " << vi.shading_language_version;

        return s.str();
    }

} // namespace video

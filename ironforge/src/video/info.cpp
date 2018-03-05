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

    auto get_info(instance_t &inst) -> std::string {
        if (inst.vendor.empty())
            inst.vendor = reinterpret_cast<const char *>(glGetString(GL_VENDOR));

        if (inst.renderer.empty())
            inst.renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

        if (inst.version.empty())
            inst.version = reinterpret_cast<const char *>(glGetString(GL_VERSION));

        if (inst.shading_language_version.empty())
            inst.shading_language_version = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));

        std::stringstream s;
        s << std::endl
          << "\tVendor: " << inst.vendor << std::endl
          << "\tRender: " << inst.renderer << std::endl
          << "\tVersion: " << inst.version << std::endl
          << "\tShading language version: " << inst.shading_language_version;

        return s.str();
    }
} // namespace video

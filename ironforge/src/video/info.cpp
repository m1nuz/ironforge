#include <sstream>

#include <video/video.hpp>
#include <glcore_330.h>

namespace video {

    auto is_extension_supported(const char *extension) -> bool {
#ifdef GL_VERSION_3_0
        if (glGetStringi == nullptr) {
            if (strstr(reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS)), extension) != nullptr)
                return true;

            return false;
        }

        auto num_extensions = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);

        if (num_extensions > 0) {

            for (auto i = 0u; i < static_cast<unsigned int>(num_extensions); i++)
                if (strcmp(reinterpret_cast<const char *>(glGetStringi(GL_EXTENSIONS, i)), extension) == 0)
                    return true;
        }
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

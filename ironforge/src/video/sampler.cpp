#include <glcore_330.h>
#include <GL/ext_texture_filter_anisotropic.h>
#include <video/journal.hpp>
#include <video/sampler.hpp>

namespace video {

    namespace gl330 {

        inline auto get_texture_mag_filter(texture_mag_filter filter) -> int32_t {
            switch (filter) {
            case texture_mag_filter::nearest:
                return GL_NEAREST;
            case texture_mag_filter::linear:
                return GL_LINEAR;
            }

            return GL_NONE;
        }

        inline auto get_texture_min_filter(texture_min_filter filter)  -> int32_t {
            switch (filter) {
            case texture_min_filter::nearest:
                return GL_NEAREST;
            case texture_min_filter::linear:
                return GL_LINEAR;
            case texture_min_filter::nearest_mipmap_nearest:
                return GL_NEAREST_MIPMAP_NEAREST;
            case texture_min_filter::linear_mipmap_nearest:
                return GL_LINEAR_MIPMAP_NEAREST;
            case texture_min_filter::nearest_mipmap_linear:
                return GL_NEAREST_MIPMAP_LINEAR;
            case texture_min_filter::linear_mipmap_linear:
                return GL_LINEAR_MIPMAP_LINEAR;
            }

            journal::debug("Texture min filter not found %", static_cast<uint32_t>(filter));

            return GL_NONE;
        }

        inline auto get_texture_wrap(texture_wrap wrap) -> int32_t {
            switch (wrap) {
            case texture_wrap::clamp_to_edge:
                return GL_CLAMP_TO_EDGE;
            case texture_wrap::mirrored_repeat:
                return GL_MIRRORED_REPEAT;
            case texture_wrap::repeat:
                return GL_REPEAT;
            case texture_wrap::clamp_to_border:
                return GL_CLAMP_TO_BORDER;
            }

            journal::debug("Texture wrap not found %", static_cast<uint32_t>(wrap));

            return GL_NONE;
        }

        auto create_sampler(const sampler_info &info) -> sampler {
            GLuint sid;
            glGenSamplers(1, &sid);

            glSamplerParameteri(sid, GL_TEXTURE_MAG_FILTER, get_texture_mag_filter(info.mag_filter));
            glSamplerParameteri(sid, GL_TEXTURE_MIN_FILTER, get_texture_min_filter(info.min_filter));

            glSamplerParameteri(sid, GL_TEXTURE_WRAP_S, get_texture_wrap(info.wrap_s));
            glSamplerParameteri(sid, GL_TEXTURE_WRAP_T, get_texture_wrap(info.wrap_t));
            //glSamplerParameteri(sid, GL_TEXTURE_WRAP_R, get_texture_wrap(info.wrap_r));

            if (info.compare_mode != GL_NONE)
                glSamplerParameteri(sid, GL_TEXTURE_COMPARE_MODE, info.compare_mode); // initial GL_NONE

            if (info.compare_func != GL_NONE)
                glSamplerParameterf(sid, GL_TEXTURE_COMPARE_FUNC, info.compare_func); // initial GL_NEVER

            if(info.anisotropy > 0) {
                glSamplerParameteri(sid, GL_TEXTURE_MAX_ANISOTROPY_EXT, info.anisotropy);
            }

            journal::debug("Create sampler %", sid);

            return {sid};
        }

        auto destroy_sampler(sampler &sam) -> void {
            if (glIsSampler(sam.id)) {
                journal::debug("Destroy sampler %", sam.id);
                glDeleteSamplers(1, &sam.id);
            }

            sam.id = 0;
        }

        auto bind_sampler(const sampler &sam, uint32_t unit) -> void {
            glBindSampler(unit, sam.id);
        }

        auto unbind_sampler(const sampler &sam, uint32_t unit) -> void {
            (void)sam;

            glBindSampler(unit, 0);
        }

    } // namespace gl330

} // namespace video

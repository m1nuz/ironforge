#pragma once

#include <cstdint>

namespace video {

    namespace gl330 {

        enum class texture_min_filter : int32_t {
            nearest,
            linear,
            nearest_mipmap_nearest,
            linear_mipmap_nearest,
            nearest_mipmap_linear,
            linear_mipmap_linear,
        };

        enum class texture_mag_filter : int32_t {
            nearest,
            linear,
        };

        enum class texture_wrap : int32_t {
            clamp_to_edge,
            mirrored_repeat,
            repeat,
            clamp_to_border
        };

        //enum class texture_compare_mode
        //enum class texture_compare_func

        struct sampler_info {
            texture_mag_filter mag_filter = texture_mag_filter::nearest;
            texture_min_filter min_filter = texture_min_filter::nearest;
            texture_wrap wrap_s = texture_wrap::repeat;
            texture_wrap wrap_t = texture_wrap::repeat;
            texture_wrap wrap_r = texture_wrap::repeat;
            int32_t compare_mode = 0;
            int32_t compare_func = 0;
            int32_t anisotropy = 0;
        };

        struct sampler {
            uint32_t id;
        };

        auto create_sampler(const sampler_info &info) -> sampler;
        auto destroy_sampler(sampler &sam) -> void;

        auto bind_sampler(const sampler &sam, uint32_t unit) -> void;
        auto unbind_sampler(const sampler &sam, uint32_t unit) -> void;

    } // namespace gl330

} // namespace video

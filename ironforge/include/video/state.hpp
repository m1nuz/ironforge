#pragma once

#include <cstdint>

namespace video {

    namespace gl330 {

        enum class blend_factor : uint32_t {
            zero,
            one,
            src_color,
            one_minus_src_color,
            dst_color,
            one_minus_dst_color,
            src_alpha,
            one_minus_src_alpha,
            dst_alpha,
            one_minus_dst_alpha,
            constant_color,
            one_minus_constant_color,
            constant_alpha,
            one_minus_constant_alpha
        };

        enum class cull_face_mode : uint32_t {
            front,
            back,
            front_and_back
        };

        enum class polygon_mode_fill : uint32_t {
            point,
            line,
            fill
        };

        enum class depth_fn : uint32_t {
            never,
            less,
            equal,
            lequal,
            greater,
            notequal,
            gequal,
            always
        };

        struct color_blend_state {
            bool            enable = false;
            blend_factor    sfactor = blend_factor::zero;
            blend_factor    dfactor = blend_factor::zero;
        };

        struct rasterizer_state {
            bool                cull_face = false;
            cull_face_mode      cull_mode = cull_face_mode::back;
            bool                polygon_mode = false;
            polygon_mode_fill   fill_mode = polygon_mode_fill::fill;
            bool                discard = false;
        };

        struct depth_stencil_state {
            bool        depth_test = false;
            bool        depth_write = true;
            //bool        depth_clamp;
            depth_fn    depth_func = depth_fn::less;
            /*bool        stencil_test;
            struct {
                uint32_t    func;
                uint8_t     mask;

                uint32_t    sfail;
                uint32_t    dpfail;
                uint32_t    dppass;
            } front, back;*/
        };

        auto set_color_blend_state(const color_blend_state &state) -> void;
        auto set_rasterizer_state(const rasterizer_state &state) -> void;
        auto set_depth_stencil_state(const depth_stencil_state &state) -> void;

        auto clear_color_blend_state() -> void;
        auto clear_rasterizer_state() -> void;
        auto clear_depth_stencil_state() -> void;

    } // namespace gl330

} // namespace video

#include <glcore_330.h>

#include <core/journal.hpp>
#include <video/state.hpp>

namespace video {
    namespace gl330 {
        inline auto get_blend_factor(blend_factor factor) -> uint32_t {
            switch (factor) {
            case blend_factor::zero:
                return GL_ZERO;
            case blend_factor::one:
                return GL_ONE;
            case blend_factor::src_color:
                return GL_SRC_COLOR;
            case blend_factor::one_minus_src_color:
                return GL_ONE_MINUS_SRC_COLOR;
            case blend_factor::dst_color:
                return GL_DST_COLOR;
            case blend_factor::one_minus_dst_color:
                return GL_ONE_MINUS_DST_COLOR;
            case blend_factor::src_alpha:
                return GL_SRC_ALPHA;
            case blend_factor::one_minus_src_alpha:
                return GL_ONE_MINUS_SRC_ALPHA;
            case blend_factor::dst_alpha:
                return GL_DST_ALPHA;
            case blend_factor::one_minus_dst_alpha:
                return GL_ONE_MINUS_DST_ALPHA;
            case blend_factor::constant_color:
                return GL_CONSTANT_COLOR;
            case blend_factor::one_minus_constant_color:
                return GL_ONE_MINUS_CONSTANT_COLOR;
            case blend_factor::constant_alpha:
                return GL_CONSTANT_ALPHA;
            case blend_factor::one_minus_constant_alpha:
                return GL_ONE_MINUS_CONSTANT_ALPHA;
            }

            game::journal::warning(game::journal::_VIDEO, "Unknown blend factor %\n", static_cast<uint32_t>(factor));

            return GL_NONE;
        }

        inline auto get_cull_face_mode(cull_face_mode mode) -> uint32_t {
            switch (mode) {
            case cull_face_mode::front:
                return GL_FRONT;
            case cull_face_mode::back:
                return GL_BACK;
            case cull_face_mode::front_and_back:
                return GL_FRONT_AND_BACK;
            }

            game::journal::warning(game::journal::_VIDEO, "Unknown cull face mode %\n", static_cast<uint32_t>(mode));

            return GL_NONE;
        }

        inline auto get_polygon_mode(polygon_mode_fill mode) -> uint32_t {
            switch (mode) {
            case polygon_mode_fill::point:
                return GL_POINT;
            case polygon_mode_fill::line:
                return GL_LINE;
            case polygon_mode_fill::fill:
                return GL_FILL;
            }

            game::journal::warning(game::journal::_VIDEO, "Unknown polygon mode fill %\n", static_cast<uint32_t>(mode));

            return GL_NONE;
        }

        inline auto get_depth_func(depth_fn f) -> uint32_t {
            switch (f) {
            case depth_fn::never:
                return GL_NEVER;
            case depth_fn::less:
                return GL_LESS;
            case depth_fn::equal:
                return GL_EQUAL;
            case depth_fn::lequal:
                return GL_LEQUAL;
            case depth_fn::greater:
                return GL_GREATER;
            case depth_fn::notequal:
                return GL_NOTEQUAL;
            case depth_fn::gequal:
                return GL_GEQUAL;
            case depth_fn::always:
                return GL_ALWAYS;
            }

            game::journal::warning(game::journal::_VIDEO, "Unknown depth func %\n", static_cast<uint32_t>(f));

            return GL_NONE;
        }

        auto set_color_blend_state(const color_blend_state *restrict state) -> void {
            if (!state->enable)
                return;

            glEnable(GL_BLEND);
            glBlendFunc(get_blend_factor(state->sfactor), get_blend_factor(state->dfactor));
        }

        auto set_rasterizer_state(const rasterizer_state *restrict state) -> void {
            if (state->cull_face) {
                glEnable(GL_CULL_FACE);
                glCullFace(get_cull_face_mode(state->cull_mode));
            }
            if (state->polygon_mode) {
                glPolygonMode(GL_FRONT_AND_BACK, get_polygon_mode(state->fill_mode));
            }

            if (state->discard)
                glEnable(GL_RASTERIZER_DISCARD);
        }

        auto set_depth_stencil_state(const depth_stencil_state *restrict state) -> void {
            if (state->depth_test) {
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(get_depth_func(state->depth_func));
            }

            if (state->depth_write)
                glDepthMask(GL_TRUE);
            else
                glDepthMask(GL_FALSE);
        }

        auto clear_color_blend_state() -> void {
            glDisable(GL_BLEND);
        }

        auto clear_rasterizer_state() -> void {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDisable(GL_RASTERIZER_DISCARD);
            glDisable(GL_CULL_FACE);
        }

        auto clear_depth_stencil_state() -> void {
            glDepthMask(GL_TRUE);
            glDisable(GL_DEPTH_TEST);
        }
    } // namespace gl330
} // namespace video

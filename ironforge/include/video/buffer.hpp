#pragma once

#include <cstddef>
#include <cstdint>

namespace video {
    namespace gl330 {
        enum class buffer_target : uint32_t {
            array,
            element_array,
            texture,
            uniform
        };

        enum class buffer_usage : uint32_t {
            stream_draw,
            stream_read,
            stream_copy,
            static_draw,
            static_read,
            static_copy,
            dynamic_draw,
            dynamic_read,
            dynamic_copy
        };

        struct buffer {
            buffer() = default;
            inline buffer(uint32_t _id, uint32_t _target, uint32_t _usage) : id{_id}, target{_target}, usage(_usage) {

            }

            uint32_t    id     = 0;
            uint32_t    target = 0;
            uint32_t    usage  = 0;
        };

        struct binding_buffer {
            uint32_t    binding_index = 0;
            buffer      *buf          = nullptr;
            ptrdiff_t   offset        = 0;
            int         stride        = 0;
        };

        auto create_buffer(buffer_target target, size_t size, const void *ptr, buffer_usage usage) -> buffer;
        auto destroy_buffer(buffer &buf) -> void;
    } // namespace gl330
} // namespace video

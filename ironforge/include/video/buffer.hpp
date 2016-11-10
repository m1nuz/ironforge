#pragma once

#include <cstddef>
#include <cstdint>

namespace video {
    namespace gl330 {
        enum class buffer_target : uint32_t {
            array,
            element_array,
            texture,
            uniform,
            //transform_feedback,
            //copy_read,
            //copy_write
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

        enum class buffer_access : uint32_t {
            read_only,
            write_only,
            read_write
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
            uint32_t    buf           = 0;
            ptrdiff_t   offset        = 0;
            int         stride        = 0;
        };

        auto create_buffer(const buffer_target target, const size_t size, const void *ptr, const buffer_usage usage) -> buffer;
        auto destroy_buffer(buffer &buf) -> void;
        auto bind_buffer(buffer &buf) -> void;
        auto unbind_buffer(buffer &buf) -> void;
        // auto bind_buffer_range() -> void;
        // auto bind_buffer_base() -> void;
        auto update_buffer(buffer &buf, const size_t offset, const void *data, const size_t size) -> void;
        auto map_buffer(buffer &buf, const buffer_access access) -> void*;
        auto unmap_buffer(buffer &buf) -> bool;
        // copy_buffer
    } // namespace gl330
} // namespace video

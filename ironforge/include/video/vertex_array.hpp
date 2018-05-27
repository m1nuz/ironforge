#pragma once

#include <cstdint>
#include <vector>

#include <video/vertices.hpp>
#include <video/buffer.hpp>

namespace video {

    namespace gl330 {

        enum class attrib_type : uint32_t {
            // int types
            byte_value,
            unsigned_byte_value,
            short_value,
            unsigned_short_value,
            int_value,
            unsigned_int_value,
            // float types
            half_float_value,
            float_value,
            double_value,
            int_2_10_10_10_rev,
            unsigned_int_2_10_10_10_rev
        };

        struct vertex_attribute {
            uint32_t    binding_index; // The index of the vertex buffer binding point to which to bind the buffer
            uint32_t    attrib_index;  // The generic vertex attribute array being described

            int32_t     size;
            uint32_t    type;
            bool        normalized;
            uint32_t    relative_offset;
        };

        struct vertex_array {
            uint32_t id;
            std::vector<vertex_attribute> attribs;
            binding_buffer vb;
            binding_buffer eb;
        };

        auto create_vertex_array() -> vertex_array;
        auto create_vertex_array(std::vector<vertex_attribute> &attribs, const binding_buffer &vb, const binding_buffer &eb) -> vertex_array;
        auto destroy_vertex_array(vertex_array &va) -> void;
        auto bind_vertex_array(const vertex_array &va) -> void;
        auto unbind_vertex_array(const vertex_array &va) -> void;
        auto vertex_array_buffer(vertex_array &va, buffer &buf, ptrdiff_t offset, ptrdiff_t stride) -> void;
        auto vertex_array_format(vertex_array &va, vertex_attributes attribindex, int32_t size, attrib_type type, bool normalized, uint32_t relative_offset) -> void;

    } // namespace gl330

} // namespace video

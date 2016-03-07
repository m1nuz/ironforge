#pragma once

#include <cstdint>
#include <vector>

#include <video/buffer.hpp>

namespace video {
    namespace gl330 {
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
    } // namespace gl330
} // namespace video

#include <glcore_330.h>
#include <core/journal.hpp>
#include <video/buffer.hpp>
#include <video/vertex_array.hpp>

namespace video {
    namespace gl330 {
        auto create_vertex_array() -> vertex_array {
            GLuint va;
            glGenVertexArrays(1, &va);

            game::journal::debug(game::journal::_VIDEO, "Create vertex array %", va);

            return {va, {}, {}, {}};
        }

        auto create_vertex_array(std::vector<vertex_attribute> &attribs, const binding_buffer &vb, const binding_buffer &eb) -> vertex_array {
            auto va = create_vertex_array();

            return va;
        }

        auto destroy_vertex_array(vertex_array &va) -> void {
            if (glIsVertexArray(va.id)) {
                game::journal::debug(game::journal::_VIDEO, "Destroy vertex array %", va.id);
                glDeleteVertexArrays(1, &va.id);
            }
            va.id = 0;
        }

        auto bind_vertex_array(const vertex_array &va) -> void {
            glBindVertexArray(va.id);
        }

        auto unbind_vertex_array(const vertex_array &va) -> void {
            UNUSED(va);

            glBindVertexArray(0);
        }

        auto vertex_array_buffer(vertex_array &va, buffer &buf, ptrdiff_t offset, ptrdiff_t stride) -> void {
            if (buf.target == GL_ARRAY_BUFFER) {
                va.vb.binding_index = 0;
                va.vb.buf = buf.id;
                va.vb.offset = offset;
                va.vb.stride = stride;
            }

            if (buf.target == GL_ELEMENT_ARRAY_BUFFER) {
                va.eb.binding_index = 0;
                va.eb.buf = buf.id;
                va.eb.offset = 0;
                va.eb.stride = 0;
            }
        }

        inline auto get_attrib_type(attrib_type type) -> uint32_t {
            switch (type) {
            case attrib_type::byte_value:
                return GL_BYTE;
            case attrib_type::unsigned_byte_value:
                return GL_UNSIGNED_BYTE;
            case attrib_type::short_value:
                return GL_SHORT;
            case attrib_type::unsigned_short_value:
                return GL_UNSIGNED_SHORT;
            case attrib_type::int_value:
                return GL_INT;
            case attrib_type::unsigned_int_value:
                return GL_UNSIGNED_INT;

            case attrib_type::half_float_value:
                return GL_HALF_FLOAT;
            case attrib_type::float_value:
                return GL_FLOAT;
            case attrib_type::double_value:
                return GL_DOUBLE;
            case attrib_type::int_2_10_10_10_rev:
                return GL_INT_2_10_10_10_REV;
            case attrib_type::unsigned_int_2_10_10_10_rev:
                return GL_UNSIGNED_INT_2_10_10_10_REV;
            }

            return GL_NONE;
        }

        auto vertex_array_format(vertex_array &va, vertex_attributes attribindex, int32_t size, attrib_type type, bool normalized, uint32_t relative_offset) -> void {
            assert(size > 0 && size < 5);

            glVertexAttribPointer(static_cast<uint32_t>(attribindex), size, get_attrib_type(type), normalized, va.vb.stride, (const GLvoid*)(va.vb.offset + relative_offset));
            glEnableVertexAttribArray(static_cast<uint32_t>(attribindex));

            // TODO : add to attribs of va
        }
    } // namespace gl330
} // namespace video

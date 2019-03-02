#include <glcore_330.h>
#include <video/journal.hpp>
#include <video/video.hpp>
#include <video/command_buffer.hpp>
#include <video/stats.hpp>

namespace video {

    namespace gl330 {

        command_buffer::command_buffer(const size_t uniform_components) : memory_offset{0} {
            memory_size = uniform_components == 0 ? (sizeof (float) * 1024) : (sizeof (float) * uniform_components);
            raw_memory = malloc(memory_size);
            memset(raw_memory, 0, memory_size);
        }

        command_buffer::~command_buffer() {
            free(raw_memory);
            raw_memory = nullptr;
            memory_size = 0;
            memory_offset = 0;
        }

        inline auto get_uniform_type_size(const uint32_t type) -> size_t {
            switch (type) {
            case GL_INT:
                return sizeof(int);
            case GL_FLOAT:
                return sizeof(float);
            case GL_FLOAT_VEC2:
                return sizeof(float) * 2;
            case GL_FLOAT_VEC3:
                return sizeof(float) * 3;
            case GL_FLOAT_VEC4:
                return sizeof(float) * 4;
            case GL_FLOAT_MAT3:
                return sizeof(float) * 9;
            case GL_FLOAT_MAT4:
                return sizeof(float) * 16;
            }

            return 0;
        }

        inline auto dispath_uniform(command_buffer &buf, uint32_t offset, int32_t location, uint32_t type, uint32_t count) -> void {
            switch (type) {
            case GL_UNSIGNED_INT:
                glUniform1uiv(location, static_cast<GLsizei>(count), reinterpret_cast<const uint32_t*>(static_cast<const char*>(buf.raw_memory) + offset));
                break;
            case GL_FLOAT_MAT4:
                glUniformMatrix4fv(location, static_cast<GLsizei>(count), GL_FALSE, reinterpret_cast<const float*>(static_cast<const char*>(buf.raw_memory) + offset));
                break;
            case GL_FLOAT:
                glUniform1fv(location, static_cast<GLsizei>(count), reinterpret_cast<const float*>(static_cast<const char*>(buf.raw_memory) + offset));
                break;
            case GL_FLOAT_VEC2:
                glUniform2fv(location, static_cast<GLsizei>(count), reinterpret_cast<const float*>(static_cast<const char*>(buf.raw_memory) + offset));
                break;
            case GL_FLOAT_VEC3:
                glUniform3fv(location, static_cast<GLsizei>(count), reinterpret_cast<const float*>(static_cast<const char*>(buf.raw_memory) + offset));
                break;
            case GL_FLOAT_VEC4:
                glUniform4fv(location, static_cast<GLsizei>(count), reinterpret_cast<const float*>(static_cast<const char*>(buf.raw_memory) + offset));
                break;
            default:
                journal::warning("Unknown uniform %", type);
                break;
            }
        }

        template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
        template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

        auto dispath_command(const command &c, command_buffer &buf) -> void {
            std::visit([&] (auto&& arg) {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, detail::clear>) {
                    glClearColor(buf.clear_color.x, buf.clear_color.y, buf.clear_color.z, buf.clear_color.w);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                } else if constexpr (std::is_same_v<T, detail::viewport>) {
                    glViewport(arg.x, arg.y, arg.w, arg.h);
                } else if constexpr (std::is_same_v<T, detail::draw_arrays>) {
                    stats_inc_dips();
                    stats_add_tris(arg.count);
                    glDrawArrays(arg.mode, static_cast<GLint>(arg.first), static_cast<GLsizei>(arg.count));
                    // TODO: instanced
                    // glDrawArraysInstanced(arg.mode, arg.first, arg.count, arg.primcount);
                } else if constexpr (std::is_same_v<T, detail::draw_elements>) {
                    stats_inc_dips();
                    stats_add_tris(arg.count);
                    glDrawElementsBaseVertex(arg.mode, static_cast<GLsizei>(arg.count), GL_UNSIGNED_SHORT, nullptr, static_cast<GLint>(arg.base_vertex));

                    // TODO: instanced
                    // glDrawElementsInstancedBaseVertex(arg.mode, arg.count, GL_UNSIGNED_SHORT, nullptr, arg.primcount, arg.base_vertex);
                } else if constexpr (std::is_same_v<T, detail::bind_framebuffer>) {
                    glBindFramebuffer(GL_FRAMEBUFFER, arg.id);
                } else if constexpr (std::is_same_v<T, detail::bind_program>) {
                    stats_inc_prg_bindings();

                    glUseProgram(arg.id);
                } else if constexpr (std::is_same_v<T, detail::bind_texture>) {
                    stats_inc_tex_bindings();

                    glActiveTexture(GL_TEXTURE0 + arg.unit);
                    glBindTexture(arg.target, arg.texture);

                    glUniform1i(arg.location, static_cast<GLint>(arg.unit));
                } else if constexpr (std::is_same_v<T, detail::bind_sampler>) {
                    glBindSampler(arg.unit, arg.id);
                } else if constexpr (std::is_same_v<T, detail::bind_vertex_array>) {
                    glBindVertexArray(arg.id);
                } else if constexpr (std::is_same_v<T, detail::bind_uniform>) {
                    dispath_uniform(buf, arg.offset, arg.location, arg.type, arg.count);
                } else if constexpr (std::is_same_v<T, detail::update>) {
                    glBindBuffer(arg.target, arg.buf);
                    glBufferSubData(arg.target, arg.offset, static_cast<GLsizeiptr>(arg.size), arg.data);
                } else if constexpr (std::is_same_v<T, detail::blit>) {
                    glBindFramebuffer(GL_READ_FRAMEBUFFER, arg.src_id);
                    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, arg.dst_id);
                    glBlitFramebuffer(static_cast<GLint>(arg.src_x0), static_cast<GLint>(arg.src_y0),
                                      static_cast<GLint>(arg.src_x1), static_cast<GLint>(arg.src_y1),
                                      static_cast<GLint>(arg.dst_x0), static_cast<GLint>(arg.dst_y0),
                                      static_cast<GLint>(arg.dst_x1), static_cast<GLint>(arg.dst_y1),
                                      arg.mask, arg.filter);
                }
            }, c);
        }

        auto operator <<(command_buffer &cb, const detail::bind_uniform &c) -> command_buffer& {
            auto cc = c;
            if (cb.raw_memory) {
                size_t sz = get_uniform_type_size(c.type) * c.count;

                assert(cb.memory_offset + sz < cb.memory_size);

                memcpy(static_cast<char*>(cb.raw_memory) + cb.memory_offset, c.ptr, c.size);
                cc.offset = static_cast<uint32_t>(cb.memory_offset);
                cb.memory_offset += sz;
            }

            cb.commands.push_back(cc);
            return cb;
        }

    } // namespace gl330

} // namespace video

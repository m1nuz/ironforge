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

        auto get_uniform_type_size(const uint32_t type) -> size_t {
            size_t sz = 0;
            switch (type) {
            case GL_INT:
                sz = sizeof(int);
                break;
            case GL_FLOAT:
                sz = sizeof(float);
                break;
            case GL_FLOAT_VEC2:
                sz = sizeof(float) * 2;
                break;
            case GL_FLOAT_VEC3:
                sz = sizeof(float) * 3;
                break;
            case GL_FLOAT_VEC4:
                sz = sizeof(float) * 4;
                break;
            case GL_FLOAT_MAT3:
                sz = sizeof(float) * 9;
                break;
            case GL_FLOAT_MAT4:
                sz = sizeof(float) * 16;
                break;
            }

            return sz;
        }

        inline auto dispath_uniform(command_buffer &buf, uint32_t offset, int32_t location, uint32_t type, uint32_t count) -> void {
            switch (type) {
            case GL_UNSIGNED_INT:
                glUniform1uiv(location, count, (const uint32_t*)((const char*)buf.raw_memory + offset));
                break;
            case GL_FLOAT_MAT4:
                glUniformMatrix4fv(location, count, GL_FALSE, (const float*)((const char*)buf.raw_memory + offset));
                break;
            case GL_FLOAT:
                glUniform1fv(location, count, (const float*)((const char*)buf.raw_memory + offset));
                break;
            case GL_FLOAT_VEC2:
                glUniform2fv(location, count, (const float*)((const char*)buf.raw_memory + offset));
                break;
            case GL_FLOAT_VEC3:
                glUniform3fv(location, count, (const float*)((const char*)buf.raw_memory + offset));
                break;
            case GL_FLOAT_VEC4:
                glUniform4fv(location, count, (const float*)((const char*)buf.raw_memory + offset));
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
                    glDrawArrays(arg.mode, arg.first, arg.count);
                    // TODO: instanced
                    // glDrawArraysInstanced(arg.mode, arg.first, arg.count, arg.primcount);
                } else if constexpr (std::is_same_v<T, detail::draw_elements>) {
                    stats_inc_dips();
                    stats_add_tris(arg.count);
                    glDrawElementsBaseVertex(arg.mode, arg.count, GL_UNSIGNED_SHORT, nullptr, arg.base_vertex);

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

                    glUniform1i(arg.location, arg.unit);
                } else if constexpr (std::is_same_v<T, detail::bind_sampler>) {
                    glBindSampler(arg.unit, arg.id);
                } else if constexpr (std::is_same_v<T, detail::bind_vertex_array>) {
                    glBindVertexArray(arg.id);
                } else if constexpr (std::is_same_v<T, detail::bind_uniform>) {
                    dispath_uniform(buf, arg.offset, arg.location, arg.type, arg.count);
                } else if constexpr (std::is_same_v<T, detail::update>) {
                    glBindBuffer(arg.target, arg.buf);
                    glBufferSubData(arg.target, arg.offset, arg.size, arg.data);
                } else if constexpr (std::is_same_v<T, detail::blit>) {
                    glBindFramebuffer(GL_READ_FRAMEBUFFER, arg.src_id);
                    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, arg.dst_id);
                    glBlitFramebuffer(arg.src_x0, arg.src_y0, arg.src_x1, arg.src_y1,
                                      arg.dst_x0, arg.dst_y0, arg.dst_x1, arg.dst_y1,
                                      arg.mask, arg.filter);
                }
            }, c);
        }

    } // namespace gl330

} // namespace video

#pragma once

#include <cstring>
#include <vector>
#include <ironforge_common.hpp>
#include <video/command.hpp>
#include <video/state.hpp>

namespace video {
    namespace gl330 {
        struct command_buffer {
            command_buffer();
            command_buffer(size_t mem_size);
            ~command_buffer();

            command_buffer(command_buffer const&) = delete;
            command_buffer& operator=(command_buffer const&) = delete;

            glm::vec4 clear_color;

            color_blend_state   blend;
            rasterizer_state    rasterizer;
            depth_stencil_state depth;

            std::vector<command> commands;

            // for unifroms
            void    *raw_memory;
            size_t  memory_size;
            size_t  memory_offset;
        };

        auto get_uniform_type_size(uint32_t type) -> size_t;

        /*inline auto& operator <<(command_buffer &cb, const clear_op &c) {
            cb.commands.push_back(c);
            return cb;
        }

        inline auto& operator <<(command_buffer &cb, const draw_elements_op &c) {
            cb.commands.push_back(c);
            return cb;
        }*/

        template <typename T>
        inline auto& operator <<(command_buffer &cb, const T &c) {
            cb.commands.push_back(c);
            return cb;
        }

        command_buffer& operator <<(command_buffer &cb, const send_uniform &c);
        auto dispath_command(const command &c, command_buffer &buf) -> void;
    } // namespace gl330
} // namespace video

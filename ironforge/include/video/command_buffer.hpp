#pragma once

#include <vector>
#include <ironforge_common.hpp>
#include <video/command.hpp>

namespace video {
    namespace gl330 {
        struct command_buffer {
            glm::vec4 clear_color;

            std::vector<command> commands;
        };

        inline auto& operator <<(command_buffer &cb, const clear_op &c) {
            cb.commands.push_back(c);
            return cb;
        }

        auto dispath_command(const command &c, command_buffer &buf) -> void;
    } // namespace gl330
} // namespace video

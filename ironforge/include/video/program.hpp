#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <video/shader.hpp>

namespace video {
    namespace gl330 {
        struct uniform {
            std::string     name;
            uint64_t        name_hash;
            int32_t         location;
            int32_t         num;
            uint32_t        type;
        };

        struct attribute {
            std::string     name;
            uint64_t        name_hash;
            int32_t         location;
            int32_t         num;
            uint32_t        type;
        };

        struct program_info {
            std::string name;
            std::vector<shader_source>  sources;
        };

        struct program {
            uint32_t pid;
            std::vector<shader_source>  sources;
            std::vector<uniform>        uniforms;
            std::vector<attribute>      attributes;
        };

        auto create_program(const program_info &info) -> program;
        auto destroy_program(program &pro) -> void;
    } // namespace gl330
} // namespace video

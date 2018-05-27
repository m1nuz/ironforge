#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <video/shader.hpp>

namespace video {

    namespace gl330 {

        /*enum class uniform_type : uint32_t {

        };*/

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
            program() = default;

            uint32_t                    pid = 0;
            std::vector<shader_source>  sources;
            std::vector<uniform>        uniforms;
            std::vector<attribute>      attributes;
        };

        auto create_program(const program_info &info) -> program;
        auto destroy_program(program &pro) -> void;
        auto get_uniform_location(const program &pro, const std::string &name) -> int32_t;
    } // namespace gl330

} // namespace video

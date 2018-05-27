#pragma once

#include <cstdint>
#include <string>

namespace video {

    namespace gl330 {

        enum class shader_type : uint32_t {
            vertex,
            geometry,
            fragment
        };

        struct shader_source {
            std::string name;
            std::string text;
        };

        struct shader {
            uint32_t    id;
            shader_type type;
        };

        auto compile_shader(shader_type type, const shader_source &source) -> shader;

    } // namespace gl330

} // namespace video

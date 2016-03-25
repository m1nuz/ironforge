#include <glcore_330.h>
#include <core/application.hpp>
#include <video/command.hpp>

namespace video {
    namespace gl330 {

        send_uniform::send_uniform(int location, const glm::mat4 &m) : command{command_type::send_uniform} {
            _send_uniform.ptr = &m[0][0];
            _send_uniform.location = location;
            _send_uniform.offset = 0; // calculated later
            _send_uniform.size = sizeof m;
            _send_uniform.count = 1;
            _send_uniform.type = GL_FLOAT_MAT4;
        }

        send_uniform::send_uniform(int location, const glm::mat3 &m) {
            _send_uniform.ptr = &m[0][0];
            _send_uniform.location = location;
            _send_uniform.offset = 0; // calculated later
            _send_uniform.size = sizeof m;
            _send_uniform.count = 1;
            _send_uniform.type = GL_FLOAT_MAT3;
        }

        send_uniform::send_uniform(int location, float value) {
            _send_uniform.ptr = &value;
            _send_uniform.location = location;
            _send_uniform.offset = 0; // calculated later
            _send_uniform.size = sizeof value;
            _send_uniform.count = 1;
            _send_uniform.type = GL_FLOAT;
        }

        send_uniform::send_uniform(int location, const glm::vec3 &value) {
            _send_uniform.ptr = &value[0];
            _send_uniform.location = location;
            _send_uniform.offset = 0; // calculated later
            _send_uniform.size = sizeof value;
            _send_uniform.count = 1;
            _send_uniform.type = GL_FLOAT_VEC3;
        }

        send_uniform::send_uniform(int location, const glm::vec4 &value) {
            _send_uniform.ptr = &value[0];
            _send_uniform.location = location;
            _send_uniform.offset = 0; // calculated later
            _send_uniform.size = sizeof value;
            _send_uniform.count = 1;
            _send_uniform.type = GL_FLOAT_VEC4;
        }

    } // namespace gl330
} // namespace video

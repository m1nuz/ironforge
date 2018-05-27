#include <glcore_330.h>
#include <video/command.hpp>

namespace video {

    namespace gl330 {

        namespace detail {

            bind_uniform::bind_uniform(const program &p, const std::string &name, const glm::mat4 &m) {
                location = get_uniform_location(p, name);
                type = GL_FLOAT_MAT4;
                size = sizeof m;
                offset = 0; // calculated later
                count = 1;
                ptr = &m[0][0];
            }

            bind_uniform::bind_uniform(const program &p, const std::string &name, const glm::mat3 &m) {
                location = get_uniform_location(p, name);
                type = GL_FLOAT_MAT3;
                offset = 0; // calculated later
                size = sizeof m;
                count = 1;
                ptr = &m[0][0];
            }

            bind_uniform::bind_uniform(const program &p, const std::string &name, const float &value) {
                location = get_uniform_location(p, name);
                type = GL_FLOAT;
                size = sizeof value;
                offset = 0; // calculated later
                count = 1;
                ptr = &value;
            }

            bind_uniform::bind_uniform(const program &p, const std::string &name, const glm::vec2 &value) {
                location = get_uniform_location(p, name);
                type = GL_FLOAT_VEC2;
                offset = 0; // calculated later
                size = sizeof value;
                count = 1;
                ptr = &value[0];
            }

            bind_uniform::bind_uniform(const program &p, const std::string &name, const glm::vec3 &value) {
                location = get_uniform_location(p, name);
                type = GL_FLOAT_VEC3;
                offset = 0; // calculated later
                size = sizeof value;
                count = 1;
                ptr = &value[0];
            }

            bind_uniform::bind_uniform(const program &p, const std::string &name, const glm::vec4 &value) {
                location = get_uniform_location(p, name);
                type = GL_FLOAT_VEC4;
                offset = 0; // calculated later
                size = sizeof value;
                count = 1;
                ptr = &value[0];
            }

            bind_uniform::bind_uniform(const program &p, const std::string &name, const int &value) {
                location = get_uniform_location(p, name);
                type = GL_INT;
                offset = 0; // calculated later
                size = sizeof value;
                count = 1;
                ptr = &value;
            }

            blit::blit(const framebuffer &src, const framebuffer &dst) {
                src_id = src.id;
                src_x0 = 0;
                src_y0 = 0;
                src_x1 = src.width;
                src_y1 = src.height;

                dst_id = dst.id;
                dst_x0 = 0;
                dst_y0 = 0;
                dst_x1 = dst.width;
                dst_y1 = dst.height;

                mask = GL_COLOR_BUFFER_BIT;
                filter = GL_LINEAR;
            }

        } // namespace detail

    } // namespace gl330

} // namespace video

#include <vector>

#include <glcore_330.h>
#include <video/journal.hpp>
#include <video/video.hpp>
#include <video/shader.hpp>

namespace video {

    namespace gl330 {

        auto get_shader_type(shader_type type) -> uint32_t {
            switch (type) {
            case shader_type::vertex:
                return GL_VERTEX_SHADER;
            case shader_type::geometry:
                return GL_GEOMETRY_SHADER;
            case shader_type::fragment:
                return GL_FRAGMENT_SHADER;
            }

            return GL_NONE;
        }

        auto compile_shader(shader_type type, const shader_source &source) -> shader {
            auto sh = shader{glCreateShader(get_shader_type(type)), type};

            const char *fullsource[] =  {source.text.c_str(), nullptr};

            glShaderSource(sh.id, 1, fullsource, nullptr);
            glCompileShader(sh.id);

            GLint status = 0;
            glGetShaderiv(sh.id, GL_COMPILE_STATUS, &status);

            if (!status) {
                GLint lenght = 0;
                glGetShaderiv(sh.id, GL_INFO_LOG_LENGTH, &lenght);
                if (lenght > 0) {
                    std::string log_text(static_cast<size_t>(lenght), 0);

                    GLsizei written = 0;
                    glGetShaderInfoLog(sh.id, lenght, &written, &log_text[0]);
                    log_text.resize(static_cast<size_t>(lenght));

                    journal::error("%", log_text);
                } else {
                    journal::error("%", "Unknown log lenght");
                }

                glDeleteShader(sh.id);
                sh.id = 0;
            }

            return sh;
        }

    } // namespace gl330

} // namespace video

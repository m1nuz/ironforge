#include <algorithm>

#include <glcore_330.h>
#include <ironforge_utility.hpp>
#include <core/application.hpp>
#include <video/video.hpp>
#include <video/program.hpp>

namespace video {
    namespace gl330 {
        static auto link_program(uint32_t pid) -> bool {
            glLinkProgram(pid);

            GLint status = 0;
            glGetProgramiv(pid, GL_LINK_STATUS, &status);

            if (!status && !video::is_debugging()) {
                GLint lenght = 0;
                glGetProgramiv(pid, GL_INFO_LOG_LENGTH, &lenght);

                std::string log_text(lenght, 0);

                GLsizei written = 0;
                glGetProgramInfoLog(pid, lenght, &written, &log_text[0]);
                log_text.resize(written);

                application::error(application::log_category::video, "%\n", log_text);
            }

            return true;
        }

        static auto get_program_attributes(program &p) -> int32_t {
            int total = -1;
            glGetProgramiv(p.pid, GL_ACTIVE_ATTRIBUTES, &total);

            if (total < 0)
                return -1;

            p.attributes.reserve(total);

            // TODO: GL_ACTIVE_ATTRIBUTE_MAX_LENGTH
            char name[1024] = {};
            int name_len = -1, num = -1;
            GLenum type = GL_ZERO;
            for (auto i = 0; i < total; i++) {
                glGetActiveAttrib(p.pid, i, sizeof(name) - 1, &name_len, &num, &type, name);

                p.attributes.push_back({name, utils::xxhash64(name, name_len), glGetAttribLocation(p.pid, name), num, type});

                application::debug(application::log_category::video, "-a- % %\n", p.attributes.back().name, p.attributes.back().location);
            }

            return static_cast<int32_t>(p.attributes.size());
        }

        static auto get_program_uniforms(program &p) -> int32_t {
            int total = -1;
            glGetProgramiv(p.pid, GL_ACTIVE_UNIFORMS, &total);

            if (total < 0)
                return -1;

            p.uniforms.reserve(total);

            // TODO: GL_ACTIVE_UNIFORM_MAX_LENGTH to get allocation size
            char name[1024] = {};
            int name_len = -1, num = -1;
            GLenum type = GL_ZERO;
            for (auto i = 0; i < total; i++) {
                glGetActiveUniform(p.pid, i, sizeof(name) - 1, &name_len, &num, &type, name);

                p.uniforms.push_back({name, utils::xxhash64(name, name_len), glGetUniformLocation(p.pid, name), num, type});

                application::debug(application::log_category::video, "-u- % %\n", p.uniforms.back().name, p.uniforms.back().location);
            }

            return static_cast<int32_t>(p.uniforms.size());
        }

        auto create_program(const program_info &info) -> program {
            auto pid = glCreateProgram();

            application::debug(application::log_category::video, "Create program %\n", pid);

            std::vector<shader> shaders;
            shaders.reserve(5);

            for (const auto &s : info.sources) {
                auto ext = s.name.substr(s.name.find_last_of('.') + 1);

                if (ext == "vert")
                    shaders.push_back(compile_shader(shader_type::vertex, s));

                if (ext == "frag")
                    shaders.push_back(compile_shader(shader_type::fragment, s));

                if (ext == "geom")
                    shaders.push_back(compile_shader(shader_type::geometry, s));
            }

            for (const auto &s : shaders)
                glAttachShader(pid, s.id);

            if (!link_program(pid))
                return {0};

            for (const auto &s : shaders) {
                glDetachShader(pid, s.id);
                glDeleteShader(s.id);
            }
            program p;
            p.pid = pid;

            get_program_uniforms(p);
            get_program_attributes(p);

            return p;
        }

        auto destroy_program(program &pro) -> void {
            if (!glIsProgram(pro.pid))
                application::debug(application::log_category::video, "Trying delete not program %\n", pro.pid);

            application::debug(application::log_category::video, "Destroy program %\n", pro.pid);

            glDeleteProgram(pro.pid);
            pro.pid = 0;
        }

        auto get_uniform_location(program &pro, const std::string &name) -> int32_t {
            auto hash = utils::xxhash64(name);

            auto it = std::find_if(pro.uniforms.begin(), pro.uniforms.end(), [&](const uniform &u) {
                return u.name_hash == hash;
            });

            if (it != pro.uniforms.end())
                return it->location;

            application::warning(application::log_category::video, "Uniform not found %\n", name);

            return -1;
        }
    } // namespace gl330
} // namespace video

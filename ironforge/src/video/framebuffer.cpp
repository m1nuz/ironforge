#include <glcore_330.h>
#include <ironforge_utility.hpp>
#include <core/application.hpp>
#include <video/screen.hpp>
#include <video/framebuffer.hpp>

namespace video {
    namespace gl330 {
        inline auto check_framebuffer() -> uint32_t {
            const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

            if (status != GL_FRAMEBUFFER_COMPLETE)
                application::error(application::log_category::video, "Framebuffer incomplete %\n", utils::to_hex(status));

            // TODO: make verbose output

            return status;
        }

        inline auto get_framebuffer_attachment(framebuffer_attachment attachment) {
            switch (attachment) {
            case framebuffer_attachment::color_attachment0:
                return GL_COLOR_ATTACHMENT0;
            case framebuffer_attachment::color_attachment1:
                return GL_COLOR_ATTACHMENT1;
            case framebuffer_attachment::color_attachment2:
                return GL_COLOR_ATTACHMENT2;
            case framebuffer_attachment::color_attachment3:
                return GL_COLOR_ATTACHMENT3;
            case framebuffer_attachment::color_attachment4:
                return GL_COLOR_ATTACHMENT4;
            case framebuffer_attachment::color_attachment5:
                return GL_COLOR_ATTACHMENT5;
            case framebuffer_attachment::color_attachment6:
                return GL_COLOR_ATTACHMENT6;
            case framebuffer_attachment::color_attachment7:
                return GL_COLOR_ATTACHMENT7;
            case framebuffer_attachment::depth_attachment:
                return GL_DEPTH_ATTACHMENT;
            case framebuffer_attachment::stencil_attachment:
                return GL_STENCIL_ATTACHMENT;
            case framebuffer_attachment::depth_stencil_attachment:
                return GL_DEPTH_STENCIL_ATTACHMENT;
            }

            return GL_NONE;
        }

        auto create_framebuffer(const framebuffer_info &info) -> framebuffer {
            GLuint buf = 0;
            glGenFramebuffers(1, &buf);
            glBindFramebuffer(GL_FRAMEBUFFER, buf);

            application::debug(application::log_category::video, "Create framebuffer %\n", buf);

            for (const auto &att : info.attachments) {
                switch (att.attachment_target) {
                case framebuffer_attachment_target::texture:
                    glFramebufferTexture(GL_FRAMEBUFFER, get_framebuffer_attachment(att.attachment), att.texture, 0);
                    break;
                case framebuffer_attachment_target::renderbuffer:
                    glFramebufferRenderbuffer(GL_FRAMEBUFFER, get_framebuffer_attachment(att.attachment), GL_RENDERBUFFER, att.texture);
                    break;
                }
            }

            check_framebuffer();

            return {buf, info.width, info.height, info.mask};
        }

        auto destroy_framebuffer(framebuffer &buf) -> void {
            if (!glIsFramebuffer(buf.id))
                application::debug(application::log_category::video, "Trying delete not framebuffer %\n", buf.id);

            application::debug(application::log_category::video, "Destroy framebuffer %\n", buf.id);

            glDeleteFramebuffers(1, &buf.id);
            buf.id = 0;
        }

        auto default_framebuffer() -> framebuffer {
            return {0, static_cast<uint32_t>(screen.width), static_cast<uint32_t>(screen.height), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT};
        }
    } // namespace gl330
} // namespace video

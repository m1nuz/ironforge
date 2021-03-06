#include <glcore_330.h>
#include <video/journal.hpp>
#include <video/instance.hpp>
#include <video/framebuffer.hpp>

namespace video {

    namespace gl330 {

        inline auto check_framebuffer() -> uint32_t {
            const GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

            if (status != GL_FRAMEBUFFER_COMPLETE)
                journal::error("Framebuffer incomplete %", status);

            // TODO: make verbose output

            return status;
        }

        inline auto get_framebuffer_attachment(const framebuffer_attachment attachment) -> GLenum {
            switch (attachment) {
            case framebuffer_attachment::color0:
                return GL_COLOR_ATTACHMENT0;
            case framebuffer_attachment::color1:
                return GL_COLOR_ATTACHMENT1;
            case framebuffer_attachment::color2:
                return GL_COLOR_ATTACHMENT2;
            case framebuffer_attachment::color3:
                return GL_COLOR_ATTACHMENT3;
            case framebuffer_attachment::color4:
                return GL_COLOR_ATTACHMENT4;
            case framebuffer_attachment::color5:
                return GL_COLOR_ATTACHMENT5;
            case framebuffer_attachment::color6:
                return GL_COLOR_ATTACHMENT6;
            case framebuffer_attachment::color7:
                return GL_COLOR_ATTACHMENT7;
            case framebuffer_attachment::depth:
                return GL_DEPTH_ATTACHMENT;
            case framebuffer_attachment::stencil:
                return GL_STENCIL_ATTACHMENT;
            case framebuffer_attachment::depth_stencil:
                return GL_DEPTH_STENCIL_ATTACHMENT;
            }

            return GL_NONE;
        }

        auto create_framebuffer(const framebuffer_info &info) -> framebuffer {
            GLuint buf = INVALID_FRAMEBUFFER_ID;
            glGenFramebuffers(1, &buf);
            glBindFramebuffer(GL_FRAMEBUFFER, buf);

            journal::debug("Create framebuffer %", buf);

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
                journal::debug("Trying delete not framebuffer %", buf.id);

            journal::debug("Destroy framebuffer %", buf.id);

            glDeleteFramebuffers(1, &buf.id);
            buf.id = 0;
        }

        auto default_framebuffer(instance_t &vi) -> framebuffer {
            return {DEFAULT_FRAMEBUFFER_ID, static_cast<uint32_t>(vi.w), static_cast<uint32_t>(vi.h), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT};
        }

    } // namespace gl330

} // namespace video

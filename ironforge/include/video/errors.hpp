#pragma once

#include <system_error>

namespace video {

    namespace errc {

        enum video_errors {
            init_sdl,
            init_opengl,
        };

        ///
        /// \brief Video errors category
        ///
        class video_category : public std::error_category {
        public:
            video_category() = default;

            std::string message(int c) const override {
                switch (static_cast<video_errors>(c)) {
                case video_errors::init_sdl:
                    return "Init SDL";
                case video_errors::init_opengl:
                    return "Init OpenGL";
                }

                return "Unknown video error";
            }

            const char* name() const noexcept override {
                return "Video";
            }

            const static std::error_category& get() {
                const static video_category category_const;
                return category_const;
            }
        };

        inline std::error_code make_error_code(video_errors e) {
            return std::error_code(static_cast<int>(e), video_category::get());
        }

    } // namespace errc

} // namespace video

namespace std {
    template <>
    struct is_error_code_enum<video::errc::video_errors> : std::true_type {
    };
} // namespace std

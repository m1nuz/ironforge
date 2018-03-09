#pragma once

#include <system_error>

namespace scene {

    namespace errc {

        enum scene_errors {
            load_scene,
        };

        ///
        /// \brief Scene errors category
        ///
        class scene_category : public std::error_category {
        public:
            scene_category() = default;

            std::string message(int c) const override {
                switch (static_cast<scene_errors>(c)) {
                case scene_errors::load_scene:
                    return "Load scene failed";
                }

                return "Unknown scene error";
            }

            const char* name() const noexcept override {
                return "Scene";
            }

            const static std::error_category& get() {
                const static scene_category category_const;
                return category_const;
            }
        };

        inline std::error_code make_error_code(scene_errors e) {
            return std::error_code(static_cast<int>(e), scene_category::get());
        }

    } // namespace errc

} // namespace scene

namespace std {
    template <>
    struct is_error_code_enum<scene::errc::scene_errors> : std::true_type {
    };
} // namespace std

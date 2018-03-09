#pragma once

#include <system_error>

namespace game {

    namespace errc {

        enum game_errors {
            init_platform,
            init_assets,
            read_assets,
            init_video,
            read_scenes,
            no_start_scene,
            init_gamecontrollers
        };

        ///
        /// \brief Game errors category
        ///
        class game_category : public std::error_category {
        public:
            game_category() = default;

            std::string message(int c) const override;

            const char* name() const noexcept override;

            const static std::error_category& get() {
                const static game_category category_const;
                return category_const;
            }
        };

        inline std::error_code make_error_code(game_errors e) noexcept {
            return std::error_code(static_cast<int>(e), game_category::get());
        }

    } // namespace errc

} // namespace game

namespace std {
    template <>
    struct is_error_code_enum<game::errc::game_errors> : std::true_type {
    };
} // namespace std

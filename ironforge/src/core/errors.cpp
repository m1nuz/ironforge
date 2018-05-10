#include <core/errors.hpp>

namespace game {

    namespace errc {

        std::string game_category::message(int c) const {
            switch (static_cast<game_errors>(c)) {
            case game_errors::init_conf:
                return "Configuration file not found";
            case game_errors::init_platform:
                return "SDL initialization failed";
            case game_errors::init_assets:
                return "Assets initialization failed";
            case game_errors::read_assets:
                return "Read assets failed";
            case game_errors::init_video:
                return "Init video failed";
            case game_errors::read_scenes:
                return "Init scenes";
            case game_errors::no_start_scene:
                return "No start scene";
            case game_errors::init_gamecontrollers:
                return "Init gamecontrollerdb";
            }

            return "Unknown game error";
        }

        const char* game_category::name() const noexcept {
            return "Game";
        }

    } // namespace errc

} // namespace game

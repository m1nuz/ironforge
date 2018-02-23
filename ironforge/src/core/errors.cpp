#include <core/errors.hpp>

namespace game {

    namespace errc {

        std::string game_category::message(int c) const {
            switch (static_cast<game_errors>(c)) {
            case game_errors::init_platform:
                return "SDL initialization failed";
            case game_errors::init_assets:
                return "Assets initialization failed";
            case game_errors::read_assets:
                return "Read asset failed";
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

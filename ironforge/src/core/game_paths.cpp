#include <core/game.hpp>
#include <SDL2/SDL_filesystem.h>

namespace game {

    namespace detail {

        auto get_base_path() noexcept -> const std::string& {
            static std::string base_path;

            if (!base_path.empty())
                return base_path;

            auto path = SDL_GetBasePath();
            base_path = path;
            SDL_free(path);

            return base_path;
        }

        auto get_pref_path() noexcept -> const std::string& {
            static std::string pref_path;

            if (!pref_path.empty())
                return pref_path;

            auto path = SDL_GetBasePath();
            pref_path = path;
            SDL_free(path);

            return pref_path;
        }

    } // namespace detail

} // namespace game

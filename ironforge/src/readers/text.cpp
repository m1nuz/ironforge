#include <core/journal.hpp>
#include <core/game.hpp>
#include <readers/text.hpp>

auto read_text(assets::instance_t &inst, SDL_RWops *rw) -> std::optional<assets::text_data_t> {
    (void)inst;

    if (!rw)
        return {};

    const auto lenght = SDL_RWsize(rw);

    assets::text_data_t text;

    if (lenght > 0) {
        text.resize(static_cast<size_t>(lenght));

        SDL_RWread(rw, &text[0], text.size(), 1);
    }

    SDL_RWclose(rw);

    return text;
}

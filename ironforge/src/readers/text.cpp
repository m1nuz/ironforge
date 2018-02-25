#include <cassert>
#include <readers/text.hpp>
#include <core/journal.hpp>
#include <core/game.hpp>

auto read_text(assets::instance_t &inst, SDL_RWops *rw) -> std::optional<assets::text_data_t> {
    assert(rw != nullptr);

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

#ifndef ABS
#define ABS(v) ((v) < 0 ? -(v) : (v))
#endif

extern char *
str_replace_once_fast(const char *orig, size_t orig_size, const char *rep, size_t rep_size, char *str, size_t str_size) {
    assert(orig != NULL);
    assert(rep != NULL);
    assert(str != NULL);

    const size_t sz = ABS((int)orig_size - (int)rep_size);
    const size_t nbytes = str_size + sz + 1;

    char *p = (char*)memmem(str, str_size, orig, orig_size);
    if (!p) {
        return strdup(str);
    }

    char *buffer = (char*)malloc(nbytes);
    buffer[nbytes - 1] = 0;

    memcpy(buffer, str, p - str);
    memcpy(buffer + (p - str), rep, rep_size);
    const size_t n = str_size - (p - str) - orig_size;
    memcpy(buffer + (p - str) + rep_size, p + orig_size, n);

    return buffer;
}

void replace_all(std::string& source, const std::string &find, const std::string &replace)
{
    using namespace std;

    for(string::size_type i = 0; (i = source.find(find, i)) != string::npos;)
    {
        source.replace(i, find.length(), replace);
        i += replace.length();
    }
}


// TODO: rewrite this with c++
auto read_shader_text(assets::instance_t &inst, SDL_RWops *rw) -> std::optional<assets::text_data_t> {
    using namespace game;

    const char include_directive[] = "#include";
    const size_t step = sizeof(include_directive) - 1;

    auto text = read_text(inst, rw).value_or(std::string{});

    char *p = &text[0];

    while (p < &text[0] + text.size()) {
        char *sp;
        if ((sp = strstr(p, include_directive)) != nullptr) {
            //printf("%s\n", text.text);
            //printf("----END\n");

            size_t pos = p - &text[0];
            char *end = strchr(sp, '\n');

            if ((size_t)(end - sp) == step) {
                journal::error(journal::_INPUT, "%", "#include expect \"FILENAME\"");
                quit();
            }

            char *bracers_first = strchr(sp, '\"');
            if (bracers_first == NULL) {
                journal::error(journal::_INPUT, "%", "#include expect \"FILENAME\"");
                quit();
            }

            char *space = sp + sizeof(include_directive);
            while (space < bracers_first)
                if (*space == ' ')
                    space++;
                else {
                    journal::error(journal::_INPUT, "%", "#include expect \"FILENAME\"");
                    quit();
                }

            char *bracers_second = strchr(bracers_first + 1, '\"');
            if (bracers_second == NULL) {
                journal::error(journal::_INPUT, "%", "#include expect \"FILENAME\"");
                quit();
            }

            size_t name_size = bracers_second - bracers_first - 1;
            if (name_size == 0) {
                journal::error(journal::_INPUT, "%", "empty filename in #include");
                quit();
            }

            char name[name_size];
            memcpy(name, bracers_first + 1, name_size);
            name[name_size] = 0;

            auto include_text = assets::get_text(inst, name).value_or(std::string{});

            //memset(sp, ' ', end - sp);

            char origin[end - sp];
            memcpy(origin, sp, sizeof(origin));
            origin[end - sp] = 0;

            replace_all(text, origin, include_text);

            /*char *old_text = text.text;
            text.text = str_replace_once_fast(origin, strlen(origin), include_text.text, include_text.size, text.text, text.size);
            text.size = strlen(text.text);
            p = text.text + pos;
            free(old_text);

            // TODO: memory...
            delete[] include_text.text;*/
        }

        p += step;
    }

    //printf("%s\n", text.text);

    return text;
}

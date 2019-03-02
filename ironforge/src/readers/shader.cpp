#include <core/journal.hpp>
#include <readers/text.hpp>

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

            //size_t pos = p - &text[0];
            char *end = strchr(sp, '\n');

            if ((size_t)(end - sp) == step) {
                journal::error(journal::_INPUT, "%", "#include expect \"FILENAME\"");
                return {};
            }

            char *bracers_first = strchr(sp, '\"');
            if (bracers_first == NULL) {
                journal::error(journal::_INPUT, "%", "#include expect \"FILENAME\"");
                return {};
            }

            char *space = sp + sizeof(include_directive);
            while (space < bracers_first)
                if (*space == ' ')
                    space++;
                else {
                    journal::error(journal::_INPUT, "%", "#include expect \"FILENAME\"");
                    return {};
                }

            char *bracers_second = strchr(bracers_first + 1, '\"');
            if (bracers_second == NULL) {
                journal::error(journal::_INPUT, "%", "#include expect \"FILENAME\"");
                return {};
            }

            size_t name_size = bracers_second - bracers_first - 1;
            if (name_size == 0) {
                journal::error(journal::_INPUT, "%", "empty filename in #include");
                return {};
            }

            std::string name{bracers_first + 1, name_size};

            auto include_text = assets::get_text(inst, name).value_or(std::string{});

            std::string origin{sp, static_cast<size_t>(end - sp)};

            replace_all(text, origin, include_text);
        }

        p += step;
    }

    return text;
}

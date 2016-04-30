#include <unordered_map>
#include <sstream>
#include <fstream>

#include <core/settings.hpp>

namespace application {
    struct _settings {
        std::unordered_map<std::string, std::string>    text_values;
        std::unordered_map<std::string, int>            int_values;
        std::unordered_map<std::string, double>         real_values;
        std::unordered_map<std::string, bool>           bool_values;
    };

    static _settings settings;

    inline auto is_int(const std::string &s) -> bool {
        return s.find_first_not_of("0123456789") == std::string::npos;
    }

    inline auto is_real(const std::string &s) -> bool {
        return s.find_first_not_of("0123456789.") == std::string::npos;
    }

    inline auto is_bool(const std::string &s) -> bool {
        if (s == "true" || s == "false")
            return true;

        return false;
    }

    inline auto to_int(const std::string &s) -> int {
        std::istringstream iss(s);
        int value = 0;
        iss >> std::noskipws >> value;

        return value;
    }

    inline auto to_real(const std::string &s) -> double {
        std::istringstream iss(s);
        double value = 0.f;
        iss >> std::noskipws >> value;

        return value;
    }

    inline auto to_bool(const std::string &s) -> bool {
        /*std::istringstream iss(s);
        bool value = true;
        iss >> std::noskipws >> value;

        return value;*/

        if (s == "true")
            return true;

        return false;
    }

    auto int_value(const std::string &key, int default_value) -> int32_t {
        auto it = settings.int_values.find(key);

        if (it != settings.int_values.end())
            return it->second;

        return default_value;
    }

    auto real_value(const std::string &key, double default_value) -> double {
        auto it = settings.real_values.find(key);

        if (it != settings.real_values.end())
            return it->second;

        return default_value;
    }

    auto bool_value(const std::string &key, bool default_value) -> bool {
        auto it = settings.bool_values.find(key);

        if (it != settings.bool_values.end())
            return it->second;

        return default_value;
    }

    auto text_value(const std::string &key, const std::string &default_value) -> const std::string& {
        auto it = settings.text_values.find(key);

        if (it != settings.text_values.end())
            return it->second;

        return default_value;
    }


    auto append_settings(const std::string &name) -> bool {
        std::ifstream file{name};

        if (!file)
            return false;

        std::stringstream stream;
        stream << file.rdbuf();

        file.close();

        std::string line;
        while (std::getline(stream, line)) {
            if (line[0] == '#' || line[0] == ';')
                continue;

            std::istringstream is_line(line);
            std::string key;

            if (std::getline(is_line, key, ' ')) {
                std::string value;

                if(std::getline(is_line, value)) {
                    if (is_int(value)) {
                        settings.int_values.emplace(key, to_int(value));
                        continue;
                    }

                    if (is_real(value)) {
                        settings.real_values.emplace(key, to_real(value));
                        continue;
                    }

                    if (is_bool(value)) {
                        settings.bool_values.emplace(key, to_bool(value));
                        continue;
                    }

                    settings.text_values.emplace(key, value);
                }
            }
        }

        return true;
    }
} // namespace application

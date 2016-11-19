#include <vector>
#include <thread>
#include <mutex>

#include <core/journal.hpp>
#include <core/game.hpp>

namespace game {
    namespace journal {
        static std::vector<std::function<int (const category, const priority, const void*, size_t)>> storages;

        auto add_storage(std::function<int (const category, const priority, const void*, size_t)> cb) -> void {
            storages.push_back(cb);
        }

        auto write(const category c, const priority p, const std::string &message) -> void {
            std::mutex out_mutex;
            std::lock_guard<std::mutex> guard(out_mutex);

            for (auto &store : storages)
                store(c, p, message.c_str(), message.size());
        }

        static const char *category_names[] = {
            "Game",
            "System",
            "Audio",
            "Video",
            "Render",
            "Input",
            "Scene",
            "Ui"
        };

        static const char *priority_names[] = {
            "VERBOSE",
            "DEBUG",
            "INFO",
            "WARNING",
            "ERROR",
            "CRITICAL"
        };

        auto console_storage(const category c, const priority p, const void *d, size_t s) -> int {
            using game::journal::priority;

            // CSI colors
            // https://en.wikipedia.org/wiki/ANSI_escape_code
            switch (p) {
            case priority::verbose:
            case priority::debug:
                fputs("\x1b[32m", stdout);
            case priority::info:
                break;
            case priority::warning:
                fputs("\x1b[33;1m", stdout);
                break;
            case priority::error:
                fputs("\x1b[31;1m", stdout);
                break;
            case priority::critical:
                fputs("\x1b[39;41;1m", stdout);
                break;
            default:
                break;
            }


            fprintf(stdout, "%s (%s): ", priority_names[static_cast<int>(p)], category_names[static_cast<int>(c)]);
            fwrite(d, s, 1, stdout);
            fputs("\x1b[0m", stdout);

            return 0;
        }

        static struct _log_file {
            _log_file() {
                auto path = game::get_pref_path() + "journal.log";

                fp = fopen(path.c_str(), "w");
            }

            ~_log_file() {
                if (fp)
                    fclose(fp);
            }

            FILE *fp = nullptr;
        } log_file;

        auto file_storage(const category c, const priority p, const void *d, size_t s) -> int {
            using game::journal::priority;

            if (!log_file.fp)
                return -1;

            auto t = std::time(NULL);
            char timestamp_str[100];
            std::strftime(timestamp_str, sizeof(timestamp_str), "%F %T", std::localtime(&t));

            fprintf(log_file.fp, "%s | %s (%s): ", timestamp_str, priority_names[static_cast<int>(p)], category_names[static_cast<int>(c)]);
            fwrite(d, s, 1, log_file.fp);
            //fputs("\n", log_file.fp);

            return 0;
        }
    } // namespace journal
} // namespace game

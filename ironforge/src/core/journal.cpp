#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>

#include <core/journal.hpp>
#include <core/game.hpp>

namespace game {
    namespace journal {
        typedef std::vector<std::function<int (const std::string &, const verbosity, const void *, size_t)>> storages_t;

        static std::unordered_map<std::string, std::pair<verbosity, storages_t>> tags;

        auto add_tag(const std::string &tag, const verbosity verbosity_level) -> void {
            tags.emplace(tag, std::make_pair(verbosity_level, storages_t{}));
        }

        auto add_storage(std::function<int (const std::string &, const verbosity, const void *, size_t)> cb) -> void {
            for (auto& tag : tags)
                tag.second.second.push_back(cb);
        }
        auto add_storage(const std::string &tag, std::function<int (const std::string &, const verbosity, const void *, size_t)> cb) -> void {
            tags[tag].second.push_back(cb);
        }

        auto write(const std::string &tag, const verbosity v, const std::string &message) -> void {
            std::mutex out_mutex;
            std::lock_guard<std::mutex> guard(out_mutex);

            for (auto &store : tags[tag].second)
                store(tag, v, message.c_str(), message.size());
        }

        static const char *priority_names[] = {
            "VERBOSE",
            "DEBUG",
            "INFO",
            "WARNING",
            "ERROR",
            "CRITICAL"
        };

        auto setup_default(const std::string &log_path) -> void {
            add_tag(_GAME, verbosity::verbose);
            add_tag(_SYSTEM, verbosity::verbose);
            add_tag(_AUDIO, verbosity::verbose);
            add_tag(_VIDEO, verbosity::verbose);
            add_tag(_RENDER, verbosity::verbose);
            add_tag(_INPUT, verbosity::verbose);
            add_tag(_SCENE, verbosity::verbose);
            add_tag(_UI, verbosity::verbose);

            add_storage(console_storage);

            using namespace std::placeholders;
            add_storage(std::bind(journal::single_file_storage, log_path, _1, _2, _3, _4));
        }

        auto console_storage(const std::string &tag, const verbosity v, const void *d, size_t s) -> int {
            using game::journal::verbosity;

            // CSI colors
            // https://en.wikipedia.org/wiki/ANSI_escape_code
            switch (v) {
            case verbosity::verbose:
            case verbosity::debug:
                fputs("\x1b[32m", stdout);
            case verbosity::info:
                break;
            case verbosity::warning:
                fputs("\x1b[33;1m", stdout);
                break;
            case verbosity::error:
                fputs("\x1b[31;1m", stdout);
                break;
            case verbosity::critical:
                fputs("\x1b[39;41;1m", stdout);
                break;
            default:
                break;
            }

            fprintf(stdout, "%s (%s): ", priority_names[static_cast<int>(v)], tag.c_str());
            fwrite(d, s, 1, stdout);
            fputs("\x1b[0m\n", stdout);

            return 0;
        }

        struct _log_file {
            _log_file() : fp(nullptr) {

            }

            _log_file(const std::string &path) {
                if (!fp)
                    fp = fopen(path.c_str(), "w");
            }

            ~_log_file() {
                if (fp)
                    fclose(fp);
            }

            FILE *fp;
        };

        auto single_file_storage(const std::string &path, const std::string &tag, const verbosity v, const void *d, size_t s) -> int {
            using game::journal::verbosity;

            static _log_file log_file(path);
            if (!log_file.fp)
                return -1;

            auto t = std::time(NULL);
            char timestamp_str[100];
            std::strftime(timestamp_str, sizeof(timestamp_str), "%F %T", std::localtime(&t));

            fprintf(log_file.fp, "%s | %s (%s): ", timestamp_str, priority_names[static_cast<int>(v)], tag.c_str());
            fwrite(d, s, 1, log_file.fp);
            //fputs("\n", log_file.fp);

            return 0;
        }

        auto multi_file_storage(const std::unordered_map<std::string, std::string> &files, const std::string &tag, const verbosity v, const void *d, size_t s) -> int {
            static std::unordered_map<std::string, _log_file> tagged_files;
            if (tagged_files.empty())
                for (const auto &f : files)
                    tagged_files.emplace(f.first, f.second);

            auto fi = tagged_files.find(tag);
            if (fi == tagged_files.end())
                return -1;

            auto fp = fi->second.fp;
            if (!fp)
                return -1;

            auto t = std::time(NULL);
            char timestamp_str[100];
            std::strftime(timestamp_str, sizeof(timestamp_str), "%F %T", std::localtime(&t));

            fprintf(fp, "%s | %s (%s): ", timestamp_str, priority_names[static_cast<int>(v)], tag.c_str());
            fwrite(d, s, 1, fp);

            return 0;
        }
    } // namespace journal
} // namespace game

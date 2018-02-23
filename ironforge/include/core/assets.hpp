#pragma once

#include <cstdint>
#include <string>
#include <variant>
#include <unordered_map>
#include <vector>
#include <functional>
#include <tuple>
#include <system_error>
#include <fstream>
#include <thread>
#include <atomic>
#include <mutex>

#include <SDL2/SDL_rwops.h>
#include <core/common.hpp>
#include <video/video.hpp>

// TODO: handle memory alloc/free

namespace assets {
    using asset_result = std::variant<std::string, std::error_code>;

    enum class result : int32_t {
        success,
        failure
    };

    typedef struct text_type {
        std::string data;
    } text_t;

    typedef struct binary_type {
        std::vector<uint8_t> data;
    } binary_t;

    struct binary_data {
        void        *raw_memory;
        size_t      size;
    };

    struct text_data {
        char        *text;
        size_t      size;
    };

    struct instance_type;
    typedef instance_type instance_t;

    using image_data = video::image_data;

    using binary_data_t = std::vector<uint8_t>;
    using text_data_t = std::string;
    using image_data_t = video::image_data;

    using binary_reader_t = std::function<std::optional<binary_data_t> (instance_t &inst, SDL_RWops *rw)>;
    using text_reader_t = std::function<std::optional<text_data_t> (instance_t &inst, SDL_RWops *rw)>;
    using image_reader_t = std::function<std::optional<image_data_t> (instance_t &inst, SDL_RWops *rw)>;

    ///
    /// \brief The readers struct
    ///
    struct readers final {
        std::unordered_map<std::string, std::function<int32_t (SDL_RWops *rw, binary_data &)>> binary_readers;
        std::unordered_map<std::string, std::function<int32_t (SDL_RWops *rw, text_data &)>>   text_readers;
        std::unordered_map<std::string, std::function<int32_t (SDL_RWops *rw, image_data &)>>  image_readers;
    };

    using text_responce = std::function<void (const std::optional<text_t> res)>;
    struct text_info {
        text_info() = default;
        utility::copyable_atomic<bool> ready = false;
        std::vector<text_responce> responces;
    };

    ///
    /// \brief The instance_type struct
    ///
    typedef struct instance_type final {

        instance_type() = default;

        using binary_reader = std::function<int32_t (SDL_RWops *rw, binary_data &)>;
        using text_reader = std::function<int32_t (SDL_RWops *rw, text_data &)>;
        using image_reader = std::function<int32_t (SDL_RWops *rw, image_data &)>;

        std::unordered_map<std::string, binary_reader>  binary_readers;
        std::unordered_map<std::string, text_reader>    text_readers;
        std::unordered_map<std::string, image_reader>   image_readers;

        std::unordered_map<std::string, binary_data>    binaries;
        std::unordered_map<std::string, text_data>      texts;
        std::unordered_map<std::string, image_data>     images;

        std::unordered_map<std::string, text_info>      text_processed;

        std::unordered_map<std::string, text_t>         all_texts;
        std::unordered_map<std::string, std::string>    all_files;

        utility::copyable_atomic<bool> active = true;

    //private:
        //instance_type(const instance_type&) = delete;
        //instance_type& operator=(const instance_type&) = delete;
    } instance_t;

    using instance_result = std::variant<instance_t, std::error_code>;

    ///
    /// \brief Create default readers
    /// \return readers
    ///
    [[nodiscard]] auto create_default_readers() -> readers;

    auto process_load(instance_t &inst) -> void;

    ///
    /// \brief Create asset instance
    /// \return asset instance
    ///
    [[nodiscard]] auto create_instance(const readers &rs) -> instance_result;

    [[nodiscard]] auto append(instance_t &inst, const std::string &ext, std::function<int32_t (SDL_RWops *rw, binary_data &)> reader) -> bool;
    [[nodiscard]] auto append(instance_t &inst, const std::string &ext, std::function<int32_t (SDL_RWops *rw, text_data &)> reader) -> bool;
    [[nodiscard]] auto append(instance_t &inst, const std::string &ext, std::function<int32_t (SDL_RWops *rw, image_data &)> reader) -> bool;

    ///
    /// \brief Open asset
    /// \param inst asset instance
    /// \param path Path to asset
    /// \return
    /// Add all readable files to asset instance
    ///
    [[nodiscard]] auto open(instance_t &inst, const std::string& path) -> result;

    auto process(instance_t &inst) -> void;
    auto cleanup(instance_t &inst) -> void;
    auto get_text(const std::string& name) -> text_data;
    auto get_text_absolute(const std::string& path) -> text_data;
    auto get_image(const std::string& name) -> image_data;
    auto get_binary(const std::string& name) -> binary_data;


    auto get_text(instance_t &inst, std::string_view name, text_responce cb) -> void;

    inline bool is_ok(const asset_result &res) {
        return std::holds_alternative<std::string>(res);
    }

    inline auto readfile(const std::string &path) -> asset_result {
        using namespace std;

        ifstream fs(path, ios::in | ios::binary);

        if (!fs.is_open())
            return make_error_code(errc::bad_file_descriptor);

        string contents;
        fs.seekg(0, ios::end);
        contents.resize(fs.tellg());
        fs.seekg(0, ios::beg);
        fs.read(&contents[0], contents.size());
        fs.close();

        return contents;
    }
} // namespace assets

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
    using instance_result = std::variant<instance_t, std::error_code>;

    struct instance_type;
    typedef instance_type instance_t;

    using binary_data_t = std::vector<uint8_t>;
    using text_data_t = std::string;
    using image_data_t = video::image_data;

    using binary_reader_t = std::function<std::optional<binary_data_t> (instance_t &inst, SDL_RWops *rw)>;
    using text_reader_t = std::function<std::optional<text_data_t> (instance_t &inst, SDL_RWops *rw)>;
    using image_reader_t = std::function<std::optional<image_data_t> (instance_t &inst, SDL_RWops *rw)>;

    using binary_responce = std::function<void (const std::optional<binary_data_t> res)>;
    using text_responce = std::function<void (const std::optional<text_data_t> res)>;
    using image_responce = std::function<void (const std::optional<image_data_t> res)>;

    ///
    /// \brief The readers struct
    ///
    struct readers final {
        std::unordered_map<std::string, binary_reader_t> binary_readers;
        std::unordered_map<std::string, text_reader_t>   text_readers;
        std::unordered_map<std::string, image_reader_t>  image_readers;
    };

    struct text_info {
        text_info() = default;
        utility::copyable_atomic<bool> ready = false;
        std::vector<text_responce> responces;
    };

    struct binary_info {
        binary_info() = default;
        utility::copyable_atomic<bool> ready = false;
        std::vector<binary_responce> responces;
    };

    struct image_info {
        image_info() = default;
        utility::copyable_atomic<bool> ready = false;
        std::vector<image_responce> responces;
    };

    ///
    /// \brief The instance_type struct
    ///
    typedef struct instance_type final {

        instance_type() = default;

        std::unordered_map<std::string, binary_reader_t>    binary_readers;
        std::unordered_map<std::string, text_reader_t>      text_readers;
        std::unordered_map<std::string, image_reader_t>     image_readers;

        std::unordered_map<std::string, binary_data_t>      binaries;
        std::unordered_map<std::string, text_data_t>        texts;
        std::unordered_map<std::string, image_data_t>       images;

        std::unordered_map<std::string, text_info>          text_processed;

        std::unordered_map<std::string, std::string>        all_files;

        utility::copyable_atomic<bool>                      active = true;

    //private:
        //instance_type(const instance_type&) = delete;
        //instance_type& operator=(const instance_type&) = delete;
    } instance_t;

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

    [[nodiscard]] auto append(instance_t &inst, const std::string &ext, binary_reader_t reader) -> bool;
    [[nodiscard]] auto append(instance_t &inst, const std::string &ext, text_reader_t reader) -> bool;
    [[nodiscard]] auto append(instance_t &inst, const std::string &ext, image_reader_t reader) -> bool;

    ///
    /// \brief Open asset
    /// \param inst asset instance
    /// \param path Path to asset
    /// \return
    /// Add all readable files to asset instance
    ///
    [[nodiscard]] auto open(instance_t &inst, const std::string& path) -> bool;

    auto process(instance_t &inst) -> void;
    auto cleanup(instance_t &inst) -> void;

    [[nodiscard]] auto get_config(std::string_view path) -> std::optional<std::string>;
    [[nodiscard]] auto get_text(instance_t &inst, std::string_view name) -> std::optional<text_data_t>;
    [[nodiscard]] auto get_image(instance_t &inst, std::string_view name) -> std::optional<image_data_t>;
    [[nodiscard]] auto get_binary(instance_t &inst, std::string_view name) -> std::optional<binary_data_t>;

    auto get_text(instance_t &inst, std::string_view name, text_responce cb) -> void;
    auto get_image(instance_t &inst, std::string_view name, image_responce cb) -> void;
    auto get_binary(instance_t &inst, std::string_view name, binary_responce cb) -> void;
} // namespace assets

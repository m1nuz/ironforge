#pragma once

#include <cstdint>
#include <cstddef>

namespace video {

    struct frame_info {
        frame_info() = default;

        uint64_t start_time = 0;
        uint64_t end_time = 0;
        double   time_value = 0.0;
        double   accumulator = 0.0;
        int      counter = 0;
        int      framerate = 0;
        int      max_framerate = 0;
        int      min_framerate = 0;
        char     info[80] = {};
        size_t   info_size = 0;
    };

    namespace stats {

        auto reset(frame_info& stats) -> void;
        auto begin(frame_info& stats) -> void;
        auto end(frame_info& stats) -> void;

    } // namespace stats

} // namespace video

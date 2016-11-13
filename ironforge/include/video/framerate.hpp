#pragma once

#include <cstdint>
#include <cstddef>

namespace video {
    struct frame_stats {
        uint64_t start_time;
        uint64_t end_time;
        double   time_value;
        double   accumulator;
        int      counter;
        int      framerate;
        int      max_framerate;
        int      min_framerate;
        char     info[80];
        size_t   info_size;
    };

    auto reset(frame_stats& stats) -> void;
    auto begin(frame_stats& stats) -> void;
    auto end(frame_stats& stats) -> void;
} // namespace video
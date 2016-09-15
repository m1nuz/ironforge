#include <SDL2/SDL_timer.h>
#include <video/framerate.hpp>

namespace video {
    auto reset(frame_stats& stats) -> void {
        stats.accumulator = 0;
        stats.framerate = 0;
        stats.max_framerate = 0;
        stats.min_framerate = 0;
        stats.time_value = 0;
        stats.counter = 0;
        stats.info_size = 0;
    }

    auto begin(frame_stats& stats) -> void {
        stats.start_time = SDL_GetPerformanceCounter();
    }

    auto end(frame_stats& stats) -> void {
        stats.end_time = SDL_GetPerformanceCounter();
        stats.time_value = (double)(stats.end_time - stats.start_time) / SDL_GetPerformanceFrequency();
        stats.accumulator += stats.time_value;
        stats.counter++;

        if (stats.accumulator > 1.0) {
            stats.framerate = round((double)stats.counter / stats.accumulator);

            if (stats.min_framerate == 0)
                stats.min_framerate = stats.framerate;

            stats.max_framerate = (stats.framerate > stats.max_framerate) ? stats.framerate : stats.max_framerate;
            stats.min_framerate = (stats.framerate < stats.min_framerate) ? stats.framerate : stats.min_framerate;

            stats.accumulator = 0;
            stats.counter = 0;

            stats.info_size = snprintf(stats.info, sizeof(stats.info), "fps %d\nmin %d\nmax %d\navg %d\ntm %lfms",
                                       stats.framerate, stats.min_framerate, stats.max_framerate,
                                       (stats.min_framerate + stats.max_framerate) / 2, stats.time_value * 1000.0);
        }
    }
} // namespace video

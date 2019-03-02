#include <SDL2/SDL_timer.h>
#include <video/framerate.hpp>

namespace video {

    namespace stats {

        auto reset(frame_info& stats) -> void {
            stats.accumulator = 0;
            stats.framerate = 0;
            stats.max_framerate = 0;
            stats.min_framerate = 0;
            stats.time_value = 0;
            stats.counter = 0;
            stats.info_size = 0;
        }

        auto begin(frame_info& stats) -> void {
            stats.start_time = SDL_GetPerformanceCounter();
        }

        auto end(frame_info& stats) -> void {
            stats.end_time = SDL_GetPerformanceCounter();
            stats.time_value = static_cast<double>(stats.end_time - stats.start_time) / SDL_GetPerformanceFrequency();
            stats.accumulator += stats.time_value;
            stats.counter++;

            if (stats.accumulator > 1.0) {
                stats.framerate = static_cast<int>(round(static_cast<double>(stats.counter) / stats.accumulator));

                if (stats.min_framerate == 0)
                    stats.min_framerate = stats.framerate;

                stats.max_framerate = (stats.framerate > stats.max_framerate) ? stats.framerate : stats.max_framerate;
                stats.min_framerate = (stats.framerate < stats.min_framerate) ? stats.framerate : stats.min_framerate;

                stats.accumulator = 0;
                stats.counter = 0;

                auto n_chars = snprintf(stats.info, sizeof stats.info, "fps %d\nmin %d\nmax %d\navg %d\nframe time %lfms",
                                        stats.framerate, stats.min_framerate, stats.max_framerate,
                                        (stats.min_framerate + stats.max_framerate) / 2, stats.time_value * 1000.0);

                if (n_chars > 0)
                    stats.info_size = static_cast<size_t>(n_chars);
            }
        }

    } // namespace stats

} // namespace video

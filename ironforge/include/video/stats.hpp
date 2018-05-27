#pragma once

#include <cstdint>
#include <cstddef>

namespace video {

    struct drawing_info {
        uint32_t    dips;
        uint32_t    tris;
        uint32_t    tex_bindings;
        uint32_t    prg_bindings;
        float       tv;
        char        info[100];
        size_t      info_size;
    };

    namespace stats {
        auto reset(drawing_info &stats) -> void;
        auto update(drawing_info &stats, const float dt) -> void;
    } // namespace stats

    extern drawing_info video_stats;

    inline void stats_add_tris(uint32_t n) {
        video_stats.tris += n;
    }

    inline void stats_add_dips(uint32_t n) {
        video_stats.dips += n;
    }

    inline void stats_inc_dips() {
        video_stats.dips++;
    }

    inline void stats_inc_tex_bindings() {
        video_stats.tex_bindings++;
    }

    inline void stats_inc_prg_bindings() {
        video_stats.prg_bindings++;
    }

    inline void stats_update(const float dt) {
        stats::update(video_stats, dt);
    }

    inline void stats_clear() {
        stats::reset(video_stats);
    }

} // namespace video

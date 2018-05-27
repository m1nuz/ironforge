#include <cstdio>
#include <cstring>
#include <video/stats.hpp>

namespace video {
    drawing_info video_stats;

    namespace stats {
        auto reset(drawing_info &stats) -> void {
            stats.dips = 0;
            stats.tris = 0;
            stats.tex_bindings = 0;
            stats.prg_bindings = 0;
        }

        auto update(drawing_info &stats, const float dt) -> void {
            stats.tv += dt;

            if (stats.tv > 0.5f) {
                // per time info
                // ...

                stats.info_size = snprintf(stats.info, sizeof stats.info, "DIPs/frame %d\nTriangles %d\nTex bindings %d\nPrg bindings %d",
                                           stats.dips, stats.tris, stats.tex_bindings, stats.prg_bindings);

                stats.tv = 0.f;
            }

            // per frame info
        }

    } // namespace stats

} // namespace video

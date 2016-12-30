#include <vector>
#include <algorithm>

#include <core/journal.hpp>
#include <scene/scene.hpp>

#include "timer.hpp"

namespace scene {
    std::vector<timer_instance> timers;

    static auto default_on_time(timer_instance &t) -> void {
        UNUSED(t);

        t.end = std::chrono::system_clock::now();
        auto elapsed_seconds = std::chrono::duration<double> {t.end - t.start};
        t.start = t.end;

        game::journal::debug(game::journal::_SCENE, "% % %s\n", t.id, "on_time", elapsed_seconds.count());
    }

    auto init_all_timers() -> void {
        timers.reserve(max_timers);
        // NOTE: not best idea
        memset(&timers[0], 0, max_timers * sizeof (timer_instance));

        timer_info ti;
        ti.type = timer_type::once;
        ti.time_interval = 1.0f;
        ti.callback = nullptr;

        create_timer(ti);
    }

    auto cleanup_all_timers() -> void {
        timers.clear();
    }

    auto update_all_timers(const float dt) -> void {
        for (auto &t : timers) {
            t.value += dt;

            if (t.value > t.stop_value) {
                switch (t.type) {
                case timer_type::once:
                    t.callback(t);
                    t.value = 0;
                    t.status = timer_status::dead;
                    break;
                case timer_type::periodic:
                    t.callback(t);
                    t.value = 0;
                    break;
                }
            }
        }

        for (size_t i = 0; i < max_timers; i++)
            if (timers[i].status == timer_status::dead) {
                auto t = &timers[i];
                delete_timer(t);
            }
    }

    auto create_timer(const timer_info &info) -> timer_instance* {
        static int32_t timer_id = 1;
        timer_instance t;
        t.id = timer_id++;
        t.type = info.type;
        t.status = timer_status::alive;
        t.value = 0.f;
        // TODO: can't be smaller then timestep
        t.stop_value = info.time_interval;
        t.callback = info.callback ? info.callback : default_on_time;
        t.start = std::chrono::system_clock::now();

        timers.push_back(t);

        game::journal::debug(game::journal::_SCENE, "Create timer %\n", t.id);

        return &timers.back();
    }

    auto delete_timer(timer_instance *&t) -> void {
        assert(t != NULL);

        /*size_t i = 0;
        for (; i < timers.size(); i++)
            if (t->id == timers[i].id)
                break;*/

        auto it = std::find_if(timers.begin(), timers.end(), [t](const timer_instance &tr) {
            return tr.id == t->id;
        });

        if (it == timers.end())
            return;

        game::journal::debug(game::journal::_SCENE, "Destroy timer %\n", t->id);

        if (timers.size() > 1)
            *it = timers.back();

        timers.pop_back();
        t = nullptr;
    }
} // namespace scene

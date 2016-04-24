#pragma once

#include <functional>
#include <chrono>
#include <cstdint>

namespace scene {
    enum class timer_type : uint32_t {
        once,
        periodic
    };

    enum class timer_status : uint32_t {
        dead,
        alive
    };

    struct timer_instance;

    struct timer_info {
        timer_type type;
        float time_interval;
        std::function<void(timer_instance &)> callback;
    };

    struct timer_instance {
        std::chrono::time_point<std::chrono::system_clock> start, end;

        int32_t         id;
        timer_type      type;
        timer_status    status;
        float           value;
        float           stop_value;
        std::function<void(timer_instance &)> callback;
    };

    auto init_all_timers() -> void;
    auto cleanup_all_timers() -> void;
    auto update_all_timers(const float dt) -> void;

    // NOTE: if timer_type == once, return pointer will be invalid after auto remove
    auto create_timer(const timer_info &info) -> timer_instance*;
    auto delete_timer(timer_instance *&t) -> void;
} // namespace scene

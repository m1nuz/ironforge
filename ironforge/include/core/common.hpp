#pragma once

#ifdef __GNUC__
#define restrict __restrict
#else
#define restrict
#endif // __GNUC__

#define UNUSED(x) (void)(x)

#include <atomic>

namespace utility {
    template <typename T>
    struct atomic_wrapper
    {
        std::atomic<T> _a;

        atomic_wrapper() : _a{} {
        }

        atomic_wrapper(const std::atomic<T> &a) : _a{a.load()} {
        }

        atomic_wrapper(const atomic_wrapper &other) : _a{other._a.load()} {
        }

        atomic_wrapper &operator=(const atomic_wrapper &other) {
            _a.store(other._a.load());
        }

        atomic_wrapper &operator=(const T &v) {
            _a.store(v);
        }
    };

    template<class T>
    class copyable_atomic : public std::atomic<T>
    {
    public:
        copyable_atomic() = default;

        constexpr copyable_atomic(T desired) : std::atomic<T>(desired) {
        }

        constexpr copyable_atomic(const copyable_atomic<T>& other) : copyable_atomic(other.load(std::memory_order_relaxed)) {
        }

        copyable_atomic& operator=(const copyable_atomic<T>& other) {
            this->store(other.load(std::memory_order_acquire), std::memory_order_release);
            return *this;
        }
    };
}

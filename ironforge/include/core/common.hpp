#pragma once

#ifdef __GNUC__
#define restrict __restrict
#else
#define restrict
#endif // __GNUC__

#define UNUSED(x) (void)(x)
#define randf() (rand() / (float)RAND_MAX)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/quaternion.hpp>
#pragma GCC diagnostic pop

#include <json.hpp>

namespace glm {
    using json = nlohmann::json;

    inline void to_json(json& j, const vec3& p) {
        j.array({p.x, p.y, p.z});
    }

    inline void from_json(const json& j, vec3& p) {
        int ix = 0;
        for (auto it = j.begin(); it != j.end(); ++it, ix++) {
            p[ix] = *it;
        }
    }
} // namespace glm

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

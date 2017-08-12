#pragma once

#ifdef __GNUC__

#define __must_ckeck __attribute__ ((warn_unused_result))
#define restrict __restrict

#endif // __GNUC__

#define UNUSED(x) (void)(x)

#include <memory>

/*template<typename T, typename ...Args>
inline std::unique_ptr<T> make_unique(Args&& ...args) {
    return std::unique_ptr<T>(new T( std::forward<Args>(args)... ));
}*/

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/quaternion.hpp>
#pragma GCC diagnostic pop

#define randf() (rand() / (float)RAND_MAX)

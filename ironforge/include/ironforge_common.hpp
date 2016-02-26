#pragma once

#ifdef __GNUC__

#define __must_ckeck __attribute__ ((warn_unused_result))

#endif // __GNUC__

#include <memory>

template<typename T, typename ...Args>
inline std::unique_ptr<T> make_unique(Args&& ...args) {
    return std::unique_ptr<T>(new T( std::forward<Args>(args)... ));
}


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#include <glm/glm.hpp>
#pragma GCC diagnostic pop

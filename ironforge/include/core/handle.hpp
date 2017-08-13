#pragma once

#include <cstdint>

namespace iron {
    template <typename T>
    struct handle {
        int32_t _id     = -1;
        T       *_ptr   = nullptr;

        auto id() -> int32_t {
            if (!_ptr)
                return -1;

            return _id == _ptr->id ? _id : -1;
        }
    };
} // namespace iron

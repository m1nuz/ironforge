#pragma once

#include <deque>

namespace utility
{
    template<typename T, typename Container = std::deque<T>>
    class iterable_stack
            : public std::stack<T, Container>
    {
        using std::stack<T, Container>::c;

    public:

        // expose just the iterators of the underlying container
        auto begin() { return std::begin(c); }
        auto end() { return std::end(c); }

        auto begin() const { return std::begin(c); }
        auto end() const { return std::end(c); }
    };
} // namespace utility

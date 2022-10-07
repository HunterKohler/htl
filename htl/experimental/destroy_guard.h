#ifndef HTL_EXPERIMENTAL_DESTROY_GUARD_H_
#define HTL_EXPERIMENTAL_DESTROY_GUARD_H_

#include <iterator>

namespace htl {

template <std::input_iterator I>
    requires std::destructible<std::iter_value_t<I>> &&
        std::is_lvalue_reference_v<std::iter_reference_t<I>>
class DestroyGuard {
public:
    explicit DestroyGuard(const I &start) noexcept
        : _start(start), _current(std::addressof(start))
    {}

    DestroyGuard(I &&) = delete;

    ~DestroyGuard()
    {
        if (_current) {
            std::ranges::destroy(std::move(_start), *_current);
        }
    }

    void release() noexcept
    {
        _current = nullptr;
    }

    void reset(const I &start)
    {
        _start = start;
        _current = std::addressof(start);
    }

private:
    I _start;
    const I *_current;
};

template <std::input_iterator I>
    requires std::destructible<std::iter_value_t<I>> &&
        std::is_lvalue_reference_v<std::iter_reference_t<I>> &&
        std::is_trivially_destructible_v<std::iter_value_t<I>>
class DestroyGuard<I> {
public:
    DestroyGuard(const I &) noexcept {}

    DestroyGuard(I &&) = delete;

    void reset(const I &) noexcept {}

    void release() noexcept {}
};

} // namespace htl

#endif

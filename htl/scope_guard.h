#ifndef HLIB_SCOPE_GUARD_H_
#define HLIB_SCOPE_GUARD_H_

#include <cerrno>
#include <concepts>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

namespace hlib {

template <class F>
    requires(std::is_invocable_v<F> &&std::is_same_v<std::decay_t<F>, F>)
class ScopeGuard {
public:
    constexpr ScopeGuard() //
        noexcept(std::is_nothrow_default_constructible_v<F>) //
        requires(std::is_default_constructible_v<F> && !std::is_function_v<F>)
        : _active(true), _f()
    {}

    template <class U = F>
        requires(std::is_constructible_v<F, U>)
    constexpr explicit ScopeGuard(U &&u) //
        noexcept(std::is_nothrow_constructible_v<F, U>) //
        : _active(true), _f(std::forward<U>(u))
    {}

    template <class U>
        requires(std::is_nothrow_constructible_v<F, U>)
    constexpr explicit(std::is_convertible_v<U, F>)
        ScopeGuard(ScopeGuard<U> &&other) noexcept
        : _active(std::exchange(other._active, false)),
          _f(std::move(other._active))
    {}

    template <class... Args>
        requires(std::is_constructible_v<F, Args...>)
    constexpr explicit ScopeGuard(std::in_place_t, Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<F, Args...>)
        : _active(true), _f(std::forward<Args>(args)...)
    {}

    template <class U, class... Args>
        requires(
            std::is_constructible_v<F, std::initializer_list<U> &, Args...>)
    constexpr ScopeGuard(
        std::in_place_t, std::initializer_list<U> ilist, Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<
                 F, std::initializer_list<U> &, Args...>)
        : _active(true), _f(ilist, std::forward<Args>(args)...)
    {}

    ScopeGuard(const ScopeGuard &) = delete;
    ScopeGuard &operator=(const ScopeGuard &) = delete;

    constexpr ScopeGuard(ScopeGuard &&other) noexcept
        requires(std::is_nothrow_move_constructible_v<F>)
        : _active(std::exchange(other._active, false)), _f(std::move(other._f))
    {}

    constexpr ScopeGuard &operator=(ScopeGuard &&other) noexcept
        requires(std::is_nothrow_move_assignable_v<F>)
    {
        _active = std::exchange(other._active, false);
        _f = std::move(other._active);
        return *this;
    }

    constexpr ~ScopeGuard() noexcept(std::is_nothrow_invocable_v<F>)
    {
        if (_active) {
            std::invoke(_f);
        }
    }

    constexpr void dismiss() noexcept
    {
        _active = false;
    }

    template <class... Args>
    void *operator new(std::size_t, Args &&...args) = delete;

    template <class... Args>
    void operator delete(void *, Args &&...args) = delete;

    friend constexpr bool operator==(
        const ScopeGuard &a, const ScopeGuard &b) noexcept
    {
        return std::addressof(a) == std::addressof(b);
    }

private:
    bool _active;
    [[no_unique_address]] F _f;
};

template <class F>
ScopeGuard(F) -> ScopeGuard<F>;

class ErrnoScopeGuard {
public:
    ErrnoScopeGuard() noexcept : _active(true), _value(std::exchange(errno, 0))
    {}

    ErrnoScopeGuard(ErrnoScopeGuard &&other) noexcept
        : _active(std::exchange(other._active, false)), _value(other._value)
    {}

    ErrnoScopeGuard &operator=(ErrnoScopeGuard &&other) noexcept
    {
        _active = std::exchange(other._active, false);
        _value = other._value;
        return *this;
    }

    ~ErrnoScopeGuard() noexcept
    {
        if (_active) {
            errno = _value;
        }
    }

    void dismiss() noexcept
    {
        _active = false;
    }

    int value() const noexcept
    {
        return _value;
    }

    friend void swap(ErrnoScopeGuard &a, ErrnoScopeGuard &b) noexcept
    {
        std::ranges::swap(a._active, b._active);
        std::ranges::swap(a._value, b._value);
    }

private:
    bool _active;
    int _value;
};

} // namespace hlib

#endif

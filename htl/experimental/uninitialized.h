#ifndef HTL_EXPERIMENTAL_UNINITIALIZED_H_
#define HTL_EXPERIMENTAL_UNINITIALIZED_H_

#include <memory>
#include <ranges>
#include <utility>

template <class T>
class Uninitialized {
public:
    using value_type = T;

    constexpr Uninitialized() noexcept {}

    template <class... Args>
        requires std::constructible_from<T, Args...>
    constexpr Uninitialized(std::in_place_t, Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : _value(std::forward<Args>(args)...)
    {}

    template <class U, class... Args>
        requires std::constructible_from<T, Args...>
    constexpr Uninitialized(
        std::in_place_t, std::initializer_list<U> ilist, Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<
                 T, std::initializer_list<U> &, Args...>)
        : _value(ilist, std::forward<Args>(args)...)
    {}

    constexpr ~Uninitialized() requires(std::is_trivially_destructible_v<T>) =
        default;

    constexpr ~Uninitialized() {}

    constexpr T *operator->() noexcept
    {
        return std::addressof(_value);
    }

    constexpr const T *operator->() const noexcept
    {
        return std::addressof(_value);
    }

    constexpr T &operator*() &noexcept
    {
        return _value;
    }

    constexpr T &&operator*() &&noexcept
    {
        return std::move(_value);
    }

    constexpr const T &operator*() const &noexcept
    {
        return _value;
    }

    constexpr const T &&operator*() const &&noexcept
    {
        return std::move(_value);
    }

    constexpr T &value() &noexcept
    {
        return _value;
    }

    constexpr T &&value() &&noexcept
    {
        return std::move(_value);
    }

    constexpr const T &value() const &noexcept
    {
        return _value;
    }

    constexpr const T &&value() const &&noexcept
    {
        return std::move(_value);
    }

    template <class... Args>
        requires(std::constructible_from<T, Args...>)
    constexpr T &construct(Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<T, Args...>)
    {
        std::ranges::construct_at(
            std::addressof(_value), std::forward<Args>(args)...);
        return _value;
    }

    template <class U, class... Args>
        requires(
            std::constructible_from<T, std::initializer_list<U> &, Args...>)
    constexpr T &construct(std::initialize_list<U> ilist, Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<
                 T, std::initializer_list<U> &, Args...>)
    {
        std::ranges::construct_at(
            std::addressof(_value), std::forward<Args>(args)...);
        return _value;
    }

    constexpr void destroy() noexcept(std::is_nothrow_destructible_v<T>)
    {
        if constexpr (!std::is_trivially_destructible_v<T>) {
            std::ranges::destroy_at(std::addressof(_value));
        }
    }

private:
    union {
        [[no_unique_address]] T _value;
    };
};

#endif

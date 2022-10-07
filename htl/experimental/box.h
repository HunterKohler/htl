#ifndef HLIB_EXPERIMENTAL_BOX_H_
#define HLIB_EXPERIMENTAL_BOX_H_

#include <optional>
#include <type_traits>
#include <hlib/compare.h>
#include <hlib/concepts.h>

namespace hlib {

template <class>
class Box;

namespace detail {

template <class T, class U>
inline constexpr bool converts_from_box =
    std::is_constructible_v<T, Box<U> &> ||
    std::is_constructible_v<T, const Box<U> &> ||
    std::is_constructible_v<T, Box<U> &&> ||
    std::is_constructible_v<T, const Box<U> &&> ||
    std::is_convertible_v<Box<U> &, T> ||
    std::is_convertible_v<const Box<U> &, T> ||
    std::is_convertible_v<Box<U> &&, T> ||
    std::is_convertible_v<const Box<U> &&, T>;

template <class T, class U>
inline constexpr bool assigns_from_box =
    std::is_assignable_v<T &, Box<U> &> ||
    std::is_assignable_v<T &, const Box<U> &> ||
    std::is_assignable_v<T &, Box<U> &&> ||
    std::is_assignable_v<T &, const Box<U> &&>;

} // namespace detail

template <class T>
    requires(!std::is_void_v<T> && !std::is_reference_v<T>)
class Box<T> {
public:
    using value_type = std::remove_cvref_t<T>;

    Box() = default;

    template <class U>
        requires(std::is_constructible_v<T, const U &> &&
                 !detail::converts_from_box<T, U>)
    constexpr explicit(!std::is_convertible_v<const U &, T>)
        Box(const Box<U> &other) //
        noexcept(std::is_nothrow_constructible_v<T, const U &>)
        : Box(std::in_place, other.value())
    {}

    template <class U>
        requires(std::is_constructible_v<T, U> &&
                 !detail::converts_from_box<T, U>)
    constexpr explicit(!std::is_convertible_v<U, T>) Box(Box<U> &&other) //
        noexcept(std::is_nothrow_constructible_v<T, U>)
        : Box(std::in_place, std::move(other.value()))
    {}

    template <class U>
        requires(std::is_constructible_v<T, U> &&
                 !std::same_as<std::remove_cvref_t<U>, std::in_place_t> &&
                 !std::same_as<std::remove_cvref_t<U>, Box>)
    constexpr explicit(!std::is_convertible_v<U, T>) Box(U &&value) //
        noexcept(std::is_nothrow_constructible_v<T, U>)
        : Box(std::in_place, std::forward<U>(value))
    {}

    template <class... Args>
        requires(std::is_constructible_v<T, Args...>)
    constexpr explicit Box(std::in_place_t, Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : _value(std::forward<Args>(args)...)
    {}

    template <class... Args>
        requires(
            std::is_constructible_v<T, std::initializer_list<U> &, Args...>)
    constexpr explicit Box(
        std::in_place_t, std::initializer_list<U> ilist, Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<
                 T, std::initializer_list<U> &, Args...>)
        : _value(ilist, std::forward<Args>(args)...)
    {}

    template <class U>
        requires(std::is_assignable_v<T &, U> &&
                 !std::same_as<std::remove_cvref<U>, Box> &&
                 (!std::is_scalar_t<T> ||
                  !std::same_as<std::decay_t<U>, std::decay_t<T>>))
    constexpr Box &operator=(U &&value) //
        noexcept(std::is_nothrow_assignable_v<T &, U>)
    {
        _value = std::forward<U>(value);
        return *this;
    }

    template <class U>
        requires(std::is_assignable_v<T &, const U &> &&
                 !detail::assigns_from_box<T, U>)
    constexpr Box &operator=(const Box<U> &other) //
        noexcept(std::is_nothrow_assignable_v<T &, U>)
    {
        _value = other.value();
        return *this;
    }

    template <class U>
        requires(std::is_assignable_v<T &, U> &&
                 !detail::assigns_from_box<T, U>)
    constexpr Box &operator=(Box<U> &&other) //
        noexcept(std::is_nothrow_assignable_v<T &, U>)
    {
        _value = std::move(other.value());
        return *this;
    }

    constexpr T &value() &noexcept { return _value; }

    constexpr T &&value() &&noexcept { return std::move(_value); }

    constexpr const T &value() const &noexcept { return _value; }

    constexpr const T &&value() const &&noexcept { return std::move(_value); }

    constexpr T &operator*() &noexcept { return _value; }

    constexpr T &&operator*() &&noexcept { return std::move(_value); }

    constexpr const T &operator*() const &noexcept { return _value; }

    constexpr const T &&operator*() const &&noexcept
    {
        return std::move(_value);
    }

    constexpr T *operator->() noexcept { return std::addressof(_value); }

    constexpr const T *operator->() const noexcept
    {
        return std::addressof(_value);
    }

private:
    [[no_unique_address]] T _value;
};

template <class T>
class Box<T &> {
public:
    using value_type = std::remove_cv_t<T>;

    Box(T &value) : _ptr(std::addressof(value)) {}

    template <class U>
        requires(!detail::converts_from_box<T, U> &&
                 std::is_constructible_v<T &, U>)
    constexpr explicit(!std::is_convertible_v<U, T &>)
        Box(const Box<U> &other) //
        noexcept(std::is_nothrow_constructible_v<T &, U>)
        : _ptr(std::addressof(other.value()))
    {}

    template <class U>
    Box(Box<U> &&) = delete;

    Box(const Box &) = default;

    Box(Box &&) = delete;

    template <class U>
        requires(std::is_assignable_v<T &, U> &&
                 !std::same_as<std::remove_cvref<U>, Box> &&
                 (!std::is_scalar_t<T> ||
                  !std::same_as<std::decay_t<U>, std::decay_t<T>>))
    constexpr Box &operator=(U &&value) //
        noexcept(std::is_nothrow_assignable_v<T &, U>)
    {
        *_ptr = std::forward<U>(value);
        return *this;
    }

    template <class U>
        requires(std::is_assignable_v<T &, const U &> &&
                 !detail::assigns_from_box<T, U>)
    constexpr Box &operator=(const Box<U> &other) //
        noexcept(std::is_nothrow_assignable_v<T &, const U &>)
    {
        *_ptr = other.value();
        return *this;
    }

    template <class U>
        requires(std::is_assignable_v<T &, U> &&
                 !detail::assigns_from_box<T, U>)
    constexpr Box &operator=(Box<U> &&other) //
        noexcept(std::is_nothrow_assignable_v<T &, U>)
    {
        *_ptr = std::move(other.value());
        return *this;
    }

    constexpr Box &operator=(const Box &other) //
        noexcept(std::is_nothrow_copy_constructible_v<T>) //
        requires(std::is_copy_constructible_v<T>)
    {
        *_ptr = other.value();
        return *this;
    }

    constexpr Box &operator=(Box &&other) //
        noexcept(std::is_nothrow_move_assignable_v<T>) //
        requires(std::is_move_assignable_v<T>)
    {
        *_ptr = std::move(other.value());
        return *this;
    }

    constexpr T &value() &noexcept { return *_ptr; }

    constexpr T &&value() &&noexcept { return std::move(*_ptr); }

    constexpr const T &value() const &noexcept { return *_ptr; }

    constexpr const T &&value() const &&noexcept { return std::move(*_ptr); }

    constexpr T &operator*() &noexcept { return *_ptr; }

    constexpr T &&operator*() &&noexcept { return std::move(*_ptr); }

    constexpr const T &operator*() const &noexcept { return *_ptr; }

    constexpr const T &&operator*() const &&noexcept
    {
        return std::move(*_ptr);
    }

    constexpr T *operator->() noexcept { return _ptr; }

    constexpr const T *operator->() const noexcept { return _ptr; }

private:
    T *_ptr;
};

template <class T, class U>
    requires std::swappable_with<T, U>
constexpr void swap(Box<T> &a, Box<U> &b) //
    noexcept(std::is_nothrow_swappable_with_v<T, U>)
{
    std::ranges::swap(a.value(), b.value());
}

// clang-format off
template <class T, class U>
constexpr bool operator<(const Box<T> &a, const Box<U> &b)
    requires requires { { a.value() < b.value() } -> BooleanTestable; }
{
    return a.value() < b.value();
}

template <class T, class U>
constexpr bool operator>(const Box<T> &a, const Box<U> &b)
    requires requires { { a.value() > b.value() } -> BooleanTestable; }
{
    return a.value() > b.value();
}

template <class T, class U>
constexpr bool operator<=(const Box<T> &a, const Box<U> &b)
    requires requires { { a.value() <= b.value() } -> BooleanTestable; }
{
    return a.value() <= b.value();
}

template <class T, class U>
constexpr bool operator>=(const Box<T> &a, const Box<U> &b)
    requires requires { { a.value() >= b.value() } -> BooleanTestable; }
{
    return a.value() >= b.value();
}
// clang-format on

template <class T, class U>
    requires(std::equality_comparable_with<
             typename Box<T>::value_type, typename Box<U>::value_type>)
constexpr bool operator==(const Box<T> &a, const Box<U> &b)
{
    return a.value() == b.value();
}

template <class T, class U>
    requires(std::three_way_comparable_with<
             typename Box<T>::value_type, typename Box<U>::value_type>)
constexpr std::compare_three_way_result_t<T, U> //
operator<=>(const Box<T> &a, const Box<U> &b)
{
    return a.value() <=> b.value();
}

} // namespace hlib

#endif

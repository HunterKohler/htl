#ifndef HLIB_EXPECTED_H_
#define HLIB_EXPECTED_H_

#include <utility>
#include <ranges>
#include <hlib/concepts.h>
#include <hlib/utility.h>

namespace hlib {

// [expected.unexpected], class template unexpected
template <class E>
class unexpected;

// [expected.bad], class template bad_expected_access
template <class E>
class bad_expected_access;

// [expected.bad.void], specialization for void
template <>
class bad_expected_access<void>;

// in-place construction of unexpected values
struct unexpect_t {
    explicit unexpect_t() = default;
};

inline constexpr unexpect_t unexpect{};

// [expected.expected], class template expected
template <class T, class E>
class expected;

// [expected.void], partial specialization of expected for void types
template <class T, class E>
    requires std::is_void_v<T>
class expected<T, E>;

namespace detail {

template <class>
inline constexpr bool is_specialization_of_unexpected = false;

template <class E>
inline constexpr bool is_specialization_of_unexpected<unexpected<E>> = true;

template <class T, class E, class U, class G>
static constexpr bool converts_to_expected =
    std::is_constructible_v<T, expected<U, G> &> ||
    std::is_constructible_v<T, expected<U, G>> ||
    std::is_constructible_v<T, const expected<U, G> &> ||
    std::is_constructible_v<T, const expected<U, G>> ||
    std::is_convertible_v<expected<U, G> &, T> ||
    std::is_convertible_v<expected<U, G>, T> ||
    std::is_convertible_v<const expected<U, G> &, T> ||
    std::is_convertible_v<const expected<U, G>, T>;

template <class T, class U, class E, class G>
inline constexpr bool converts_to_unexpected =
    std::is_constructible_v<unexpected<E>, expected<U, G> &> ||
    std::is_constructible_v<unexpected<E>, expected<U, G>> ||
    std::is_constructible_v<unexpected<E>, const expected<U, G> &> ||
    std::is_constructible_v<unexpected<E>, const expected<U, G>>;

} // namespace detail

template <class E>
class unexpected {
public:
    static_assert(
        !std::is_array_v<E> && std::is_same_v<std::remove_cv_t<E>, E> &&
        std::is_object_v<E> && !detail::is_specialization_of_unexpected<E>);

    unexpected(const unexpected &) = default;

    unexpected(unexpected &&) = default;

    template <class U = E>
        requires(!std::is_same_v<std::remove_cvref_t<U>, unexpected> &&
                 !std::is_same_v<std::remove_cvref_t<U>, std::in_place_t> &&
                 std::is_constructible_v<E, U>)
    constexpr explicit unexpected(U &&u) //
        noexcept(std::is_nothrow_constructible_v<E, U>)
        : _error(std::forward<U>(u))
    {}

    template <class... Args>
        requires std::is_constructible_v<E, Args...>
    constexpr explicit unexpected(std::in_place_t, Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<E, Args...>)
        : _error(std::forward<Args>(args)...)
    {}

    template <class U, class... Args>
        requires(
            std::is_constructible_v<E, std::initializer_list<U> &, Args...>)
    constexpr explicit unexpected(
        std::in_place_t, std::initializer_list<U> il, Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<
                 E, std::initializer_list<U> &, Args...>)
        : _error(il, std::forward<Args>(args)...)
    {}

    unexpected &operator=(const unexpected &) = default;

    unexpected &operator=(unexpected &&) = default;

    constexpr E &error() &noexcept
    {
        return _error;
    }

    constexpr E &&error() &&noexcept
    {
        return std::move(_error);
    }

    constexpr const E &error() const &noexcept
    {
        return _error;
    }

    constexpr const E &&error() const &&noexcept
    {
        return std::move(_error);
    }

    constexpr void swap(unexpected &other) //
        noexcept(std::is_nothrow_swappable_v<E>) //
        requires(std::is_swappable_v<E>)
    {
        std::ranges::swap(_error, other._error);
    }

    friend constexpr void swap(unexpected &x, unexpected &y) //
        noexcept(noexcept(x.swap(y))) //
        requires(std::is_swappable_v<E>)
    {
        x.swap(y);
    }

    friend constexpr void swap(unexpected &x, unexpected &y) = delete;

    // clang-format off

    template <class E2>
    friend constexpr bool operator==(
        const unexpected &x, const unexpected<E2> &y)
        requires requires {
            { x.error() == y.error() } -> BooleanTestable;
        }
    {
        return x.error() == y.error();
    }

    // clang-format on

private:
    E _error;
};

template <class E>
unexpected(E) -> unexpected<E>;

template <>
class bad_expected_access<void> : public std::exception {
protected:
    bad_expected_access() noexcept {}

    bad_expected_access(const bad_expected_access &) noexcept {}

    bad_expected_access(bad_expected_access &&) noexcept {}

    bad_expected_access &operator=(const bad_expected_access &) noexcept
    {
        return *this;
    }

    bad_expected_access &operator=(bad_expected_access &&) noexcept
    {
        return *this;
    }

    virtual ~bad_expected_access() noexcept {}

public:
    virtual const char *what() const noexcept
    {
        return "bad expected access";
    }
};

template <class E>
class bad_expected_access : public bad_expected_access<void> {
public:
    explicit bad_expected_access(E e) //
        noexcept(std::is_nothrow_move_constructible_v<E>)
        : _error(std::move(e))
    {}

    E &error() &noexcept
    {
        return _error;
    }

    E &&error() &&noexcept
    {
        return std::move(_error);
    }

    const E &error() const &noexcept
    {
        return _error;
    }

    const E &&error() const &&noexcept
    {
        return std::move(_error);
    }

private:
    E _error;
};

template <class T, class E>
class expected {
public:
    static_assert(
        !std::is_reference_v<T> && !std::is_function_v<T> &&
        !std::is_same_v<std::remove_cv_t<T>, std::in_place_t> &&
        !std::is_same_v<std::remove_cv_t<T>, unexpect_t> &&
        !detail::is_specialization_of_unexpected<T>);

    static_assert(
        !std::is_array_v<E> && std::is_same_v<std::remove_cv_t<E>, E> &&
        std::is_object_v<E> && !detail::is_specialization_of_unexpected<E>);

    static_assert(std::is_nothrow_destructible_v<T> &&
                  std::is_nothrow_destructible_v<E>);

    using value_type = T;
    using error_type = E;
    using unexpected_type = unexpected<E>;

    template <class U>
    using rebind = expected<U, E>;

    // [expected.object.ctor], constructors
    constexpr expected() //
        noexcept(std::is_nothrow_default_constructible_v<T>) //
        requires(std::is_default_constructible_v<T>)
        : _has_value(true), _value()
    {}

    expected(const expected &) requires(
        std::is_trivially_copy_constructible_v<T>
            &&std::is_trivially_copy_constructible_v<E>) = default;

    constexpr expected(const expected &other) //
        noexcept(std::is_nothrow_copy_constructible_v<T>
                     &&std::is_nothrow_copy_constructible_v<E>) //
        requires(
            std::is_copy_constructible_v<T> &&std::is_copy_constructible_v<E>)
        : _has_value(other._has_value)
    {
        if (other._has_value) {
            _construct_value(other._value);
        } else {
            _construct_error(other._error);
        }
    }

    expected(expected &&) requires(
        std::is_trivially_move_constructible_v<T>
            &&std::is_trivially_move_constructible_v<E>) = default;

    constexpr expected(expected &&other) //
        noexcept(std::is_nothrow_move_constructible_v<T>
                     &&std::is_nothrow_move_constructible_v<E>) //
        requires(
            std::is_move_constructible_v<T> &&std::is_move_constructible_v<E>)
        : _has_value(other._has_value)
    {
        if (other._has_value) {
            _construct_value(other, std::move(_value));
        } else {
            _construct_error(other, std::move(_error));
        }
    }

    template <class U, class G>
        requires(std::is_constructible_v<T, const U &>
                     &&std::is_constructible_v<E, const G &> &&
                 !detail::converts_to_expected<T, E, U, G> &&
                 !detail::converts_to_unexpected<T, E, U, G>)
    constexpr explicit(!std::is_convertible_v<const U &, T> ||
                       !std::is_convertible_v<const G &, E>)
        expected(const expected<U, G> &other) noexcept(
            std::is_nothrow_constructible_v<T, const U &>
                &&std::is_nothrow_constructible_v<E, const G &>)
        : _has_value(other._has_value)
    {
        if (other._has_value) {
            _construct_value(other._value);
        } else {
            _construct_error(other._error);
        }
    }

    template <class U, class G>
        requires(
            std::is_constructible_v<T, U> &&std::is_constructible_v<E, G> &&
            !detail::converts_to_expected<T, E, U, G> &&
            !detail::converts_to_unexpected<T, E, U, G>)
    constexpr explicit(!std::is_convertible_v<U, T> ||
                       !std::is_convertible_v<G, E>)
        expected(expected<U, G> &&other) noexcept(
            std::is_nothrow_constructible_v<T, U>
                &&std::is_nothrow_constructible_v<E, G>)
        : _has_value(other._has_value)
    {
        if (other._has_value) {
            _construct_value(std::move(other._value));
        } else {
            _construct_error(std::move(other._error));
        }
    }

    template <class U = T>
        requires(
            !std::is_same_v<std::remove_cvref_t<U>, std::in_place_t> &&
            !std::is_same_v<expected, std::remove_cvref_t<U>> &&
            !detail::is_specialization_of_unexpected<std::remove_cvref_t<U>> &&
            std::is_constructible_v<T, U>)
    constexpr explicit(!std::is_convertible_v<U, T>) expected(U &&u) //
        noexcept(std::is_nothrow_constructible_v<T, U>)
        : _has_value(true), _value(std::forward<U>(u))
    {}

    template <class G>
        requires(std::is_constructible_v<E, const G &>)
    constexpr explicit(!std::is_convertible_v<const G &, E>)
        expected(const unexpected<G> &e) //
        noexcept(std::is_nothrow_constructible_v<E, const G &>)
        : _has_value(false), _error(e.error())
    {}

    template <class G>
        requires(std::is_constructible_v<E, G>)
    constexpr explicit(!std::is_convertible_v<G, E>)
        expected(unexpected<G> &&e) //
        noexcept(std::is_nothrow_constructible_v<E, G>)
        : _has_value(false), _error(std::move(e.error()))
    {}

    template <class... Args>
        requires(std::is_constructible_v<T, Args...>)
    constexpr explicit expected(std::in_place_t, Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : _has_value(true), _value(std::forward<Args>(args)...)
    {}

    template <class U, class... Args>
        requires(
            std::is_constructible_v<T, std::initializer_list<U> &, Args...>)
    constexpr explicit expected(
        std::in_place_t, std::initializer_list<U> &il, Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<
                 T, std::initializer_list<U> &, Args...>)
        : _has_value(true), _value(il, std::forward<Args>(args)...)
    {}

    template <class... Args>
        requires(std::is_constructible_v<E, Args...>)
    constexpr explicit expected(unexpect_t, Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<E, Args...>)
        : _has_value(false), _error(std::forward<Args>(args)...)
    {}

    template <class U, class... Args>
        requires(
            std::is_constructible_v<E, std::initializer_list<U> &, Args...>)
    constexpr explicit expected(
        unexpect_t, std::initializer_list<U> il, Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<
                 E, std::initializer_list<U> &, Args...>)
        : _has_value(false), _error(il, std::forward<Args>(args)...)
    {}

    ~expected() requires(
        std::is_trivially_default_constructible_v<T>
            &&std::is_trivially_default_constructible_v<E>) = default;

    constexpr ~expected() noexcept
    {
        if (has_value()) {
            std::ranges::destroy_at(std::addressof(_value));
        } else {
            std::ranges::destroy_at(std::addressof(_error));
        }
    }

    // [expected.object.assign], assignment

    constexpr expected &operator=(const expected &other) = delete;

    constexpr expected &operator=(const expected &other) //
        // noexcept(std::is_nothrow_copy_assignable_v<T>
        //              &&std::is_nothrow_copy_constructible_v<T>
        //                  &&std::is_nothrow_copy_assignable_v<E>
        //                      &&std::is_nothrow_copy_constructible_v<E>) //
        requires(std::is_copy_assignable_v<T> &&std::is_copy_constructible_v<T>
                     &&std::is_copy_assignable_v<E>
                         &&std::is_copy_constructible_v<E> &&
                 (std::is_nothrow_move_constructible_v<T> ||
                  std::is_nothrow_move_constructible_v<E>))
    {
        if (has_value() && other._has_value) {
            _value = other._value;
        } else if (has_value()) {
            _reset(_error, _value, other._error);
            _value = false;
        } else if (other._has_value) {
            _reset(_value, _error, other._value);
            _value = true;
        } else {
            _error = other._error;
        }

        return *this;
    }

    // constexpr expected &operator=(expected &&other) = delete;

    constexpr expected &operator=(expected &&other) //
        noexcept(std::is_nothrow_move_assignable_v<T>
                     &&std::is_nothrow_move_constructible_v<T>
                         &&std::is_nothrow_move_assignable_v<E>
                             &&std::is_nothrow_move_constructible_v<E>) //
        requires(std::is_move_constructible_v<T> &&std::is_move_assignable_v<T>
                     &&std::is_move_constructible_v<E>
                         &&std::is_move_assignable_v<E> &&
                 (std::is_nothrow_move_constructible_v<T> ||
                  std::is_nothrow_move_constructible_v<E>)) //
    {
        if (has_value() && other._has_value) {
            _value = std::move(other._value);
        } else if (has_value()) {
            _reset(_error, _value, std::move(other._error));
            _value = false;
        } else if (other._has_value) {
            _reset(_value, _error, std::move(other._value));
            _value = true;
        } else {
            _error = std::move(other._error);
        }

        return *this;
    }

    template <class U = T>
        requires(
            !std::is_same_v<expected, std::remove_cvref_t<U>> &&
            !detail::is_specialization_of_unexpected<U> &&
            std::is_constructible_v<T, U> && std::is_assignable_v<T &, U> &&
            (std::is_nothrow_constructible_v<T, U> ||
             std::is_nothrow_constructible_v<T> ||
             std::is_nothrow_move_constructible_v<E>))
    constexpr expected &operator=(U &&new_value)
    {
        if (has_value()) {
            _value = std::forward<U>(new_value);
        } else {
            _reset(_value, _error, std::forward<U>(new_value));
            _has_value = true;
        }

        return *this;
    }

    template <class G>
        requires(std::is_constructible_v<E, const G &>
                     &&std::is_assignable_v<E &, const G &> &&
                 (std::is_nothrow_constructible_v<E, const G &> ||
                  std::is_nothrow_move_constructible_v<T> ||
                  std::is_nothrow_move_constructible_v<E>))
    constexpr expected &operator=(const unexpected<G> &e)
    {
        if (has_value()) {
            _reset(_error, _value, e.error());
        } else {
            _error = e.error();
        }

        return *this;
    }

    template <class G>
        requires(std::is_constructible_v<E, G> &&std::is_assignable_v<E &, G> &&
                 (std::is_nothrow_constructible_v<E, G> ||
                  std::is_nothrow_move_constructible_v<T> ||
                  std::is_nothrow_move_constructible_v<E>))
    constexpr expected &operator=(unexpected<G> &&e)
    {
        if (has_value()) {
            _reset(_error, _value, std::move(e.error()));
        } else {
            _error = std::move(e.error());
        }

        return *this;
    }

    template <class... Args>
        requires(std::is_nothrow_destructible_v<T, Args...>)
    constexpr T &emplace(Args &&...args) noexcept
    {
        return _emplace(std::forward<Args>(args)...);
    }

    template <class U, class... Args>
        requires(std::is_nothrow_destructible_v<
                 T, std::initializer_list<U> &, Args...>)
    constexpr T &emplace(std::initializer_list<U> il, Args &&...args) noexcept
    {
        return _emplace(il, std::forward<Args>(args)...);
    }

    // [expected.object.swap], swap
    constexpr void swap(expected &other) //
        noexcept(std::is_nothrow_move_constructible_v<T>
                     &&std::is_nothrow_swappable_v<T>
                         &&std::is_nothrow_move_constructible_v<E>
                             &&std::is_nothrow_swappable_v<E>) //
        requires(std::is_swappable_v<T> &&std::is_swappable_v<E>
                     &&std::is_move_constructible_v<T>
                         &&std::is_move_constructible_v<E> &&
                 (std::is_nothrow_move_constructible_v<T> ||
                  std::is_nothrow_move_constructible_v<E>))
    {
        if (has_value()) {
            if (other._has_value) {
                std::ranges::swap(_value, other._value);
            } else {
                _swap_to_empty(_value, other);
            }
        } else if (other._has_value) {
            other._swap_to_empty(_value, other._value);
        } else {
            std::ranges::swap(_error, other._error);
        }
    }

    // clang-format off
    friend constexpr void swap(expected &x, expected &y) //
        noexcept(noexcept(x.swap(y)))
        requires requires { x.swap(y); }
    {
        return x.swap(y);
    }
    // clang-format on

    friend constexpr void swap(expected &x, expected &y) = delete;

    // [expected.object.obs], observers

    constexpr T *operator->() noexcept
    {
        assert(has_value());
        return std::addressof(_value);
    }

    constexpr const T *operator->() const noexcept
    {
        assert(has_value());
        return std::addressof(_value);
    }

    constexpr T &operator*() &noexcept
    {
        assert(has_value());
        return _value;
    }

    constexpr T &&operator*() &&noexcept
    {
        assert(has_value());
        return std::move(_value);
    }

    constexpr const T &operator*() const &noexcept
    {
        assert(has_value());
        return _value;
    }

    constexpr const T &&operator*() const &&noexcept
    {
        assert(has_value());
        return std::move(_value);
    }

    constexpr explicit operator bool() const noexcept
    {
        return _has_value;
    }

    constexpr bool has_value() const noexcept
    {
        return _has_value;
    }

    constexpr T &value() &
    {
        if (!has_value()) {
            throw bad_expected_access(error());
        }

        return _value;
    }

    constexpr const T &value() const &
    {
        if (!has_value()) {
            throw bad_expected_access(error());
        }

        return _value;
    }

    constexpr T &&value() &&
    {
        if (!has_value()) {
            throw bad_expected_access(std::move(error()));
        }

        return std::move(_value);
    }

    constexpr const T &&value() const &&
    {
        if (!has_value()) {
            throw bad_expected_access(std::move(error()));
        }

        return std::move(_value);
    }

    constexpr E &error() &noexcept
    {
        assert(!has_value());
        return _error;
    }

    constexpr const E &error() const &noexcept
    {
        assert(!has_value());
        return _error;
    }

    constexpr E &&error() &&noexcept
    {
        assert(!has_value());
        return std::move(_error);
    }

    constexpr const E &&error() const &&noexcept
    {
        assert(!has_value());
        return std::move(_error);
    }

    template <class U>
        requires(std::is_copy_constructible_v<T> &&std::is_convertible_v<U, T>)
    constexpr T value_or(U &&u) &&
    {
        return _has_value ? _value : implicit_cast<T>(std::forward<U>(u));
    }

    template <class U>
        requires(std::is_move_constructible_v<T> &&std::is_convertible_v<U, T>)
    constexpr T value_or(U &&u) const &
    {
        return _has_value ? std::move(_value)
                          : implicit_cast<T>(std::forward<U>(u));
    }

    // [expected.object.eq], equality operators

    // clang-format off
    template <class T2, class E2>
    friend constexpr bool operator==(
        const expected &x, const expected<T2, E2> &y)
        requires requires {
            requires !std::is_void_v<T2>;
            { x.value() == y.value() } -> BooleanTestable;
            { x.error() == y.error() } -> BooleanTestable;
        }
    {
        return x.has_value() == y.has_value() && (
            (x.has_value() && implicit_cast<bool>(*x == *y)) ||
            (!x.has_value() && implicit_cast<bool>(x.error() == x.error())));
    }

    template <class T2>
    friend constexpr bool operator==(const expected &x, const T2 &y)
        requires requires {
            { x.value() == y } -> BooleanTestable;
        }
    {
        return x.has_value() && implicit_cast<bool>(*x == y);
    }


    template <class E2>
    friend constexpr bool operator==(const expected &x, const unexpected<E2> &y)
        requires requires {
            { x.error() == y.error() } -> BooleanTestable;
        }
    {
        return !x.has_value() && implicit_cast<bool>(x.error() == y.error());
    }

private:
    bool _has_value;
    union {
        T _value;
        E _error;
    };

    template <class... Args>
    constexpr void _construct_value(Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<T, Args...>)
    {
        std::ranges::construct_at(
            std::addressof(_value), std::forward<Args>(args)...);
    }

    template <class... Args>
    constexpr void _construct_error(Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<T, Args...>)
    {
        std::ranges::construct_at(
            std::addressof(_error), std::forward<Args>(args)...);
    }

    constexpr void _destroy_value() noexcept
    {
        std::ranges::destroy_at(std::addressof(_value));
    }

    constexpr void _destroy_error() noexcept
    {
        std::ranges::destroy_at(std::addressof(_error));
    }

    template <class... Args>
    constexpr T &_emplace(Args &&...args) noexcept
    {
        if (has_value()) {
            std::ranges::destroy_at(std::addressof(_value));
        } else {
            std::ranges::destroy_at(std::addressof(_error));
            _has_value = true;
        }

        std::ranges::construct_at(
            std::addressof(_value), std::forward<Args>(args)...);

        return _value;
    }

    template <class U, class V, class... Args>
    constexpr void _reset(U &new_value, V &old_value, Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<U, Args...>)
    {
        if constexpr (std::is_nothrow_constructible_v<U, Args...>) {
            std::ranges::destroy_at(std::addressof(old_value));
            std::ranges::construct_at(
                std::addressof(new_value), std::forward<Args>(args)...);
        } else if constexpr (std::is_nothrow_move_constructible_v<U>) {
            U tmp(std::forward<Args>(args)...);
            std::ranges::destroy_at(std::addressof(old_value));
            std::ranges::construct_at(
                std::addressof(new_value), std::forward<Args>(args)...);
        } else {
            V tmp(std::move(old_value));
            std::ranges::destroy_at(std::addressof(old_value));
            try {
                std::ranges::construct_at(
                    std::addressof(new_value), std::forward<Args>(args)...);
            } catch (...) {
                std::ranges::construct_at(
                    std::addressof(old_value), std::move(tmp));
                throw;
            }
        }
    }

    constexpr void _swap_to_empty(expected &other) //
        noexcept(std::is_nothrow_move_constructible_v<E>
                     &&std::is_nothrow_move_constructible_v<T>) //
        requires(std::is_nothrow_move_constructible_v<E> ||
                 std::is_nothrow_move_constructible_v<T>)
    {
        if constexpr (std::is_nothrow_move_constructible_v<E> &&
                      std::is_nothrow_move_constructible_v<T>) {
            E tmp(std::move(other._error));
            other._destroy_error();
            other._construct_value(std::move(_value));
            _destroy_value();
            _construct_error(std::move(tmp));
        } else if constexpr (std::is_nothrow_move_constructible_v<E>) {
            E tmp(std::move(other._error));
            other._destroy_error();
            try {
                other._construct_value(std::move(_value));
            } catch (...) {
                other._construct_error(std::move(tmp));
                throw;
            }
            _destroy_value();
            _construct_error(std::move(tmp));
        } else {
            T tmp(std::move(_value));
            _destroy_value();
            try {
                _construct_error(std::move(other._error));
            } catch (...) {
                _construct_value(std::move(tmp));
                throw;
            }
            other._destroy_error();
            other._construct_value(std::move(tmp));
        }
    }
};

template <class T, class E>
    requires std::is_void_v<T>
class expected<T, E> {
public:
    static_assert(
        !std::is_array_v<E> && std::is_same_v<std::remove_cv_t<E>, E> &&
        std::is_object_v<E> && !detail::is_specialization_of_unexpected<E>);

    static_assert(std::is_nothrow_destructible_v<E>);

    using value_type = T;
    using error_type = E;
    using unexpected_type = unexpected<E>;

    template <class U>
    using rebind = expected<U, error_type>;

    // [expected.void.ctor], constructors
    constexpr expected() noexcept : _has_value(true) {}

    expected(const expected &) requires(
        std::is_trivially_copy_constructible_v<E>) = default;

    constexpr expected(const expected &other) //
        noexcept(std::is_nothrow_copy_constructible_v<E>) //
        requires(std::is_copy_constructible_v<E>)
        : _has_value(other._has_value)
    {
        if (other._has_value) {
            std::ranges::construct_at(std::addressof(_error), other._error);
        }
    }

    expected(expected &&) requires(std::is_trivially_move_constructible_v<E>) =
        default;

    constexpr expected(expected &&other) //
        noexcept(std::is_nothrow_move_constructible_v<E>) //
        requires(std::is_move_constructible_v<E>)
        : _has_value(other._has_value)
    {
        if (other._has_value) {
            std::ranges::construct_at(
                std::addressof(_error), std::move(other._error));
        }
    }

    template <class U, class G>
        requires(std::is_void_v<U> &&std::is_constructible_v<E, const G &> &&
                 !detail::converts_to_unexpected<T, E, U, G>)
    constexpr explicit(std::is_convertible_v<const G &, E>)
        expected(const expected<U, G> &other) //
        noexcept(std::is_nothrow_constructible_v<E, const G &>)
        : _has_value(other._has_value)
    {
        if (other._has_value) {
            _construct_error(other._error);
        }
    }

    template <class U, class G>
        requires(std::is_void_v<U> &&std::is_constructible_v<E, G> &&
                 !detail::converts_to_unexpected<T, E, U, G>)
    constexpr explicit(std::is_convertible_v<G, E>)
        expected(expected<U, G> &&other) //
        noexcept(std::is_nothrow_constructible_v<E, G>)
        : _has_value(other._has_value)
    {
        if (other._has_value) {
            _construct_error(std::move(other._error));
        }
    }

    template <class G>
        requires(std::is_constructible_v<E, const G &>)
    constexpr explicit(!std::is_convertible_v<const G &, E>)
        expected(const unexpected<G> &e) //
        noexcept(std::is_nothrow_constructible_v<E, const G &>)
        : _has_value(false), _error(e.error())
    {}

    template <class G>
        requires(std::is_constructible_v<E, G>)
    constexpr explicit(!std::is_convertible_v<G, E>)
        expected(unexpected<G> &&e) //
        noexcept(std::is_nothrow_constructible_v<E, G>)
        : _has_value(false), _error(std::move(e.error()))
    {}

    constexpr explicit expected(std::in_place_t) noexcept : _has_value(true) {}

    template <class... Args>
        requires(std::is_constructible_v<E, Args...>)
    constexpr explicit expected(unexpect_t, Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<E, Args...>)
        : _has_value(false), _error(std::forward<Args>(args)...)
    {}

    template <class U, class... Args>
        requires(
            std::is_constructible_v<E, std::initializer_list<U> &, Args...>)
    constexpr explicit expected(
        unexpect_t, std::initializer_list<U> il, Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<
                 E, std::initializer_list<U> &, Args...>)
        : _has_value(false), _error(il, std::forward<Args>(args)...)
    {}

    ~expected() requires(std::is_trivially_destructible_v<E>) = default;

    // [expected.void.dtor], destructor
    constexpr ~expected() noexcept
    {
        if (!_has_value) {
            _destroy_error();
        }
    }

    // [expected.void.assign], assignment
    constexpr expected &operator=(const expected &other) //
        noexcept(std::is_nothrow_copy_assignable_v<E>
                     &&std::is_nothrow_copy_constructible_v<E>) //
        requires(std::is_copy_assignable_v<E> &&std::is_copy_constructible_v<E>)
    {
        if (!_has_value && !other._has_value) {
            _error = other._error;
        } else if (_has_value) {
            _construct_error(other._error);
            _has_value = false;
        } else if (other._has_value) {
            _destroy_error();
            _has_value = true;
        }

        return *this;
    }

    constexpr expected &operator=(expected &&other) //
        noexcept(std::is_nothrow_move_assignable_v<E>
                     &&std::is_nothrow_move_constructible_v<E>) //
        requires(std::is_move_assignable_v<E> &&std::is_move_constructible_v<E>)
    {
        if (!has_value() && !other._has_value) {
            _error = std::move(other._error);
        } else if (has_value()) {
            _construct_error(std::move(other._error));
            _has_value = false;
        } else if (other._has_value) {
            _destroy_error();
            _has_value = true;
        }

        return *this;
    }

    template <class G>
        requires(std::is_constructible_v<E, const G &>
                     &&std::is_assignable_v<E &, const G &>)
    constexpr expected &operator=(const unexpected<G> &e) //
        noexcept(std::is_nothrow_constructible_v<E, const G &>
                     &&std::is_nothrow_assignable_v<E &, const G &>)
    {
        if (has_value()) {
            _construct_error(e.error());
            _has_value = false;
        } else {
            _error = e.error();
        }
    }

    template <class G>
        requires(std::is_constructible_v<E, G> &&std::is_assignable_v<E &, G>)
    constexpr expected &operator=(unexpected<G> &&e) //
        noexcept(std::is_nothrow_constructible_v<E, G>
                     &&std::is_nothrow_assignable_v<E &, G>)
    {
        if (has_value()) {
            _contruct_error(std::move(e._error()));
            _has_value = false;
        } else {
            _error = std::move(e.error());
        }
    }

    constexpr void emplace() noexcept
    {
        if (!has_value()) {
            _destroy_error();
            _has_value = true;
        }
    }

    // [expected.void.swap], swap
    constexpr void swap(expected &other) //
        noexcept(std::is_nothrow_swappable_v<E>
                     &&std::is_nothrow_move_constructible_v<E>) //
        requires(std::is_swappable_v<E> &&std::is_move_constructible_v<E>)
    {
        if (has_value()) {
            if (!other._has_value) {
                _swap_to_empty(other);
            }
        } else if (other._has_value) {
            other._swap_to_empty(*this);
        } else {
            std::ranges::swap(_error, other._error);
        }
    }

    friend constexpr void swap(expected &x, expected &y) //
        noexcept(noexcept(x.swap(y))) //
        requires(std::is_swappable_v<E> &&std::is_move_constructible_v<E>)
    {
        return x.swap(y);
    }

    friend constexpr void swap(expected &, expected &) = delete;

    // [expected.void.obs], observers
    constexpr explicit operator bool() const noexcept { return _has_value; }

    constexpr bool has_value() const noexcept { return _has_value; }

    constexpr void operator*() const noexcept { assert(has_value()); }

    constexpr void value() const &
    {
        if (!has_value()) {
            throw bad_expected_access(_error);
        }
    }

    constexpr void value() &&
    {
        if (!has_value()) {
            throw bad_expected_access(std::move(_error));
        }
    }

    constexpr void value() const &&
    {
        if (!has_value()) {
            throw bad_expected_access(std::move(_error));
        }
    }

    constexpr E &error() &noexcept
    {
        assert(!has_value());
        return _error;
    }

    constexpr E &&error() &&noexcept
    {
        assert(!has_value());
        return std::move(_error);
    }

    constexpr const E &error() const &noexcept
    {
        assert(!has_value());
        return _error;
    }

    constexpr const E &&error() const &&noexcept
    {
        assert(!has_value());
        return std::move(_error);
    }

    // [expected.void.eq], equality operators

    // clang-format off

    template <class T2, class E2>
    friend constexpr bool operator==(expected &x, expected<T2, E2> &y)
        requires requires {
            requires std::is_void_v<T2>;
            { x.error() == y.error() } -> BooleanTestable;
        }
    {
        return x.has_value() == y.has_value() &&
               (x.has_value() || implicit_cast<bool>(x.error() == y.error()));
    }

    template <class E2>
    friend constexpr bool operator==(const expected &x, const unexpected<E2> &y)
        requires requires {
            { x.error() == y.error() } -> BooleanTestable;
        }
    {
        return !x.has_value() && implicit_cast<bool>(x.error() == y.error());
    }

    // clang-format on

private:
    bool _has_value; // exposition only
    union {
        E _error; // exposition only
    };

    constexpr void _swap_to_empty(expected &other) //
        noexcept(std::is_nothrow_move_constructible_v<E>) //
        requires(std::is_move_constructible_v<E>)
    {
        assert(has_value() && !other._has_value);

        other._construct_error(std::move(_error));
        other._has_value = true;
        _destroy_error();
        _has_value = false;
    }

    template <class... Args>
    constexpr void _construct_error(Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<E, Args...>)
    {
        std::ranges::construct_at(_error, std::forward<Args>(args)...);
    }

    constexpr void _destroy_error() noexcept
    {
        std::ranges::destroy_at(std::addressof(_error));
    }
};

} // namespace hlib

#endif

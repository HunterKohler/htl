#ifndef HLIB_CONCEPTS_H_
#define HLIB_CONCEPTS_H_

#include <chrono>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <ranges>
#include <tuple>
#include <utility>

// clang-format off

namespace hlib {

/**
 * See: https://en.cppreference.com/w/cpp/concepts/boolean-testable
 */
template <class T>
concept BooleanTestable =
    std::convertible_to<T, bool> &&
    requires (T &&t) { { !std::forward<T>(t) } -> std::convertible_to<bool>; };

template <class T>
concept Arithmetic = std::is_arithmetic_v<T>;

template <class T>
concept Nullable =
    std::regular<T> &&
    std::equality_comparable_with<T, std::nullptr_t> &&
    std::assignable_from<T, std::nullptr_t> &&
    std::constructible_from<T, std::nullptr_t>;

template <class T>
concept NullablePointer = BooleanTestable<T> && Nullable<T> &&
    requires { typename std::pointer_traits<T>::element_type; };

template <class Alloc>
concept AllocatorLike = std::is_class_v<Alloc> && requires(Alloc &a,
    typename std::allocator_traits<Alloc>::size_type n,
    typename std::allocator_traits<Alloc>::pointer p)
{
    a.allocate(n);
    a.deallocate(p, n);
};

namespace detail {

template <class T, std::size_t N>
inline constexpr bool is_tuple_element = requires (T t) {
    typename std::tuple_element_t<N - 1, std::remove_const_t<T>>;
    { std::get<N - 1>(t) } ->
        std::convertible_to<std::tuple_element_t<N - 1, T>>;
    requires is_tuple_element<T, N - 1>;
};

template <class T>
inline constexpr bool is_tuple_element<T, 0> = true;

} // namespace detail

template <class T>
concept TupleLike = requires {
    typename std::tuple_size<T>::type;
    requires !std::is_reference_v<T>;
    requires detail::is_tuple_element<T, std::tuple_size_v<T>>;
};

template <class T>
concept PairLike = TupleLike<T> && std::tuple_size_v<T> == 2;

template <class T>
concept Constant = std::is_const_v<std::remove_reference_t<T>>;

template <class T>
concept Mutable = !Constant<T>;

/**
 * Can be stored (through dynamic allocation). Namely, a cv-unqualified
 * object type. See requirements for allocator value types and contained
 * values.
 */
template <class T>
concept Storable = std::is_object_v<T> && !std::is_const_v<T> &&
    !std::is_volatile_v<T>;

template <class From, class To>
concept ExplicitlyConvertibleTo =
    requires { static_cast<To>(std::declval<From>()); };


// https://en.cppreference.com/w/cpp/named_req/BitmaskType
template <class T>
concept BitmaskType = requires(T &x, T &y)
{
    { ~x } -> std::same_as<T>;
    { x & y } -> std::same_as<T>;
    { x | y } -> std::same_as<T>;
    { x ^ y } -> std::same_as<T>;
    { x &= y } -> std::same_as<T &>;
    { x |= y } -> std::same_as<T &>;
    { x ^= y } -> std::same_as<T &>;
};

// https://en.cppreference.com/w/cpp/named_req/BasicLockable
template <class T>
concept BasicLockable = requires(T &t)
{
    t.lock();
    t.unlock();
};

// https://en.cppreference.com/w/cpp/named_req/Lockable
template <class T>
concept Lockable = requires(T &t)
{
    { t.try_lock() } -> BooleanTestable;
};

// https://en.cppreference.com/w/cpp/named_req/TimedLockable
template <class T>
concept TimedLockable = requires(
    T &t, std::chrono::duration<int> duration,
    std::chrono::time_point<std::chrono::system_clock> time)
{
    { t.try_lock_for(duration) } -> BooleanTestable;
    { t.try_lock_until(time) } -> BooleanTestable;
};

} // namespace hlib

#endif

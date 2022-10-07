#ifndef HLIB_MATH_H_
#define HLIB_MATH_H_

#include <concepts>
#include <type_traits>
#include <utility>
#include <limits>

namespace hlib {

constexpr bool checked_add(std::integral auto a, std::integral auto b,
                           std::integral auto &res) noexcept
{
    return __builtin_add_overflow(a, b, std::addressof(res));
}

constexpr bool checked_sub(std::integral auto a, std::integral auto b,
                           std::integral auto &res) noexcept
{
    return __builtin_sub_overflow(a, b, std::addressof(res));
}

constexpr bool checked_mul(std::integral auto a, std::integral auto b,
                           std::integral auto &res) noexcept
{
    return __builtin_mul_overflow(a, b, std::addressof(res));
}

template <class T>
    requires std::is_arithmetic_v<T>
constexpr int sign(T value) noexcept
{
    if constexpr (std::is_same_v<T, bool>) {
        return value;
    } else if constexpr (std::is_unsigned_v<T>) {
        return value > T{};
    } else if constexpr (std::is_signed_v<T>) {
        return (value > T{}) - (value < T{});
    }
}

template <class T>
    requires std::is_arithmetic_v<T>
constexpr bool signbit(T value) noexcept
{
    if constexpr (std::is_unsigned_v<T>) {
        return false;
    } else {
        return value > 0;
    }
}

} // namespace hlib

#endif

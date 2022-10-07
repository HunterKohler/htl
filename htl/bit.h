#ifndef HTL_BIT_H_
#define HTL_BIT_H_

#include <bit>
#include <concepts>
#include <algorithm>
#include <climits>
#include <ranges>

namespace htl {
namespace detail {

constexpr std::uint16_t byteswap16(std::uint16_t value) noexcept
{
    if constexpr (__has_builtin(__builtin_bswap16)) {
        return __builtin_bswap16(value);
    } else {
        return ((value << 8) & 0xFF00) | ((value >> 8) & 0xFF);
    }
}

constexpr std::uint32_t byteswap32(std::uint32_t value) noexcept
{
    if constexpr (__has_builtin(__builtin_bswap32)) {
        return __builtin_bswap32(value);
    } else {
        return ((value << 24) & 0xFF000000) | ((value << 8) & 0xFF0000) |
               ((value >> 8) & 0xFF00) | ((value >> 24) & 0xFF);
    }
}

constexpr std::uint64_t byteswap64(std::uint64_t value) noexcept
{
    if constexpr (__has_builtin(__builtin_bswap64)) {
        return __builtin_bswap64(value);
    } else {
        return ((value << 56) & 0xFF00000000000000) |
               ((value << 40) & 0xFF000000000000) |
               ((value << 24) & 0xFF0000000000) |
               ((value << 8) & 0xFF00000000) | ((value >> 8) & 0xFF000000) |
               ((value >> 24) & 0xFF0000) | ((value >> 40) & 0xFF00) |
               ((value >> 56) & 0xFF);
    }
}

} // namespace detail

// [bit.byteswap], byteswap (C++23)
template <std::integral T>
constexpr T byteswap(T value) noexcept
{
    if constexpr (sizeof(T) == 1) {
        return value;
    } else if constexpr (sizeof(T) == 2) {
        return detail::byteswap16(value);
    } else if constexpr (sizeof(T) == 4) {
        return detail::byteswap32(value);
    } else if constexpr (sizeof(T) == 8) {
        return detail::byteswap64(value);
    } else {
        static_assert(!sizeof(T), "byteswap not supported for type");
        return 0;
    }
}

} // namespace htl

#endif

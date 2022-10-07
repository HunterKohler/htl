#ifndef HLIB_DETAIL_BYTE_ARRAY_H_
#define HLIB_DETAIL_BYTE_ARRAY_H_

#include <array>
#include <concepts>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace hlib {
namespace detail {

template <std::size_t N>
struct ByteArray : std::array<std::uint8_t, N> {
    // Allows non-value default initialization
    constexpr ByteArray() noexcept = default;

    template <std::constructible_from<std::uint8_t>... Args>
    constexpr ByteArray(Args &&...args) //
        noexcept((std::is_nothrow_constructible_v<std::uint8_t, Args> && ...))
        : std::array<std::uint8_t, N>{ static_cast<std::uint8_t>(
              std::forward<Args>(args))... }
    {}
};

} // namespace detail
} // namespace hlib

#endif

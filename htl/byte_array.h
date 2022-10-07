#ifndef HLIB_BYTE_ARRAY_H_
#define HLIB_BYTE_ARRAY_H_

#include <array>
#include <type_traits>

namespace hlib {

template <std::size_t N>
struct ByteArray : std::array<std::uint8_t, N> {
    ByteArray() = default;

    template <class... Args>
        requires(std::is_constructible_v<std::uint8_t, Args> &&...)
    ByteArray(Args &&...args)
    noexcept((std::is_nothrow_constructible_v<std::uint8_t, Args> && ...))
        : std::array<std::uint8_t, N>{ static_cast<std::uint8_t>(
              std::forward<Args>(args))... }
    {}
};

// namespace detail {

// inline constexpr char byte_array_hex_charset[] = "0123456789ABCDEF";

// template <std::size_t N>
// inline void byte_array_to_hex(
//     const ByteArray<N> &data, std::output_iterator<std::uint8_t> auto out)
// {
//     for (auto value: data) {
//         *out = byte_array_hex_charset[value >> 4];
//         ++out;
//         *out = byte_array_hex_charset[value & 15];
//         ++out;
//     }
// }

// } // namespace detail

// template <class Alloc, std::size_t N>
// inline std::basic_string<char, std::char_traits<char>, Alloc>
// to_string(const ByteArray<N> &data, const Alloc &alloc = Alloc())
// {
//     std::basic_string<char, std::char_traits<char>, Alloc> dest(alloc);
//     detail::byte_array_to_hex(data, std::back_inserter(dest));
//     return dest;
// }

// template <class CharT, class Traits, std::size_t N>
// inline std::basic_ostream<CharT, Traits> &
// operator<<(std::basic_ostream<CharT, Traits> &os, const ByteArray<N> &data)
// {
//     detail::byte_array_to_hex(std::ostream_iterator<char>(os));
//     return os;
// }

} // namespace hlib

#endif

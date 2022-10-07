#ifndef HLIB_EXPERIMENTAL_UUID_H_
#define HLIB_EXPERIMENTAL_UUID_H_

// #include <cstdint>
// #include <charconv>
// #include <random>
// #include <exception>
// #include <stdexcept>
// #include <hlib/codec.h>
// #include <hlib/detail/byte_array.h>

// namespace hlib {

// enum class UUIDVersion : std::uint8_t {
//     v1 = 1,
//     v2 = 2,
//     v3 = 3,
//     v4 = 4,
//     v5 = 5,
// };

// class UUID {
// public:
//     using bytes_type = detail::ByteArray<16>;

//     constexpr UUID() noexcept = default;

//     constexpr UUID(std::nullptr_t) noexcept : _data{} {}

//     constexpr UUID(const bytes_type &data) noexcept : _data{ data } {}

//     // constexpr UUID(std::string_view s);

//     constexpr bytes_type to_bytes() const noexcept
//     {
//         return _data;
//     }

//     friend void swap(UUID &a, UUID &b) noexcept
//     {
//         std::ranges::swap(a._data, b._data);
//     }

//     friend bool operator==(const UUID &a, const UUID &b) = default;

//     friend std::strong_ordering operator<=>(const UUID &a, const UUID &b) =
//         default;

// private:
//     bytes_type _data;

//     bool _try_parse(std::string_view s)
//     {
//         if (s.size() != 36 || s[8] != '-' || s[13] != '-' || s[18] != '-' ||
//             s[23] != '-') {
//             return true;
//         }

//         char hex[]{
//             s[0],  s[1],  s[2],  s[3],  s[4],  s[5],  s[6],  s[7],
//             s[9],  s[10], s[11], s[12], s[14], s[15], s[16], s[17],
//             s[19], s[20], s[21], s[22], s[24], s[25], s[26], s[27],
//             s[28], s[29], s[30], s[31], s[32], s[33], s[34], s[35],
//         };

//         auto res = hex_decode(hex, _data.begin());
//         if (res.err != std::errc{} || res.in != s.end()) {
//             return true;
//         }

//         return false;
//     }
// };

// // namespace detail {

// // constexpr expected<UUID, std::errc> try_make_uuid()
// // {}

// // } // namespace detail

// // constexpr expected<UUID, std::errc> try_make_uuid(
// //     std::string_view s, const std::error_code &ec)
// // {
// //     return {};
// // }

// // constexpr UUID make_uuid(std::string_view value)
// // {
// //     UUID::bytes_type data;

// //     if (s.size() != 36 || s[8] != '-' || s[13] != '-' || s[18] != '-' ||
// //         s[23] != '-') {
// //         return true;
// //     }

// //     char hex[]{
// //         s[0],  s[1],  s[2],  s[3],  s[4],  s[5],  s[6],  s[7],
// //         s[9],  s[10], s[11], s[12], s[14], s[15], s[16], s[17],
// //         s[19], s[20], s[21], s[22], s[24], s[25], s[26], s[27],
// //         s[28], s[29], s[30], s[31], s[32], s[33], s[34], s[35],
// //     };

// //     auto res = hex_decode(hex, _data.begin());
// //     if (res.err != std::errc{} || res.in != s.end()) {
// //         return true;
// //     }

// //     return false;
// // }

// } // namespace hlib

#endif

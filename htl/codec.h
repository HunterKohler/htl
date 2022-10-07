#ifndef HLIB_CODEC_H_
#define HLIB_CODEC_H_

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <ranges>
#include <concepts>
#include <type_traits>
#include <algorithm>
#include <hlib/type_traits.h>
#include <hlib/concepts.h>
#include <hlib/utility.h>
#include <hlib/detail/iterator.h>
#include <hlib/detail/codec.h>

namespace hlib {

template <class I, class O, class E>
struct CodecResult {
    [[no_unique_address]] I in;
    [[no_unique_address]] O out;
    [[no_unique_address]] E err;

    template <class I2, class O2, class E2>
        requires std::convertible_to<const I &, I2> &&
            std::convertible_to<const O &, O2> &&
            std::convertible_to<const E &, E2>
    constexpr operator CodecResult<I2, O2, E2>() const &
    {
        return { in, out, err };
    }

    template <class I2, class O2, class E2>
        requires std::convertible_to<I, I2> && std::convertible_to<O, O2> &&
            std::convertible_to<E, E2>
    constexpr operator CodecResult<I2, O2, E2>() &&
    {
        return { std::move(in), std::move(out), std::move(err) };
    }
};

template <class I, class O>
using BinEncodeResult = CodecResult<I, O, std::error_code>;

template <class I, class O>
using BinDecodeResult = CodecResult<I, O, std::error_code>;

template <class I, class O>
using HexEncodeResult = CodecResult<I, O, std::error_code>;

template <class I, class O>
using HexDecodeResult = CodecResult<I, O, std::error_code>;

template <class I, class O>
using Base64EncodeResult = CodecResult<I, O, std::error_code>;

template <class I, class O>
using Base64DecodeResult = CodecResult<I, O, std::error_code>;

template <class I, class O>
using Base64URLEncodeResult = CodecResult<I, O, std::error_code>;

template <class I, class O>
using Base64URLDecodeResult = CodecResult<I, O, std::error_code>;

template <std::input_iterator I, std::sentinel_for<I> S,
          std::output_iterator<char> O,
          std::sentinel_for<O> B = std::unreachable_sentinel_t>
    requires is_byte_v<std::iter_value_t<I>>
constexpr BinEncodeResult<I, O> bin_encode(I first, S last, O out, B bound = {})
{
    std::errc err = detail::try_bin_encode(first, last, out, bound);
    return { std::move(first), std::move(out), std::make_error_code(err) };
}

template <std::ranges::input_range R, std::output_iterator<char> O,
          std::sentinel_for<O> B = std::unreachable_sentinel_t>
    requires is_byte_v<std::ranges::range_value_t<R>>
constexpr BinEncodeResult<std::ranges::borrowed_iterator_t<R>, O> bin_encode(
    R &&r, O out, B bound = {})
{
    return bin_encode(
        std::ranges::begin(r), std::ranges::end(r), std::move(out),
        std::move(bound));
}

template <std::input_iterator I, std::sentinel_for<I> S,
          std::output_iterator<std::uint8_t> O,
          std::sentinel_for<O> B = std::unreachable_sentinel_t>
    requires is_char_v<std::iter_value_t<I>>
constexpr BinDecodeResult<I, O> bin_decode(I first, S last, O out, B bound = {})
{
    std::errc err = detail::try_bin_decode(first, last, out, bound);
    return { std::move(first), std::move(out), std::make_error_code(err) };
}

template <std::ranges::input_range R, std::output_iterator<std::uint8_t> O,
          std::sentinel_for<O> B = std::unreachable_sentinel_t>
    requires is_char_v<std::ranges::range_value_t<R>>
constexpr BinDecodeResult<std::ranges::borrowed_iterator_t<R>, O> bin_decode(
    R &&r, O out, B bound = {})
{
    return bin_decode(
        std::ranges::begin(r), std::ranges::end(r), std::move(out),
        std::move(bound));
}

template <std::input_iterator I, std::sentinel_for<I> S,
          std::output_iterator<char> O,
          std::sentinel_for<O> B = std::unreachable_sentinel_t>
    requires is_byte_v<std::iter_value_t<I>>
constexpr HexEncodeResult<I, O> hex_encode(I first, S last, O out, B bound = {})
{
    std::errc err = detail::try_hex_encode(first, last, out, bound);
    return { std::move(first), std::move(out), std::make_error_code(err) };
}

template <std::ranges::input_range R, std::output_iterator<char> O,
          std::sentinel_for<O> B = std::unreachable_sentinel_t>
    requires is_byte_v<std::ranges::range_value_t<R>>
constexpr HexEncodeResult<std::ranges::borrowed_iterator_t<R>, O> hex_encode(
    R &&r, O out, B bound = {})
{
    return hex_encode(
        std::ranges::begin(r), std::ranges::end(r), std::move(out),
        std::move(bound));
}

template <std::input_iterator I, std::sentinel_for<I> S,
          std::output_iterator<std::uint8_t> O,
          std::sentinel_for<O> B = std::unreachable_sentinel_t>
    requires is_char_v<std::iter_value_t<I>>
constexpr HexDecodeResult<I, O> hex_decode(I first, S last, O out, B bound = {})
{
    std::errc err = detail::try_hex_decode(first, last, out, bound);
    return { std::move(first), std::move(out), std::make_error_code(err) };
}

template <std::ranges::input_range R, std::output_iterator<std::uint8_t> O,
          std::sentinel_for<O> B = std::unreachable_sentinel_t>
    requires is_char_v<std::ranges::range_value_t<R>>
constexpr HexDecodeResult<std::ranges::borrowed_iterator_t<R>, O> hex_decode(
    R &&r, O out, B bound = {})
{
    return hex_decode(
        std::ranges::begin(r), std::ranges::end(r), std::move(out),
        std::move(bound));
}

// template <std::input_iterator I, std::sentinel_for<I> S,
//           std::output_iterator<char> O,
//           std::sentinel_for<O> B = std::unreachable_sentinel_t>
// constexpr Base64EncodeResult<I, O>
// base64_encode(I first, S last, O out, B bound = {})
// {
//     return { std::move(first), std::move(last),
//              detail::try_base64_encode(
//                  first, last, out, bound, detail::codec_base64_charset) };
// }

// template <std::ranges::input_range R, std::output_iterator<char> O,
//           std::sentinel_for<O> B = std::unreachable_sentinel_t>
// constexpr Base64EncodeResult<std::ranges::borrowed_iterator_t<R>, O>
// base64_encode(R &&r, O out, B bound = {})
// {
//     return base64_encode(
//         std::ranges::begin(r), std::ranges::end(r), std::move(out),
//         std::move(bound));
// }

// template <std::input_iterator I, std::sentinel_for<I> S,
//           std::output_iterator<std::uint8_t> O,
//           std::sentinel_for<O> B = std::unreachable_sentinel_t>
// constexpr Base64DecodeResult<I, O>
// base64_decode(I first, S last, O out, B bound = {})
// {
//     return { std::move(first), std::move(last),
//              detail::try_base64_decode(
//                  first, last, out, bound, detail::codec_base64_values) };
// }

// template <std::ranges::input_range R, std::output_iterator<std::uint8_t> O,
//           std::sentinel_for<O> B = std::unreachable_sentinel_t>
// constexpr Base64DecodeResult<std::ranges::borrowed_iterator_t<R>, O>
// base64_decode(R &&r, O out, B bound = {})
// {
//     return base64_decode(
//         std::ranges::begin(r), std::ranges::end(r), std::move(out),
//         std::move(bound));
// }

// template <std::input_iterator I, std::sentinel_for<I> S,
//           std::output_iterator<char> O,
//           std::sentinel_for<O> B = std::unreachable_sentinel_t>
// constexpr Base64URLEncodeResult<I, O>
// base64url_encode(I first, S last, O out, B bound = {})
// {
//     return { std::move(first), std::move(last),
//              detail::try_base64_encode(
//                  first, last, out, bound, detail::codec_base64url_charset) };
// }

// template <std::ranges::input_range R, std::output_iterator<char> O,
//           std::sentinel_for<O> B = std::unreachable_sentinel_t>
// constexpr Base64URLEncodeResult<std::ranges::borrowed_iterator_t<R>, O>
// base64url_encode(R &&r, O out, B bound = {})
// {
//     return base64url_encode(
//         std::ranges::begin(r), std::ranges::end(r), std::move(out),
//         std::move(bound));
// }

// template <std::input_iterator I, std::sentinel_for<I> S,
//           std::output_iterator<std::uint8_t> O,
//           std::sentinel_for<O> B = std::unreachable_sentinel_t>
// constexpr Base64URLDecodeResult<I, O>
// base64url_decode(I first, S last, O out, B bound = {})
// {
//     return { std::move(first), std::move(last),
//              detail::try_base64_decode(
//                  first, last, out, bound, detail::codec_base64url_values) };
// }

// template <std::ranges::input_range R, std::output_iterator<std::uint8_t> O,
//           std::sentinel_for<O> B = std::unreachable_sentinel_t>
// constexpr Base64URLDecodeResult<std::ranges::borrowed_iterator_t<R>, O>
// base64url_decode(R &&r, O out, B bound = {})
// {
//     return base64url_decode(
//         std::ranges::begin(r), std::ranges::end(r), std::move(out),
//         std::move(bound));
// }

} // namespace hlib

#endif

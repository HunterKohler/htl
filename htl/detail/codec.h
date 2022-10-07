#ifndef HLIB_DETAIL_CODEC_H_
#define HLIB_DETAIL_CODEC_H_

#include <cstdint>
#include <system_error>
#include <vector>
#include <string>

namespace hlib::detail {

inline constexpr char codec_hex_charset[]{ "0123456789ABCDEF" };

inline constexpr char codec_base64_charset[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

inline constexpr char codec_base64url_charset[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

inline constexpr std::uint8_t codec_hex_values[256]{
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 0,  1,  2,  3,  4,  5,  6,  7,  8,
    9,  16, 16, 16, 16, 16, 16, 16, 10, 11, 12, 13, 14, 15, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 10, 11, 12, 13, 14, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16, 16
};

inline constexpr std::uint8_t codec_base64_values[256]{
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 62, 64, 64, 64, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60,
    61, 64, 64, 64, 64, 64, 64, 64, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64,
    64, 64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64,
};

inline constexpr std::uint8_t codec_base64url_values[256]{
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 52, 53, 54, 55, 56, 57, 58, 59, 60,
    61, 64, 64, 64, 64, 64, 64, 64, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
    11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64,
    63, 64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64,
};

template <std::input_iterator I>
constexpr std::iter_reference_t<I> codec_read(I &it)
{
    std::iter_reference_t<I> value(*it);
    ++it;
    return value;
}

template <std::input_iterator I>
constexpr std::uint8_t codec_read_byte(I &it)
{
    return static_cast<std::uint8_t>(codec_read(it));
}

template <std::input_iterator I>
constexpr char codec_read_char(I &it)
{
    return static_cast<char>(codec_read(it));
}

template <class T, std::output_iterator<T> O>
constexpr void codec_write(T &&value, O &out)
{
    *out = std::forward<T>(value);
    ++out;
}

template <class T, std::output_iterator<std::uint8_t> O>
constexpr void codec_write_byte(T &&value, O &out)
{
    codec_write(static_cast<std::uint8_t>(std::forward<T>(value)), out);
}

template <class T, std::output_iterator<std::uint8_t> O>
constexpr void codec_write_char(T &&value, O &out)
{
    codec_write(static_cast<char>(std::forward<T>(value)), out);
}

template <class I, class S, class O, class B>
constexpr std::errc try_hex_encode(I &first, S &last, O &out, B &bound)
{
    for (; first != last;) {
        std::uint8_t value = codec_read_byte(first);

        if (bound == out) {
            return std::errc::value_too_large;
        }

        codec_write_char(codec_hex_charset[value >> 4], out);

        if (bound == out) {
            return std::errc::value_too_large;
        }

        codec_write_char(codec_hex_charset[value & 15], out);
    }

    return {};
}

template <class I, class S, class O, class B>
constexpr std::errc try_hex_decode(I &first, S &last, O &out, B &bound)
{
    for (; first != last;) {
        std::uint8_t a = codec_hex_values[to_unsigned(codec_read_char(first))];

        if (a >= 16) {
            break;
        } else if (out == bound) {
            return std::errc::value_too_large;
        } else if (first == last) {
            codec_write_byte(a << 4, out);
            break;
        }

        std::uint8_t b = codec_hex_values[to_unsigned(codec_read_char(first))];

        if (b >= 16) {
            codec_write_byte(a << 4, out);
            break;
        }

        codec_write_byte(a << 4 | b, out);
    }

    return {};
}

template <class I, class S, class O, class B>
constexpr std::errc try_bin_encode(I &first, S &last, O &out, B &bound)
{
    for (; first != last;) {
        std::uint8_t value = codec_read_byte(first);

        for (int i = 7; i >= 0; --i) {
            if (out == bound) {
                return std::errc::value_too_large;
            }

            codec_write_char(((value >> i) & 1) + '0', out);
        }
    }

    return {};
}

template <class I, class S, class O, class B>
constexpr std::errc try_bin_decode(I &first, S &last, O &out, B &bound)
{
    for (; first != last;) {
        std::uint8_t value = 0;

        for (int i = 7; i >= 0 && first != last; --i) {
            int c = codec_read_char(first) - '0';

            if (c == 1) {
                value |= 1 << i;
            } else if (c != 0) {
                if (out == bound) {
                    return std::errc::value_too_large;
                }

                codec_write_byte(value, out);
                return {};
            }
        }

        if (out == bound) {
            return std::errc::value_too_large;
        }

        codec_write_byte(value, out);
    }

    return {};
}

// template <class I, class S, class O, class B>
// constexpr std::errc
// try_base64_encode(I &first, S &last, O &out, B &bound, const char *charset)
// {
//     for (; first != last;) {
//         std::uint8_t a = codec_read_byte(first);

//         if (out == bound) {
//             return std::errc::value_too_large;
//         }

//         codec_write_char(charset[a >> 2], out);

//         if (first == last) {
//             if (out == bound) {
//                 return std::errc::value_too_large;
//             }

//             codec_write_char(charset[(a & 3) << 4]);
//             break;
//         }

//         std::uint8_t b = codec_read_byte(first);

//         if (out == bound) {
//             return std::errc::value_too_large;
//         }
//         codec_write_char(charset[(a & 3) << 4 | b >> 4], out);

//         if (first == last) {
//             if (out == bound) {
//                 return std::errc::value_too_large;
//             }
//             codec_write_char(charset[(b & 15) << 2], out);
//             break;
//         }

//         std::uint8_t c = codec_read_byte(first);

//         if (out == bound) {
//             return std::errc::value_too_large;
//         }

//         codec_write_char(out, charset[(b & 15) << 2 | c >> 6]);

//         if (out == bound) {
//             return std::errc::value_too_large;
//         }

//         codec_write_char(out, charset[c & 63]);
//     }
// }

// template <class I, class S, class O, class B>
// constexpr std::errc try_base64_decode(
//     I &first, S &last, O &out, B &bound, const std::uint8_t *table)
// {}

} // namespace hlib::detail

#endif

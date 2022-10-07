#ifndef HLIB_ASCII_H_
#define HLIB_ASCII_H_

#include <cassert>
#include <cstdint>
#include <hlib/utility.h>

namespace hlib {
namespace detail {

enum class ascii_property : std::uint8_t {
    isalnum = 1,
    isalpha = 2,
    isblank = 4,
    iscntrl = 8,
    ispunct = 16,
    isspace = 32,
    isxdigit = 64,
};

inline constexpr std::uint8_t ascii_property_table[256]{
    8,  8,  8,  8,  8,  8,  8,  8,  8,  44, 40, 40, 40, 40, 8,  8,  8,  8,  8,
    8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  36, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 65, 65, 65, 65, 65, 65, 65, 65, 65,
    65, 16, 16, 16, 16, 16, 16, 16, 67, 67, 67, 67, 67, 67, 3,  3,  3,  3,  3,
    3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  16, 16, 16, 16,
    16, 16, 67, 67, 67, 67, 67, 67, 3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
    3,  3,  3,  3,  3,  3,  3,  3,  3,  16, 16, 16, 16, 8,
};

inline constexpr std::uint8_t ascii_tolower_table[256]{
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,
    15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
    30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,
    45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
    60,  61,  62,  63,  64,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106,
    107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121,
    122, 91,  92,  93,  94,  95,  96,  97,  98,  99,  100, 101, 102, 103, 104,
    105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 123, 124, 125, 126, 127,
};

inline constexpr std::uint8_t ascii_toupper_table[256]{
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,  12,  13,  14,  15,
    16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,  28,  29,  30,  31,
    32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43,  44,  45,  46,  47,
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59,  60,  61,  62,  63,
    64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75,  76,  77,  78,  79,
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91,  92,  93,  94,  95,
    96, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75,  76,  77,  78,  79,
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 123, 124, 125, 126, 127,
};

constexpr bool ascii_get_property(std::uint8_t c, ascii_property prop) noexcept
{
    return ascii_property_table[c] & to_underlying(prop);
}

} // namespace detail

constexpr char ascii_isascii(char c) noexcept
{
    return static_cast<unsigned char>(c) < 128;
}

constexpr bool ascii_isdigit(char c) noexcept
{
    assert(ascii_isascii(c));
    return '0' <= c && c <= '9';
}

constexpr bool ascii_islower(char c) noexcept
{
    assert(ascii_isascii(c));
    return 'a' <= c && c <= 'z';
}

constexpr bool ascii_isupper(char c) noexcept
{
    assert(ascii_isascii(c));
    return 'A' <= c && c <= 'Z';
}

constexpr bool ascii_isgraph(char c) noexcept
{
    assert(ascii_isascii(c));
    return c > 32 && c < 127;
}

constexpr bool ascii_isprint(char c) noexcept
{
    assert(ascii_isascii(c));
    return c >= 32 && c < 127;
}

constexpr bool ascii_isalnum(char c) noexcept
{
    assert(ascii_isascii(c));
    return detail::ascii_get_property(c, detail::ascii_property::isalnum);
}

constexpr bool ascii_isalpha(char c) noexcept
{
    assert(ascii_isascii(c));
    return detail::ascii_get_property(c, detail::ascii_property::isalpha);
}

constexpr bool ascii_isblank(char c) noexcept
{
    assert(ascii_isascii(c));
    return detail::ascii_get_property(c, detail::ascii_property::isblank);
}

constexpr bool ascii_iscntrl(char c) noexcept
{
    assert(ascii_isascii(c));
    return detail::ascii_get_property(c, detail::ascii_property::iscntrl);
}

constexpr bool ascii_ispunct(char c) noexcept
{
    assert(ascii_isascii(c));
    return detail::ascii_get_property(c, detail::ascii_property::ispunct);
}

constexpr bool ascii_isspace(char c) noexcept
{
    assert(ascii_isascii(c));
    return detail::ascii_get_property(c, detail::ascii_property::isspace);
}

constexpr bool ascii_isxdigit(char c) noexcept
{
    assert(ascii_isascii(c));
    return detail::ascii_get_property(c, detail::ascii_property::isxdigit);
}

constexpr char ascii_tolower(char c) noexcept
{
    assert(ascii_isascii(c));
    return detail::ascii_tolower_table[to_unsigned(c)];
}

constexpr char ascii_toupper(char c) noexcept
{
    assert(ascii_isascii(c));
    return detail::ascii_toupper_table[to_unsigned(c)];
}

} // namespace hlib

#endif

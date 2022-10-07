#ifndef HTL_ASCII_H_
#define HTL_ASCII_H_

#include <cassert>
#include <cstdint>
#include <htl/utility.h>
#include <htl/detail/ascii.h>

namespace htl {

/**
 * Checks if the given character is ascii.
 */
constexpr char ascii_isascii(char c) noexcept
{
    return static_cast<unsigned char>(c) < 128;
}

/**
 * Checks if the given ascii character is a digit.
 */
constexpr bool ascii_isdigit(char c) noexcept
{
    assert(ascii_isascii(c));
    return '0' <= c && c <= '9';
}

/**
 * Checks if the given ascii haracter is a lowercase letter.
 */
constexpr bool ascii_islower(char c) noexcept
{
    assert(ascii_isascii(c));
    return 'a' <= c && c <= 'z';
}

/**
 * Checks if the given ascii haracter is an uppercase letter.
 */
constexpr bool ascii_isupper(char c) noexcept
{
    assert(ascii_isascii(c));
    return 'A' <= c && c <= 'Z';
}

/**
 * Checks if the given ascii character is graphic.
 */
constexpr bool ascii_isgraph(char c) noexcept
{
    assert(ascii_isascii(c));
    return c > 32 && c < 127;
}

/**
 * Checks if the given ascii character is printable.
 */
constexpr bool ascii_isprint(char c) noexcept
{
    assert(ascii_isascii(c));
    return c >= 32 && c < 127;
}

/**
 * Checks if the given ascii character is alphanumeric.
 */
constexpr bool ascii_isalnum(char c) noexcept
{
    assert(ascii_isascii(c));
    return detail::ascii_get_property(c, detail::ascii_property::isalnum);
}

/**
 * Checks if the given ascii character is alphabetical.
 */
constexpr bool ascii_isalpha(char c) noexcept
{
    assert(ascii_isascii(c));
    return detail::ascii_get_property(c, detail::ascii_property::isalpha);
}

/**
 * Checks if the given ascii character is a blank.
 */
constexpr bool ascii_isblank(char c) noexcept
{
    assert(ascii_isascii(c));
    return detail::ascii_get_property(c, detail::ascii_property::isblank);
}

/**
 * Checks if the given ascii character is a control character.
 */
constexpr bool ascii_iscntrl(char c) noexcept
{
    assert(ascii_isascii(c));
    return detail::ascii_get_property(c, detail::ascii_property::iscntrl);
}

/**
 * Checks if the given ascii character is punctuation.
 */
constexpr bool ascii_ispunct(char c) noexcept
{
    assert(ascii_isascii(c));
    return detail::ascii_get_property(c, detail::ascii_property::ispunct);
}

/**
 * Checks if the given ascii character is a space.
 */
constexpr bool ascii_isspace(char c) noexcept
{
    assert(ascii_isascii(c));
    return detail::ascii_get_property(c, detail::ascii_property::isspace);
}

/**
 * Checks if the given ascii character is a hexidecimal digit.
 */
constexpr bool ascii_isxdigit(char c) noexcept
{
    assert(ascii_isascii(c));
    return detail::ascii_get_property(c, detail::ascii_property::isxdigit);
}

/**
 * Converts the ascii character to its lowercase representation if it is
 * a letter.
 */
constexpr char ascii_tolower(char c) noexcept
{
    assert(ascii_isascii(c));
    return detail::ascii_tolower_table[to_unsigned(c)];
}

/**
 * Converts the ascii character to its uppercase representation if it is
 * a letter.
 */
constexpr char ascii_toupper(char c) noexcept
{
    assert(ascii_isascii(c));
    return detail::ascii_toupper_table[to_unsigned(c)];
}

} // namespace htl

#endif

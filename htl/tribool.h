#ifndef HLIB_TRIBOOL_H_
#define HLIB_TRIBOOL_H_

#include <compare>
#include <cstddef>
#include <iostream>
#include <memory>
#include <string_view>
#include <string>

namespace hlib {

class TriBool {
public:
    enum class State : std::uint8_t {
        Null,
        False,
        True,
    };

    State _state;

public:
    constexpr TriBool() noexcept = default;

    constexpr TriBool(std::nullptr_t) noexcept : TriBool() {}

    constexpr TriBool(bool state) noexcept
        : _state(static_cast<State>(state + 1))
    {}

    constexpr bool operator!() const noexcept { return _state == State::False; }

    constexpr explicit operator bool() const noexcept
    {
        return _state == State::True;
    }

    friend constexpr bool operator==(TriBool, TriBool) noexcept = default;

    friend constexpr std::strong_ordering operator<=>(TriBool, TriBool) //
        noexcept = default;
};

namespace detail {

inline std::string_view get_tribool_string(TriBool value) noexcept
{
    return value ? "true" : !value ? "false" : "null";
}

} // namespace detail

template <class Alloc = std::allocator<char>>
inline std::basic_string<char, std::char_traits<char>, Alloc>
to_string(TriBool value, const Alloc &alloc = Alloc())
{
    return std::basic_string<char, std::char_traits<char>, Alloc>(
        detail::get_tribool_string(value), alloc);
}

template <class CharT, class Traits>
inline std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os, TriBool value)
{
    os << detail::get_tribool_string(value);
    return os;
}

} // namespace hlib

#endif

#ifndef HLIB_JSON_TYPE_H_
#define HLIB_JSON_TYPE_H_

#include <cstddef>
#include <iostream>
#include <iterator>
#include <string>
#include <hlib/json/common.h>
#include <hlib/utility.h>

namespace hlib::json {

enum class Type : std::uint8_t {
    Null,
    Bool,
    Int,
    Float,
    String,
    Array,
    Object,
};

namespace detail {

constexpr const char *get_type_name(Type type)
{
    switch (type) {
    case Type::Null:
        return "Null";
    case Type::Bool:
        return "Bool";
    case Type::Int:
        return "Int";
    case Type::Float:
        return "Float";
    case Type::String:
        return "String";
    case Type::Array:
        return "Array";
    case Type::Object:
        return "Object";
    default:
        throw std::invalid_argument(
            "Invalid underlying value for hlib::json::Type");
    }
}

} // namespace detail

template <std::output_iterator<char> O>
constexpr O to_chars(Type type, O out)
{
    for (auto name = detail::get_type_name(type); *name; ++name) {
        *out = *name;
        ++out;
    }

    return out;
}

template <class Alloc>
constexpr std::basic_string<char, std::char_traits<char>, Alloc>
to_string(Type x, const Alloc &alloc = Alloc{})
{
    return { detail::get_type_name(x), alloc };
}

template <class CharT, class Traits>
std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os, Type type)
{
    to_chars(type, std::ostream_iterator<char>(os));
    return os;
}

} // namespace hlib::json

#endif

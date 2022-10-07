#ifndef HLIB_TYPE_TRAITS_H_
#define HLIB_TYPE_TRAITS_H_

#include <type_traits>

namespace hlib {

template <class T>
struct is_char : std::false_type {};

template <class T>
struct is_char<const T> : is_char<T> {};

template <class T>
struct is_char<volatile T> : is_char<T> {};

template <>
struct is_char<char> : std::true_type {};

template <>
struct is_char<wchar_t> : std::true_type {};

template <>
struct is_char<signed char> : std::true_type {};

template <>
struct is_char<unsigned char> : std::true_type {};

template <>
struct is_char<char8_t> : std::true_type {};

template <>
struct is_char<char16_t> : std::true_type {};

template <>
struct is_char<char32_t> : std::true_type {};

template <class T>
struct is_byte : std::false_type {};

template <class T>
struct is_byte<const T> : is_byte<T> {};

template <class T>
struct is_byte<volatile T> : is_byte<T> {};

template <>
struct is_byte<char> : std::true_type {};

template <>
struct is_byte<signed char> : std::true_type {};

template <>
struct is_byte<unsigned char> : std::true_type {};

template <>
struct is_byte<char8_t> : std::true_type {};

template <>
struct is_byte<std::byte> : std::true_type {};

template <class T>
struct is_function_pointer
    : std::conjunction<std::is_pointer<T>,
                       std::is_function<std::remove_pointer_t<T>>> {};

template <class T>
inline constexpr bool is_char_v = is_char<T>::value;

template <class T>
inline constexpr bool is_byte_v = is_byte<T>::value;

template <class T>
inline constexpr bool is_function_pointer_v = is_function_pointer<T>::value;

} // namespace hlib

#endif

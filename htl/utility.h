#ifndef HLIB_UTILITY_H_
#define HLIB_UTILITY_H_

#include <type_traits>
#include <concepts>
#include <bit>

#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

namespace hlib {

template <class... T>
struct Overload : T... {
    using T::operator()...;
};

// C++ 23
template <class E>
constexpr std::underlying_type_t<E> to_underlying(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}

// C++ 23
[[noreturn]] inline void unreachable()
{
#if __has_builtin(__builtin_unreachable)
    __builtin_unreachable();
#elif _MSC_VER
    __assume(false);
#else
    std::abort();
#endif
}

template <class T, class Member>
inline std::ptrdiff_t offset_of(const Member T::*member)
{
    return reinterpret_cast<std::ptrdiff_t>(&reinterpret_cast<T *>(0)->*member);
}

template <class T, class Member>
inline T *container_of(Member *ptr, const Member T::*member)
{
    return reinterpret_cast<T *>(
        reinterpret_cast<std::uintptr_t>(ptr) - offset_of(member));
}

template <std::signed_integral T>
constexpr std::make_unsigned_t<T> to_unsigned(T value) noexcept
{
    return value;
}

template <std::unsigned_integral T>
constexpr std::make_signed_t<T> to_signed(T value) noexcept
{
    return value;
}

template <class T>
constexpr T implicit_cast(std::type_identity_t<T> value) //
{
    return value;
}

} // namespace hlib

#endif

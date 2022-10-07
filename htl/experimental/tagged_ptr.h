#ifndef HLIB_TAGGED_PTR_H_
#define HLIB_TAGGED_PTR_H_

#include <bit>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <type_traits>
#include <hlib/detail/simple_hash.h>

namespace hlib {
namespace detail {

auto get_tagged_ptr_value(auto &ptr)
{
    return ptr._value;
}

} // namespace detail

template <class T>
    requires(sizeof(T) > 1 && alignof(T) > 1)
class TaggedPtr {
public:
    using element_type = std::remove_extent_t<T>;
    using pointer = element_type *;
    using uint_type = std::uintptr_t;

    template <class U>
    using rebind = TaggedPtr<U>;

    static constexpr std::size_t bits = std::countr_zero(alignof(element_type));

private:
    static constexpr uint_type mask = (1 << bits) - 1;

    template <class U>
    static constexpr bool raw_convertible =
        (!std::is_array_v<T> && std::is_convertible_v<U, T *>) ||
        (std::is_unbounded_array_v<T> && std::is_pointer_v<U> &&
         std::is_convertible_v<std::remove_pointer_t<U> (*)[], T *>) ||
        (std::is_bounded_array_v<T> && std::is_pointer_v<U> &&
         std::is_convertible_v<
             std::remove_pointer_t<U> (*)[std::extent_v<T>], T *>);

    template <class U>
    static constexpr bool pointer_convertible =
        std::is_convertible_v<U *, T *> ||
        (std::is_bounded_array_v<U> && std::is_unbounded_array_v<T> &&
         std::is_same_v<std::remove_extent_t<U>,
                        std::remove_cvref_t<std::remove_extent_t<T>>>);

public:
    TaggedPtr() noexcept = default;

    template <class U>
        requires(raw_convertible<U>)
    TaggedPtr(U ptr)
    noexcept(std::is_nothrow_convertible_v<U, pointer>) { reset(ptr); }

    template <class U>
        requires(raw_convertible<U>)
    TaggedPtr(U ptr, uint_type tag)
    noexcept(std::is_nothrow_convertible_v<U, pointer>) { reset(ptr, tag); }

    template <class U>
        requires(pointer_convertible<U>)
    TaggedPtr(const TaggedPtr<U> &other)
    noexcept : _value{ other._value } {}

    TaggedPtr &operator=(std::nullptr_t) noexcept
    {
        _value = 0;
        return *this;
    }

    template <class U>
        requires(pointer_convertible<U>)
    TaggedPtr &operator=(const TaggedPtr<U> &other) noexcept
    {
        _value = other._value;
        return *this;
    }

    void reset() noexcept { _value = 0; }

    void reset(std::nullptr_t) noexcept { _value = 0; }

    void reset(std::nullptr_t, uint_type tag) noexcept { _value = tag & mask; }

    template <class U>
        requires(raw_convertible<U>)
    void reset(U ptr) noexcept(std::is_nothrow_convertible_v<U, pointer>)
    {
        _value = reinterpret_cast<uint_type>(static_cast<pointer>(ptr)) & ~mask;
    }

    template <class U>
        requires(raw_convertible<U>)
    void reset(U ptr, uint_type tag) //
        noexcept(std::is_nothrow_convertible_v<U, pointer>)
    {
        _value =
            (reinterpret_cast<uint_type>(static_cast<pointer>(ptr)) & ~mask) |
            (tag & mask);
    }

    void tag(uint_type new_value) const noexcept
    {
        _value = (_value & ~mask) | (new_value & mask);
    }

    uint_type tag() const noexcept { return _value & mask; }

    pointer get() const noexcept
    {
        return reinterpret_cast<pointer>(_value & ~mask);
    }

    pointer operator->() const noexcept requires(!std::is_array_v<T>)
    {
        return get();
    }

    std::add_lvalue_reference_t<element_type> operator*() const
        requires(!std::is_array_v<T>)
    {
        return *get();
    }

    std::add_lvalue_reference_t<element_type> operator[](std::size_t n) const
        requires(std::is_array_v<T>)
    {
        return get()[n];
    }

    operator bool() const noexcept { return get(); }

private:
    uint_type _value;

    friend auto detail::get_tagged_ptr_value(auto &);
};

template <class CharT, class Traits, class T>
inline std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os, const TaggedPtr<T> &ptr)
{
    os << detail::get_tagged_ptr_value(ptr);
    return os;
}

template <class U, class V>
    requires(std::equality_comparable_with<
             typename TaggedPtr<U>::pointer, typename TaggedPtr<V>::pointer>)
inline bool operator==(const TaggedPtr<U> &a, const TaggedPtr<V> &b) noexcept
{
    return detail::get_tagged_ptr_value(a) == detail::get_tagged_ptr_value(b);
}

template <class U, class V>
    requires(std::three_way_comparable_with<
             typename TaggedPtr<U>::pointer, typename TaggedPtr<V>::pointer>)
inline std::compare_three_way_result_t<
    typename TaggedPtr<U>::pointer, typename TaggedPtr<V>::pointer>
operator<=>(const TaggedPtr<U> &a, const TaggedPtr<V> &b) noexcept
{
    return detail::get_tagged_ptr_value(a) <=> detail::get_tagged_ptr_value(b);
}

} // namespace hlib

template <class T>
struct std::hash<hlib::TaggedPtr<T>> {
    std::size_t operator()(const hlib::TaggedPtr<T> &ptr) const noexcept
    {
        return hlib::detail::simple_hash(
            hlib::detail::get_tagged_ptr_value(ptr));
    }
};

#endif

#ifndef HTL_EXPERIMENTAL_INT128_H_
#define HTL_EXPERIMENTAL_INT128_H_

#include <cstdint>
#include <limits>
#include <htl/math.h>

namespace htl {
namespace detail {

template <std::endian, class HighWord>
struct Int128Storage;

template <class HighWord>
struct Int128Storage<std::endian::little, HighWord> {
    Int128Storage() = default;

    constexpr Int128Storage(HighWord hi, std::uint64_t lo) noexcept
        : hi(hi), lo(lo)
    {}

    std::uint64_t lo;
    HighWord hi;
};

template <class HighWord>
struct Int128Storage<std::endian::big, HighWord> {
    Int128Storage() = default;

    constexpr Int128Storage(HighWord hi, std::uint64_t lo) noexcept
        : hi(hi), lo(lo)
    {}

    HighWord hi;
    std::uint64_t lo;
};

struct UInt128Value : public Int128Storage<std::endian::native, std::uint64_t> {
    using Base = Int128Storage<std::endian::native, HighWord>;
    using reference = const Int128ValueBase &;

    using Base::Base;
    using Base::hi;
    using Base::lo;

    constexpr void add(reference other) noexcept
    {
        hi += other.hi + checked_add(lo, other.lo, lo);
    }

    constexpr void sub(reference other) noexcept
    {
        hi -= other.hi + checked_sub(lo, other.lo, lo);
    }

    constexpr void mul(reference other) noexcept
    {
        std::uint64_t self00 = lo & 0xFFFFFFFF;
        std::uint64_t self32 = lo >> 32;
        std::uint64_t other00 = other.lo & 0xFFFFFFFF;
        std::uint64_t other32 = other.lo >> 32;
        std::uint64_t carry0 = self00 * other32;
        std::uint64_t carry1 = self32 * other00;

        hi = hi * other.lo + //
             lo * other.hi + //
             self32 * other32 + //
             (carry0 >> 32) + //
             (carry1 >> 32);

        lo = self00 * other00 + //
             (carry0 << 32) + //
             (carry1 << 32);
    }

    constexpr void div(reference other) noexcept
    {
        throw std::runtime_error("Not implemented");
    }

    constexpr void shift_left(int n) noexcept
    {
        assert(n >= 0 && n < 128);

        if (n >= 64) {
            hi = lo << (n - 64);
            lo = 0;
        } else if (n > 0) {
            hi = hi << n | lo >> (64 - n);
            lo <<= n;
        }
    }

    constexpr void shift_right(int n) noexcept
    {
        assert(n >= 0 && n < 128);

        if (n >= 64) {
            hi = 0;
            lo = hi >> (n - 64);
        } else if (n > 0) {
            hi >>= n;
            lo = lo >> n | hi << (64 - n);
        }
    }

    constexpr void bitwise_or(reference other) noexcept
    {
        hi |= other.hi;
        lo |= other.lo;
    }

    constexpr void bitwise_and(reference other) noexcept
    {
        hi &= other.hi;
        lo &= other.lo;
    }

    constexpr void bitwise_xor(reference other) noexcept
    {
        hi ^= other.hi;
        lo ^= other.lo;
    }

    constexpr void bitwise_not() noexcept
    {
        hi = ~hi;
        lo = ~lo;
    }

    constexpr void negate() noexcept
    {
        hi = ~hi + !lo;
        lo = ~hi + 1;
    }

    constexpr void assign(HighWord a, unsigned long long lo) noexcept
    {
        lo =
    }

    friend constexpr bool operator==(reference a, reference b) noexcept
    {
        return a.hi == b.hi && a.lo == b.lo;
    }

    friend constexpr std::strong_ordering operator<=>(
        reference a, reference b) noexcept
    {
        return std::tuple(a.hi, a.lo) <=> std::tuple(b.hi, b.lo);
    }
};

struct Int128Value : public Int128ValueBase<std::int64_t> {
    using Base = Int128ValueBase<std::int64_t>;
    using Base::Base;
};

struct UInt128Value : public Int128ValueBase<std::uint64_t> {
    using Base = Int128ValueBase<std::uint64_t>;
    using Base::Base;
};

} // namespace detail

struct Int128;

struct UInt128 {
    constexpr UInt128(int value) noexcept : _value(0, value) {}

    constexpr UInt128(long value) noexcept : _value(0, value) {}

    constexpr UInt128(long long value) noexcept : _value(0, value) {}

    constexpr UInt128(unsigned int value) noexcept : _value(0, value) {}

    constexpr UInt128(unsigned long value) noexcept : _value(0, value) {}

    constexpr UInt128(unsigned long long value) noexcept : _value(0, value) {}

    constexpr UInt128(std::uint64_t hi, std::uint64_t lo) noexcept
        : _value(hi, lo)
    {}

    constexpr UInt128 &operator=(int other) noexcept
    {
        _value.assign(0, other);
        return *this;
    }

    constexpr UInt128 &operator=(long other) noexcept
    {
        _assign(0, other);
        return *this;
    }

    constexpr UInt128 &operator=(long long other) noexcept
    {
        _assign(0, other);
        return *this;
    }

    constexpr UInt128 &operator=(unsigned int other) noexcept
    {
        _assign(0, other);
        return *this;
    }

    constexpr UInt128 &operator=(unsigned long other) noexcept
    {
        _assign(0, other) return *this;
    }

    constexpr UInt128 &operator=(unsigned long long other) noexcept
    {
        _value.in = other;
        return *this;
    }

    constexpr UInt128 &operator+=(UInt128 other) noexcept
    {
        _value.add(other._value);
        return *this;
    }

    constexpr UInt128 &operator-=(UInt128 other) noexcept
    {
        _value.sub(other._value);
        return *this;
    }

    constexpr UInt128 &operator*=(UInt128 other) noexcept
    {
        _value.mul(other._value);
        return *this;
    }

    constexpr UInt128 &operator/=(UInt128 other) noexcept
    {
        _value.div(other._value);
        return *this;
    }

    constexpr UInt128 &operator%=(UInt128 other) noexcept
    {
        _value.rem(other._value);
        return *this;
    }

    constexpr UInt128 &operator^=(UInt128 other) noexcept
    {
        _value.bitwise_xor(other._value);
        return *this;
    }

    constexpr UInt128 &operator&=(UInt128 other) noexcept
    {
        _value.bitwise_and(other._value);
        return *this;
    }

    constexpr UInt128 &operator|=(UInt128 other) noexcept
    {
        _value.bitwise_or(other._value);
        return *this;
    }

    constexpr UInt128 &operator<<=(int n) noexcept
    {
        _value.shift_left(n);
        return *this;
    }

    constexpr UInt128 &operator>>=(int n) noexcept
    {
        _value.shift_right(n);
        return *this;
    }

    constexpr UInt128 &operator++() noexcept
    {
        return *this += 1;
    }

    constexpr UInt128 &operator--() noexcept
    {
        return *this -= 1;
    }

    constexpr UInt128 operator++(int) noexcept
    {
        auto copy(*this);
        ++*this;
        return copy;
    }

    constexpr UInt128 operator--(int) noexcept
    {
        auto copy(*this);
        --*this;
        return copy;
    }

    constexpr UInt128 operator+() noexcept
    {
        return *this;
    }

    constexpr UInt128 operator-() noexcept
    {
        auto copy(*this);
        copy._value.negate();
        return copy;
    }

    constexpr explicit operator bool() const
    {
        return !_value.hi && !_value.lo;
    }

    constexpr explicit operator char() const noexcept
    {
        return static_cast<char>(_value.lo);
    }

    constexpr explicit operator unsigned char() const noexcept
    {
        return static_cast<unsigned char>(_value.lo);
    }

    constexpr explicit operator signed char() const noexcept
    {
        return static_cast<signed char>(_value.lo);
    }

    constexpr explicit operator char8_t() const noexcept
    {
        return static_cast<char8_t>(_value.lo);
    }

    constexpr explicit operator char16_t() const noexcept
    {
        return static_cast<char16_t>(_value.lo);
    }

    constexpr explicit operator char32_t() const noexcept
    {
        return static_cast<char32_t>(_value.lo);
    }

    constexpr explicit operator short() const noexcept
    {
        return static_cast<short>(_value.lo);
    }

    constexpr explicit operator int() const noexcept
    {
        return static_cast<int>(_value.lo);
    }

    constexpr explicit operator long() const noexcept
    {
        return static_cast<long>(_value.lo);
    }

    constexpr explicit operator long long() const noexcept
    {
        return static_cast<long long>(_value.lo);
    }

    constexpr explicit operator unsigned short() const noexcept
    {
        return static_cast<unsigned short>(_value.lo);
    }

    constexpr explicit operator unsigned int() const noexcept
    {
        return static_cast<unsigned int>(_value.lo);
    }

    constexpr explicit operator unsigned long() const noexcept
    {
        return static_cast<unsigned long>(_value.lo);
    }

    constexpr explicit operator unsigned long long() const noexcept
    {
        return static_cast<unsigned long long>(_value.lo);
    }

    friend constexpr UInt128 operator+(UInt128 a, UInt128 b) noexcept
    {
        return a += b;
    }

    friend constexpr UInt128 operator-(UInt128 a, UInt128 b) noexcept
    {
        return a -= b;
    }

    friend constexpr UInt128 operator*(UInt128 a, UInt128 b) noexcept
    {
        return a *= b;
    }

    friend constexpr UInt128 operator/(UInt128 a, UInt128 b) noexcept
    {
        return a /= b;
    }

    friend constexpr UInt128 operator%(UInt128 a, UInt128 b) noexcept
    {
        return a %= b;
    }

    friend constexpr UInt128 operator&(UInt128 a, UInt128 b) noexcept
    {
        return a &= b;
    }

    friend constexpr UInt128 operator^(UInt128 a, UInt128 b) noexcept
    {
        return a ^= b;
    }

    friend constexpr UInt128 operator|(UInt128 a, UInt128 b) noexcept
    {
        return a |= b;
    }

    friend constexpr UInt128 operator<<(UInt128 a, int n) noexcept
    {
        return a <<= n;
    }

    friend constexpr UInt128 operator>>(UInt128 a, int n) noexcept
    {
        return a >>= n;
    }

    friend constexpr bool operator==(UInt128, UInt128) noexcept = default;

    friend constexpr std::strong_ordering operator<=>(
        UInt128, UInt128) noexcept = default;

    template <class CharT, class Traits>
    friend auto &
    operator<<(std::basic_ostream<CharT, Traits> &os, UInt128 value)
    {
        os << "UInt128(" << value._value.hi << "," << value._value.lo << ")";
        return os;
    }

private:
    detail::UInt128Value _value;
};

} // namespace htl

#endif

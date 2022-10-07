#ifndef HLIB_EXPERIMENTAL_BIGINT_H_
#define HLIB_EXPERIMENTAL_BIGINT_H_

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <memory_resource>
#include <limits>
#include <iosfwd>
#include <vector>
#include <hlib/math.h>

namespace hlib {

template <class Allocator>
class BasicBigInt {
private:
    using Vector = std::vector<
        std::uint64_t,
        typename std::allocator_traits<Allocator>::rebind_alloc<std::uint64_t>>;

public:
    using allocator_type = Allocator;

    BasicBigInt() noexcept(noexcept(Allocator())) : _sign(0), _data(1) {}

    explicit BasicBigInt(
        std::integral auto value, const Allocator &alloc = Allocator())
        : _sign(sign(value)), _data(1, alloc)
    {
        _data.push_back(std::abs(value));
    }

    BasicBigInt(const BasicBigInt &other) = default;

    BasicBigInt(const BasicBigInt &other, const Allocator &alloc)
        : _sign(other._sign), _data(other._data, alloc)
    {}

    BasicBigInt(BasicBigInt &&other) noexcept = default;

    BasicBigInt(BasicBigInt &&other, const Allocator &alloc) noexcept(
        std::allocator_traits<Allocator>::is_always_equal::value)
        : _sign(other._sign), _data(std::move(other._data), alloc)
    {}

    ~BasicBigInt() noexcept = default;

    BasicBigInt &operator=(const BasicBigInt &other) = default;

    BasicBigInt &operator=(BasicBigInt &&other) noexcept(
        std::allocator_traits<Allocator>::is_always_equal::value ||
        std::allocator_traits<Allocator>::
            propagate_on_container_move_assignment::value) = default;

    Allocator get_allocator() const noexcept
    {
        return _data.get_allocator();
    }

    void assign(std::integral auto value)
    {
        _sign = sign(value);
        _data.resize(1);
        _data[0] = std::abs(value);
    }

    void shrink_to_fit()
    {
        size_type n = _data.size() - 1;

        for (; !_data[n]; --n) {
        }

        _data.resize(n + 1);
        _data.shrink_to_fit();
    }

    // size_type size();
    // size_type capacity();
    // void reserve(size_type n);
    // void reserve_at_least(size_type n);

    BasicBigInt &operator++()
    {
        return *this += 1;
    }

    BasicBigInt &operator--()
    {
        return *this -= 1;
    }

    BasicBigInt operator++(int)
    {
        auto copy(*this);
        ++*this;
        return copy;
    }

    BasicBigInt operator--(int)
    {
        auto copy(*this);
        --*this;
        return copy;
    }

    BasicBigInt &operator+=(std::integral auto value);

    BasicBigInt &operator-=(std::integral auto value);

    BasicBigInt &operator*=(std::integral auto value);

    BasicBigInt &operator/=(std::integral auto value);

    BasicBigInt &operator%=(std::integral auto value);

    BasicBigInt &operator&=(std::integral auto value)
    {
        _data.resize(1);
        _data[0] &= value;
        return *this;
    }

    BasicBigInt &operator^=(std::integral auto value)
    {
        _data[0] ^= value;
        return *this;
    }

    BasicBigInt &operator|=(std::integral auto value)
    {
        _data[0] |= value;
        return *this;
    }

    BasicBigInt &operator+=(const BasicBigInt &other);

    BasicBigInt &operator-=(const BasicBigInt &other);

    BasicBigInt &operator*=(const BasicBigInt &other);

    BasicBigInt &operator/=(const BasicBigInt &other);

    BasicBigInt &operator%=(const BasicBigInt &other);

    BasicBigInt &operator&=(const BasicBigInt &other)
    {
        _data.resize(std::min(_data.size(), other._data.size()));

        for (int i = 0; i < _data.size(); i++) {
            _data[i] &= other._data[i];
        }

        return *this;
    }

    BasicBigInt &operator^=(const BasicBigInt &other)
    {
        _data.resize(std::max(_data.size(), other._data.size()));

        for (int i = 0; i < _data.size(); i++) {
            _data[i] ^= other._data[i];
        }

        return *this;
    }

    BasicBigInt &operator|=(const BasicBigInt &other)
    {
        _data.resize(std::max(_data.size(), other._data.size()));

        for (int i = 0; i < _data.size(); i++) {
            _data[i] |= other._data[i];
        }

        return *this;
    }

    BasicBigInt &operator<<=(int n)
    {
        if (n > 0) {
            _left_shift(n);
        } else if (n < 0) {
            _right_shift(n);
        }

        return *this;
    }

    BasicBigInt &operator>>=(int n)
    {
        if (n > 0) {
            _right_shift(n);
        } else if (n < 0) {
            _left_shift(n);
        }

        return *this;
    }

    BasicBigInt operator+() const noexcept
    {
        return *this;
    }

    BasicBigInt operator-() const noexcept
    {
        auto copy(*this);
        _sign *= -1;
        return *this;
    }

    // BasicBigInt operator~() const noexcept
    // {
    //     auto copy(*this);
    //     copy.bit_not();
    //     return copy;
    // }

    bool operator!() const noexcept
    {
        return *this == 0;
    }

    explicit operator bool() const noexcept
    {
        return *this != 0;
    }

    friend BasicBigInt operator+(const BasicBigInt &a, const BasicBigInt &b)
    {
        return BasicBigInt(a) += b;
    }

    friend BasicBigInt operator-(const BasicBigInt &a, const BasicBigInt &b)
    {
        return BasicBigInt(a) -= b;
    }

    friend BasicBigInt operator*(const BasicBigInt &a, const BasicBigInt &b)
    {
        return BasicBigInt(a) *= b;
    }

    friend BasicBigInt operator/(const BasicBigInt &a, const BasicBigInt &b)
    {
        return BasicBigInt(a) /= b;
    }

    friend BasicBigInt operator%(const BasicBigInt &a, const BasicBigInt &b)
    {
        return BasicBigInt(a) %= b;
    }

    friend BasicBigInt operator&(const BasicBigInt &a, const BasicBigInt &b)
    {
        return BasicBigInt(a) &= b;
    }

    friend BasicBigInt operator^(const BasicBigInt &a, const BasicBigInt &b)
    {
        return BasicBigInt(a) ^= b;
    }

    friend BasicBigInt operator|(const BasicBigInt &a, const BasicBigInt &b)
    {
        return BasicBigInt(a) |= b;
    }

    friend BasicBigInt operator+(const BasicBigInt &a, std::integral auto b)
    {
        return BasicBigInt(a) += b;
    }

    friend BasicBigInt operator-(const BasicBigInt &a, std::integral auto b)
    {
        return BasicBigInt(a) -= b;
    }

    friend BasicBigInt operator*(const BasicBigInt &a, std::integral auto b)
    {
        return BasicBigInt(a) *= b;
    }

    friend BasicBigInt operator/(const BasicBigInt &a, std::integral auto b)
    {
        return BasicBigInt(a) /= b;
    }

    friend BasicBigInt operator%(const BasicBigInt &a, std::integral auto b)
    {
        return BasicBigInt(a) %= b;
    }

    friend BasicBigInt operator&(const BasicBigInt &a, std::integral auto b)
    {
        return BasicBigInt(a) &= b;
    }

    friend BasicBigInt operator^(const BasicBigInt &a, std::integral auto b)
    {
        return BasicBigInt(a) ^= b;
    }

    friend BasicBigInt operator|(const BasicBigInt &a, std::integral auto b)
    {
        return BasicBigInt(a) |= b;
    }

    friend BasicBigInt operator+(std::integral auto a, const BasicBigInt &b)
    {
        return b + a;
    }

    friend BasicBigInt operator-(std::integral auto a, const BasicBigInt &b)
    {
        return b - a;
    }

    friend BasicBigInt operator*(std::integral auto a, const BasicBigInt &b)
    {
        return b * a;
    }

    friend BasicBigInt operator/(std::integral auto a, const BasicBigInt &b)
    {
        return b / a;
    }

    friend BasicBigInt operator%(std::integral auto a, const BasicBigInt &b)
    {
        return b % a;
    }

    friend BasicBigInt operator&(std::integral auto a, const BasicBigInt &b)
    {
        return b & a;
    }

    friend BasicBigInt operator^(std::integral auto a, const BasicBigInt &b)
    {
        return b ^ a;
    }

    friend BasicBigInt operator|(std::integral auto a, const BasicBigInt &b)
    {
        return b | a;
    }

    friend BasicBigInt operator<<(const BasicBigInt &a, int n)
    {
        return BasicBigInt(a) <<= n;
    }

    friend BasicBigInt operator>>(const BasicBigInt &a, int n)
    {
        return BasicBigInt(a) >>= n;
    }

    friend bool operator==(const BasicBigInt &a, std::integral auto b)
    {
        return a._data.size() == 1 && a._data[0] == b &&
               a._sign * a._data[0] == b;
    }

    friend bool operator==(const BasicBigInt &a, const BasicBigInt &b)
    {
        return a._sign == b._sign && a._data == b._data;
    }

    friend std::strong_ordering operator<=>(
        const BasicBigInt &a, std::integral auto b);

    friend std::strong_ordering operator<=>(
        const BasicBigInt &a, const BasicBigInt &b);

    friend void swap(BasicBigInt &a, BasicBigInt &b) noexcept(
        std::allocator_traits<Allocator>::is_always_equal::value ||
        std::allocator_traits<Allocator>::propagate_on_container_swap::value)
    {
        std::ranges::swap(a._data, b._data);
        std::ranges::swap(a._sign, b._sign);
    }

    // template <class I>
    // struct ParseResult {
    //     [[no_unique_address]] I in;
    //     [[no_unique_address]] BasicBigInt value;
    //     std::errc err;
    // };

    // template <std::input_iterator I, std::sentinel_for<I> S>
    // static std::tuple<I, BasicBigInt, std::errc>
    // parse(I first, S last, const Allocator &alloc = Allocator())
    // {}

private:
    std::int8_t _sign;
    Vector _data;

    void _right_shift(int n);

    void _left_shift(int n);

    void _add(const std::uint64_t *data, std::size_t data_size);
    void _sub(const std::uint64_t *data, std::size_t data_size);
    void _mul(const std::uint64_t *data, std::size_t data_size);
    void _div(const std::uint64_t *data, std::size_t data_size);
    void _mod(const std::uint64_t *data, std::size_t data_size);
};

template <class BigIntAlloc, class Alloc = std::allocator<char>>
inline std::basic_string<char, std::char_traits<char>, Alloc>
to_string(const BasicBigInt<BigIntAlloc> &value, const Alloc &alloc = Alloc());

template <class CharT, class Traits, class Alloc>
inline std::basic_ostream<CharT, Traits> &operator<<(
    std::basic_ostream<CharT, Traits> &os, const BasicBigInt<Alloc> &value);

template <class CharT, class Traits, class Alloc>
inline std::basic_istream<CharT, Traits> &
operator>>(std::basic_istream<CharT, Traits> &is, BasicBigInt<Alloc> &value);

using BigInt = BasicBigInt<std::allocator<std::byte>>;

namespace pmr {

using BigInt = BasicBigInt<std::pmr::polymorphic_allocator<std::byte>>;

} // namespace pmr

} // namespace hlib

template <class Alloc>
struct std::hash<hlib::BasicBigInt<Alloc>> {
    std::size_t operator()(const hlib::BasicBigInt<Alloc> &value) noexcept;
};

template <class Alloc>
struct std::numeric_limits<hlib::BasicBigInt<Alloc>> {
    static constexpr bool is_specialized = true;

    static constexpr bool is_signed = true;

    static constexpr bool is_integer = true;

    static constexpr bool is_exact = true;

    static constexpr bool has_infinity = false;

    static constexpr bool has_quiet_NaN = false;

    static constexpr bool has_signaling_NaN = false;

    static constexpr std::float_denorm_style has_denorm = std::denorm_absent;

    static constexpr bool has_denorm_loss = false;

    static constexpr std::float_round_style round_style =
        std::round_toward_zero;

    static constexpr bool is_iec559 = false;

    static constexpr bool is_bounded = false;

    static constexpr bool is_modulo = true;

    static constexpr int digits = 0;

    static constexpr int digits10 = 0;

    static constexpr int max_digits10 = 0;

    static constexpr int radix = 2;

    static constexpr int min_exponent = 0;

    static constexpr int min_exponent10 = 0;

    static constexpr int max_exponent = 0;

    static constexpr int max_exponent10 = 0;

    static constexpr bool traps = std::number_limits<int>::traps;

    static constexpr bool tinyness_before = false;

    static constexpr hlib::BasicBigInt<Alloc> lowest() noexcept
    {
        return {};
    }

    static constexpr hlib::BasicBigInt<Alloc> min() noexcept
    {
        return {};
    }

    static constexpr hlib::BasicBigInt<Alloc> max() noexcept
    {
        return {};
    }

    static constexpr hlib::BasicBigInt<Alloc> epsilon() noexcept
    {
        return {};
    }

    static constexpr hlib::BasicBigInt<Alloc> round_error() noexcept
    {
        return {};
    }

    static constexpr hlib::BasicBigInt<Alloc> infinity() noexcept
    {
        return {};
    }

    static constexpr hlib::BasicBigInt<Alloc> quiet_NaN() noexcept
    {
        return {};
    }

    static constexpr hlib::BasicBigInt<Alloc> signaling_NaN() noexcept
    {
        return {};
    }

    static constexpr hlib::BasicBigInt<Alloc> denorm_min() noexcept
    {
        return {};
    }
};

#endif

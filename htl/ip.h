#ifndef HLIB_IP_H_
#define HLIB_IP_H_

#include <bit>
#include <charconv>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <string_view>
#include <string>
#include <typeinfo>
#include <hlib/detail/ip.h>
#include <hlib/detail/simple_hash.h>
#include <hlib/byte_array.h>
#include <hlib/utility.h>
#include <hlib/math.h>

namespace hlib {

class IPAddress;
class IPv4Address;
class IPv6Address;
class IPv4Network;
class IPv6Network;

using scope_id_type = std::uint32_t;

class IPv4Address {
public:
    using bytes_type = ByteArray<4>;
    using uint_type = std::uint32_t;

    IPv4Address() noexcept : _bytes() {}

    IPv4Address(const bytes_type &value) noexcept : _bytes(value) {}

    explicit IPv4Address(uint_type value) noexcept
    {
        store_unaligned_le32(_bytes.data(), value);
    }

    bytes_type to_bytes() const noexcept
    {
        return _bytes;
    }

    uint_type to_uint() const noexcept
    {
        return load_unaligned_le32(_bytes.data());
    }

    bool is_unspecified() const noexcept
    {
        // 0.0.0.0/32
        return to_uint() == 0x00000000;
    }

    bool is_loopback() const noexcept
    {
        // 127.0.0.0/8
        return (to_uint() & 0xFF000000) == 0x7F000000;
    }

    bool is_multicast() const noexcept
    {
        // 224.0.0.0/4
        return (to_uint() & 0xF0000000) == 0xE0000000;
    }

    bool is_link_local() const noexcept
    {
        // 169.254.0.0/16
        return (to_uint() & 0xFFFF0000) == 0xA9FE0000;
    }

    bool is_private() const noexcept
    {
        uint_type value = to_uint();

        // 10.0.0.0/8
        // 172.16.0.0/12
        // 192.168.0.0/16
        return ((value & 0xFF000000) == 0xA0000000) ||
               ((value & 0xFFF00000) == 0xAC100000) ||
               ((value & 0xFFFF0000) == 0xC0A80000);
    }

    static IPv4Address any() noexcept
    {
        return IPv4Address{ 0x00000000 };
    }

    static IPv4Address loopback() noexcept
    {
        return IPv4Address{ 0x7F000001 };
    }

    friend bool operator==(const IPv4Address &, const IPv4Address &) noexcept =
        default;

    friend std::strong_ordering operator<=>(
        const IPv4Address &, const IPv4Address &) noexcept = default;

    template <class Alloc = std::allocator<char>>
    friend std::basic_string<char, std::char_traits<char>, Alloc>
    to_string(const IPv4Address &value, const Alloc &alloc = Alloc())
    {
        std::basic_string<char, std::char_traits<char>, Alloc> dest(alloc);
        detail::IPStringWriter(std::back_inserter(dest))
            .write_ipv4_address(value);
        return dest;
    }

    template <class CharT, class Traits>
    friend std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os, const IPv4Address &value)
    {
        detail::IPStringWriter(std::ostream_iterator<char>(os))
            .write_ipv4_address(value._bytes.data());
        return os;
    }

    friend std::hash<IPv4Address>;

private:
    bytes_type _bytes;
};

class IPv6Address {
public:
    using bytes_type = ByteArray<16>;

    IPv6Address() noexcept : _bytes() {}

    IPv6Address(const bytes_type &data) noexcept : _bytes(data) {}

    IPv6Address(const bytes_type &data, scope_id_type scope) noexcept
        : _bytes(data), _scope_id(scope)
    {}

    scope_id_type scope_id() const noexcept
    {
        return _scope_id;
    }

    scope_id_type scope_id(scope_id_type new_scope_id) noexcept
    {
        return std::exchange(_scope_id, new_scope_id);
    }

    bytes_type to_bytes() const noexcept
    {
        return _bytes;
    }

    bool is_unspecified() const noexcept
    {
        // ::/128
        return _is_range_zero(16);
    }

    bool is_loopback() const noexcept
    {
        // ::1/128
        return _is_range_zero(15) && _bytes[15] == 1 && !_scope_id;
    }

    bool is_link_local() const noexcept
    {
        // fe80::/10
        return _bytes[0] == 0xFE && (_bytes[1] & 0xC0) == 0x80;
    }

    bool is_site_local() const noexcept
    {
        // fec0::/10
        return _bytes[0] == 0xFE && (_bytes[1] & 0xC0) == 0xC0;
    }

    bool is_v4_mapped() const noexcept
    {
        // ::ffff:0.0.0.0/96
        return _is_range_zero(10) && _bytes[10] == 0xFF && _bytes[11] == 0xFF;
    }

    bool is_v4_compatible() const noexcept
    {
        // ::0.0.0.0/96
        return _is_range_zero(12);
    }

    bool is_multicast() const noexcept
    {
        // ff00::/8
        return _bytes[0] == 0xFF;
    }

    bool is_multicast_node_local() const noexcept
    {
        // ff01::/16
        return is_multicast() && (_bytes[1] & 0x0F) == 0x01;
    }

    bool is_multicast_link_local() const noexcept
    {
        // ff02::/16
        return is_multicast() && (_bytes[1] & 0x0F) == 0x02;
    }

    bool is_multicast_site_local() const noexcept
    {
        // ff05::/16
        return is_multicast() && (_bytes[1] & 0x0F) == 0x05;
    }

    bool is_multicast_org_local() const noexcept
    {
        // ff08::/16
        return is_multicast() && (_bytes[1] & 0x0F) == 0x08;
    }

    bool is_multicast_global() const noexcept
    {
        // ff0e::/16
        return is_multicast() && (_bytes[1] & 0x0F) == 0x0E;
    }

    bool is_private() const noexcept
    {
        if (is_v4_mapped()) {
            return IPv4Address(
                       { _bytes[12], _bytes[13], _bytes[14], _bytes[15] })
                .is_private();
        }

        // fc00::/7
        return _bytes[0] == 0xFC && (_bytes[1] & 0xFE) == 0;
    }

    static IPv6Address any() noexcept
    {
        return {};
    }

    static IPv6Address loopback() noexcept
    {
        return { bytes_type{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 } };
    }

    friend bool operator==(const IPv6Address &, const IPv6Address &) noexcept =
        default;

    friend std::strong_ordering operator<=>(
        const IPv6Address &, const IPv6Address &) noexcept = default;

    template <class Alloc = std::allocator<char>>
    friend std::basic_string<char, std::char_traits<char>, Alloc>
    to_string(const IPv6Address &value, const Alloc &alloc = Alloc())
    {
        std::basic_string<char, std::char_traits<char>, Alloc> dest(alloc);
        detail::IPStringWriter(std::back_inserter(dest))
            .write_ipv6_address(value._bytes.data(), value._scope_id);
        return dest;
    }

    template <class CharT, class Traits>
    friend std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os, const IPv6Address &value)
    {
        detail::IPStringWriter(std::ostream_iterator<char>(os))
            .write_ipv6_address(value._bytes.data(), value._scope_id);
        return os;
    }

    friend std::hash<IPv6Address>;

private:
    bytes_type _bytes;
    scope_id_type _scope_id;

    bool _is_range_zero(int n) const noexcept
    {
        for (; n >= 0; --n) {
            if (_bytes[n]) {
                return false;
            }
        }

        return true;
    }
};

class IPAddress {
public:
    IPAddress() noexcept : _is_v4(true), _v4() {}

    explicit IPAddress(const IPv4Address &value) : _is_v4(true), _v4(value) {}

    explicit IPAddress(const IPv6Address &value) : _is_v4(false), _v6(value) {}

    void assign(const IPv4Address &value) noexcept
    {
        _is_v4 = true;
        _v4 = value;
    }

    void assign(const IPv6Address &value) noexcept
    {
        _is_v4 = false;
        _v6 = value;
    }

    bool is_v4() const noexcept
    {
        return _is_v4;
    }

    bool is_v6() const noexcept
    {
        return !_is_v4;
    }

    IPv4Address to_v4() const
    {
        assert(is_v4());
        return _v4;
    }

    IPv6Address to_v6() const
    {
        assert(is_v6());
        return _v6;
    }

    bool is_unspecified() const noexcept
    {
        return _is_v4 ? _v4.is_unspecified() : _v6.is_unspecified();
    }

    bool is_loopback() const noexcept
    {
        return _is_v4 ? _v4.is_loopback() : _v6.is_loopback();
    }

    bool is_multicast() const noexcept
    {
        return _is_v4 ? _v4.is_multicast() : _v6.is_multicast();
    }

    friend bool operator==(const IPAddress &a, const IPAddress &b) noexcept
    {
        return a.is_v4() == b.is_v4() &&
               ((a.is_v4() && a._v4 == b._v4) ||
                (!a.is_v4() && a._v6 == b._v6));
    }

    friend std::strong_ordering operator<=>(
        const IPAddress &a, const IPAddress &b) noexcept
    {
        if (a.is_v4() == b.is_v4()) {
            return a.is_v4() ? a._v4 <=> b._v4 : a._v6 <=> b._v6;
        } else {
            return a.is_v4() ? std::strong_ordering::less
                             : std::strong_ordering::greater;
        }
    }

    template <class Alloc = std::allocator<char>>
    friend std::basic_string<char, std::char_traits<char>, Alloc>
    to_string(const IPAddress &value, const Alloc &alloc = Alloc())
    {
        return value.is_v4() ? to_string(value._v4) : to_string(value._v6);
    }

    template <class CharT, class Traits>
    friend std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os, const IPAddress &value)
    {
        return value.is_v4() ? os << value._v4 : os << value._v6;
    }

    friend std::hash<IPAddress>;

private:
    bool _is_v4;
    union {
        IPv4Address _v4;
        IPv6Address _v6;
    };
};

template <class>
class BasicIPAddressIterator;

template <>
class BasicIPAddressIterator<IPv4Address> {
public:
    using difference_type = std::ptrdiff_t;
    using value_type = IPv4Address;
    using reference = const IPv4Address &;
    using pointer = const IPv4Address *;
    using iterator_category = std::bidirectional_iterator_tag;
    using iterator_concept = std::bidirectional_iterator_tag;

    BasicIPAddressIterator() = default;

    explicit BasicIPAddressIterator(reference value) : _value(value) {}

    reference operator*() const
    {
        return _value;
    }

    pointer operator->() const
    {
        return std::addressof(_value);
    }

    BasicIPAddressIterator &operator+=(difference_type n)
    {
        _value = IPv4Address(_value.to_uint() + n);
        return *this;
    }

    BasicIPAddressIterator &operator-=(difference_type n)
    {
        _value = IPv4Address(_value.to_uint() - n);
        return *this;
    }

    BasicIPAddressIterator &operator++()
    {
        return *this += 1;
    }

    BasicIPAddressIterator operator++(int)
    {
        auto copy(*this);
        ++*this;
        return copy;
    }

    BasicIPAddressIterator &operator--()
    {
        return *this -= 1;
    }

    BasicIPAddressIterator operator--(int)
    {
        auto copy(*this);
        --*this;
        return copy;
    }

    friend BasicIPAddressIterator operator+(
        const BasicIPAddressIterator &a, difference_type n)
    {
        return BasicIPAddressIterator(a) += n;
    }

    friend BasicIPAddressIterator operator+(
        difference_type n, const BasicIPAddressIterator &a)
    {
        return BasicIPAddressIterator(a) += n;
    }

    friend BasicIPAddressIterator operator-(
        const BasicIPAddressIterator &a, difference_type n)
    {
        return BasicIPAddressIterator(a) -= n;
    }

    friend BasicIPAddressIterator operator-(
        const BasicIPAddressIterator &a, const BasicIPAddressIterator &b)
    {
        return a._value.to_uint() - b._value.to_uint();
    }

private:
    IPv4Address _value;
};

template <>
class BasicIPAddressIterator<IPv6Address> {
public:
    using difference_type = std::ptrdiff_t;
    using value_type = IPv6Address;
    using reference = const IPv6Address &;
    using pointer = const IPv6Address *;
    using iterator_category = std::bidirectional_iterator_tag;
    using iterator_concept = std::bidirectional_iterator_tag;

    BasicIPAddressIterator() = default;

    explicit BasicIPAddressIterator(reference value) : _value(value) {}

    reference operator*() const
    {
        return _value;
    }

    pointer operator->() const
    {
        return std::addressof(_value);
    }

    BasicIPAddressIterator &operator+=(difference_type n);

    BasicIPAddressIterator &operator-=(difference_type n);

    BasicIPAddressIterator &operator++()
    {
        return *this += 1;
    }

    BasicIPAddressIterator operator++(int)
    {
        auto copy(*this);
        ++*this;
        return copy;
    }

    BasicIPAddressIterator &operator--()
    {
        return *this -= 1;
    }

    BasicIPAddressIterator operator--(int)
    {
        auto copy(*this);
        --*this;
        return copy;
    }

    friend BasicIPAddressIterator operator+(
        const BasicIPAddressIterator &a, difference_type n)
    {
        return BasicIPAddressIterator(a) += n;
    }

    friend BasicIPAddressIterator operator+(
        difference_type n, const BasicIPAddressIterator &a)
    {
        return BasicIPAddressIterator(a) += n;
    }

    friend BasicIPAddressIterator operator-(
        const BasicIPAddressIterator &a, difference_type n)
    {
        return BasicIPAddressIterator(a) -= n;
    }

    friend std::ptrdiff_t operator-(
        const BasicIPAddressIterator &a, const BasicIPAddressIterator &b);

private:
    IPv6Address _value;

    auto _data()
    {
        return _value._data.data();
    }
};

using IPv4AddressIterator = BasicIPAddressIterator<IPv4Address>;
using IPv6AddressIterator = BasicIPAddressIterator<IPv6Address>;

template <class Address>
class BasicIPAddressRange {
public:
    using iterator = BasicIPAddressIterator<Address>;

    BasicIPAddressRange(const Address &start, const Address &stop)
        : _start(start), _stop(stop)
    {}

    iterator begin() const noexcept
    {
        return _start;
    }

    iterator end() const noexcept
    {
        return _end;
    }

    std::size_t size() const noexcept
    {
        return end() - begin();
    }

    bool empty() const noexcept
    {
        return !size();
    }

    explicit operator bool() const noexcept
    {
        return size();
    }

private:
    Address _start, _stop;
};

using IPv4AddressRange = BasicIPAddressRange<IPv4Address>;
using IPv6AddressRange = BasicIPAddressRange<IPv6Address>;

class IPv4Network {
public:
    IPv4Network() noexcept : _address(), _prefix_length() {}

    IPv4Network(const IPv4Address &address, int prefix_length)
        : _address(address), _prefix_length(prefix_length)
    {
        if (prefix_length < 0 || prefix_length > 32) {
            throw std::out_of_range(
                "hlib::IPv4Network(): Invalid prefix length");
        }
    }

    IPv4Network(const IPv4Address &address, const IPv4Address &mask)
        : _address(address)
    {
        auto value = mask.to_uint();
        auto bits = std::popcount(value);

        if (bits != 32 && value << bits) {
            throw std::invalid_argument(
                "hlib::IPv4Network(): Invalid network mask");
        }

        _prefix_length = bits;
    }

    IPv4Address address() const noexcept
    {
        return _address;
    }

    std::size_t prefix_length() const noexcept
    {
        return _prefix_length;
    }

    IPv4Address netmask() const noexcept
    {
        return IPv4Address(_get_prefix_mask());
    }

    IPv4Address network() const noexcept
    {
        return IPv4Address(_address.to_uint() & _get_prefix_mask());
    }

    IPv4Address broadcast() const noexcept
    {
        return IPv4Address(
            (_address.to_uint() & _get_prefix_mask()) | !_get_prefix_mask());
    }

    IPv4Network canonical() const noexcept
    {
        return IPv4Network(network(), _prefix_length);
    }

    // address_v4_range hosts() const noexcept;
    // constexpr bool is_host() const noexcept;
    // constexpr bool is_subnet_of(const network_v4& other) const noexcept;

    friend bool operator==(const IPv4Network &, const IPv4Network &) noexcept =
        default;

    template <class Alloc = std::allocator<char>>
    friend std::basic_string<char, std::char_traits<char>, Alloc>
    to_string(const IPv4Network &value, const Alloc &alloc = Alloc())
    {
        std::basic_string<char, std::char_traits<char>, Alloc> dest(alloc);
        detail::IPStringWriter(std::back_inserter(dest))
            .write_ipv4_network(value);
        return dest;
    }

    template <class CharT, class Traits>
    friend std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os, const IPv4Network &value)
    {
        detail::IPStringWriter(std::ostream_iterator<char>(os))
            .write_ipv4_network(
                value._address._bytes.data(), value._prefix_length);
        return os;
    }

    friend std::hash<IPv4Network>;

private:
    IPv4Address _address;
    std::uint8_t _prefix_length;

    IPv4Address::uint_type _get_prefix_mask() const noexcept
    {
        return ~(~0 >> _prefix_length);
    }
};

class IPv6Network {
public:
    IPv6Network() noexcept : _address(), _prefix_length() {}

    IPv6Network(const IPv6Address &address, int prefix_length)
        : _address(address), _prefix_length(prefix_length)
    {
        if (prefix_length > 128) {
            throw std::out_of_range(
                "hlib::IPv4Network(): Invalid prefix length");
        }
    }

    IPv6Address address() const noexcept
    {
        return _address;
    }

    int prefix_length() const noexcept
    {
        return _prefix_length;
    }

    IPv6Address network() const noexcept;

    IPv6Address netmask() const noexcept;

    friend bool operator==(const IPv6Network &, const IPv6Network &) noexcept =
        default;

    template <class Alloc = std::allocator<char>>
    friend std::basic_string<char, std::char_traits<char>, Alloc>
    to_string(const IPv6Network &value, const Alloc &alloc = Alloc())
    {
        std::basic_string<char, std::char_traits<char>, Alloc> dest(alloc);
        detail::IPStringWriter(std::back_inserter(dest))
            .write_ipv6_network(value);
        return dest;
    }

    template <class CharT, class Traits>
    friend std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os, const IPv6Network &value)
    {
        detail::IPStringWriter(std::ostream_iterator<char>(os))
            .write_ipv6_network(value);
        return os;
    }

    friend std::hash<IPv6Network>;

private:
    IPv6Address _address;
    std::uint8_t _prefix_length;
};

} // namespace hlib

template <>
struct std::hash<hlib::IPv4Address> {
    std::size_t operator()(const hlib::IPv4Address &value) noexcept
    {
        return hlib::detail::simple_hash(value._bytes);
    }
};

template <>
struct std::hash<hlib::IPv6Address> {
    std::size_t operator()(const hlib::IPv6Address &value) noexcept
    {
        return hlib::detail::simple_hash(value._bytes, value._scope_id);
    }
};

template <>
struct std::hash<hlib::IPAddress> {
    std::size_t operator()(const hlib::IPAddress &value) noexcept
    {
        return value.is_v4()
                   ? hlib::detail::simple_hash(
                         true, std::hash<hlib::IPv4Address>{}(value._v4))
                   : hlib::detail::simple_hash(
                         false, std::hash<hlib::IPv6Address>{}(value._v6));
    }
};

template <>
struct std::hash<hlib::IPv4Network> {
    std::size_t operator()(const hlib::IPv4Network &value) noexcept
    {
        return hlib::detail::simple_hash(value._address, value._prefix_length);
    }
};

template <>
struct std::hash<hlib::IPv6Network> {
    std::size_t operator()(const hlib::IPv6Network &value) noexcept
    {
        return hlib::detail::simple_hash(value._address, value._prefix_length);
    }
};

#endif

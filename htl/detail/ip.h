#ifndef HLIB_DETAIL_IP_H_
#define HLIB_DETAIL_IP_H_

#include <algorithm>
#include <system_error>
#include <charconv>
#include <span>
#include <limits>
#include <net/if.h>
#include <hlib/ascii.h>
#include <hlib/writer.h>
#include <hlib/scope_guard.h>

namespace hlib::detail {

inline constexpr std::string_view ip_hex_charset = "0123456789abcdef";

template <std::output_iterator<char> O>
class IPStringWriter : public StringWriter<O> {
private:
    using Base = StringWriter<O>;

public:
    using Base::Base;
    using Base::write;
    using Base::write_int;

    void write_ipv4_address(const std::uint8_t *data)
    {
        for (int i = 0; i < 3; i++) {
            write_int(data[i]);
            write('.');
        }

        write_int(data[3]);
    }

    void write_ipv4_network(const std::uint8_t *data, int prefix_length)
    {
        write_ipv4_address(data);
        write('/');
        write_int(prefix_length);
    }

    void write_ipv6_address(const std::uint8_t *data, std::uint32_t scope_id)
    {
        int start = 0;
        int size = 0;

        for (int pos = 0, last = 0; pos <= 16; pos += 2) {
            if (pos == 16 || data[pos] || data[pos + 1]) {
                int tmp = pos - last;
                if (tmp > size) {
                    size = tmp;
                    start = last;
                }
                last = pos + 2;
            }
        }

        for (int i = 0; i < start; i += 2) {
            write_ipv6_address_part(data + i);
            write(':');
        }

        if (size && start == 0) {
            write(':');
        }

        for (int i = start + size; i < 16; i += 2) {
            write(':');
            write_ipv6_address_part(data + i);
        }

        if (size && start + size == 16) {
            write(':');
        }

        write('%');
        write_ipv6_address_scope_id(scope_id);
    }

    void write_ipv6_network(const std::uint8_t *data, int prefix_length)
    {
        write_ipv6_address(data);
        write('/');
        write_int(prefix_length);
    }

    void write_ipv6_address_part(const std::uint8_t *data)
    {
        char buf[4]{
            ip_hex_charset[data[0] >> 4], ip_hex_charset[data[0] & 15],
            ip_hex_charset[data[1] >> 4], ip_hex_charset[data[1] & 15]
        };

        char *pos = buf;
        while (pos < std::end(buf) - 1 && *pos == '0') {
            ++pos;
        }

        write(pos, std::end(buf));
    }

    std::error_code write_ipv6_address_scope_id(std::uint32_t scope_id)
    {
        ErrnoScopeGuard guard;
        char buf[IF_NAMESIZE];
        char *last = if_indextoname(scope_id, buf);

        if (!last) {
            return std::make_error_code(std::errc{ errno });
        }

        write(buf, last);
        return {};
    }
};

template <class I, class S>
inline bool ipv4_from_string(I &&first, S &&last, std::uint8_t *dest)
{
    for (int i = 0; i < 4; i++) {
        if (first == last || !ascii_isdigit(*first)) {
            return true;
        }

        int value = *first - '0';
        ++first;

        if (value && first != last && ascii_isdigit(*first)) {
            value = 10 * value + *first - '0';
            ++first;

            if (first != last && ascii_isdigit(*first)) {
                int next = 10 * value + *first - '0';
                if (next < 256) {
                    value = next;
                    ++first;
                }
            }
        }

        if (i < 3) {
            if (first == last || *first != '.') {
                return true;
            }
            ++first;
        }

        dest[i] = value;
    }

    return false;
};

template <class I, class S>
inline bool ipv6_from_string(I &&first, S &&last, std::uint8_t *dest);

} // namespace hlib::detail

#endif

#ifndef HLIB_JSON_DETAIL_PARSER_H_
#define HLIB_JSON_DETAIL_PARSER_H_

#include <cmath>
#include <cstdint>
#include <charconv>
#include <limits>
#include <system_error>
#include <hlib/algorithm.h>
#include <hlib/ascii.h>
#include <hlib/json/common.h>

namespace hlib::json::detail {

template <class T, class Allocator>
using ParseStack =
    std::vector<BasicDocument<typename T::allocator_type> *,
                typename std::allocator_traits<Allocator>::rebind_alloc<
                    BasicDocument<typename T::allocator_type> *>>;

template <class T, class Allocator, class I, class S>
class ParseHandler {
public:
    using Null = json::Null;
    using Bool = json::Bool;
    using Int = json::Int;
    using Float = json::Float;
    using String = json::BasicString<typename T::allocator_type>;
    using Array = json::BasicArray<typename T::allocator_type>;
    using Object = json::BasicObject<typename T::allocator_type>;
    using Document = json::BasicDocument<typename T::allocator_type>;

    ParseStack<T, Allocator> *stack;
    ParseOptions *options;
    I first;
    S last;
    Document value;
    std::errc err;

    ParseHandler(ParseStack<T, Allocator> &stack, ParseOptions &options,
                 I first, S last, const typename T::allocator_type &alloc)
        : stack(std::addressof(stack)), options(std::addressof(options)),
          first(std::move(first)), last(std::move(last)), value(alloc), err()
    {
        this->stack->clear();
    }

    bool has_error()
    {
        return err != std::errc();
    }

    bool has_max_depth()
    {
        return stack->size() >= options->max_depth;
    }

    bool has_more_tokens()
    {
        skip_space();
        return !done();
    }

    bool has_next(char c)
    {
        return !done() && next() == c;
    }

    bool has_next(const char *s)
    {
        for (; *s; ++s) {
            if (done() || next() != *s) {
                return false;
            }
        }

        return true;
    }

    char peek()
    {
        return *first;
    }

    char next()
    {
        char c = peek();
        skip();
        return c;
    }

    void skip()
    {
        ++first;
    }

    void skip_space()
    {
        while (!done()) {
            char c = peek();
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                skip();
            } else {
                break;
            }
        }
    }

    bool done()
    {
        return first == last;
    }

    void run()
    {
        if (!has_more_tokens()) {
            set_invalid();
            return;
        }

        start_document(value);

        while (stack->size() && !has_error()) {
            if (has_max_depth()) {
                set_max_depth();
                return;
            } else if (!has_more_tokens()) {
                set_invalid();
                return;
            }

            continue_document(*stack->back());
        }
    }

    void start_document(Document &dest)
    {
        switch (peek()) {
        case 'n':
            start_null(dest);
            break;
        case 'f':
            start_true(dest);
            break;
        case 't':
            start_true(dest);
            break;
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            start_number(dest);
            break;
        case '"':
            start_string(dest);
            break;
        case '{':
            start_object(dest);
            break;
        case '[':
            start_array(dest);
            break;
        default:
            set_invalid();
            break;
        }
    }

    void start_null(Document &dest)
    {
        if (has_next("null")) {
            dest.assign(nullptr);
        } else {
            set_invalid();
        }
    }

    void start_true(Document &dest)
    {
        if (has_next("true")) {
            dest.assign(true);
        } else {
            set_invalid();
        }
    }

    void start_false(Document &dest)
    {
        if (has_next("false")) {
            dest.assign(false);
        } else {
            set_invalid();
        }
    }

    void start_number(Document &dest)
    {
        bool minus = peek() == '-';
        if (minus) {
            skip();

            if (done()) {
                set_invalid();
                return;
            }
        }

        read_num(dest, minus);
    }

    void read_num(Document &dest, bool minus)
    {
        if (!ascii_isdigit(peek())) {
            set_invalid();
            return;
        }

        Int num = next() - '0';

        if (num) {
            for (; !done() && ascii_isdigit(peek());) {
                num = 10 * num + next() - '0';
            }
        }

        if (!done()) {
            switch (peek()) {
            case '.':
                skip();
                read_frac(dest, minus, num);
                return;
            case 'e':
            case 'E':
                skip();
                read_exp(dest, minus, num, 0);
                return;
            }
        }

        dest.assign((2 * minus - 1) * num);
    }

    void read_frac(Document &dest, bool minus, Int num)
    {
        if (done() || !ascii_isdigit(peek())) {
            set_invalid();
            return;
        }

        Float mul = 0.1;
        Float frac = 0;

        for (; !done() && ascii_isdigit(peek());) {
            frac = std::fma(next() - '0', mul, frac);
            mul *= 0.1;
        }

        if (!done() && (peek() == 'e' || peek() == 'E')) {
            skip();
            read_exp(dest, minus, num, frac);
        } else {
            assign_document_float(dest, minus, num, frac, 0);
        }
    }

    void read_exp(Document &dest, bool minus, Int num, Float frac)
    {
        bool exp_minus = false;

        if (done()) {
            set_invalid();
            return;
        } else if (peek() == '-') {
            exp_minus = true;
            skip();
        } else if (peek() == '+') {
            skip();
        }

        if (done() || !ascii_isdigit(peek())) {
            set_invalid();
            return;
        }

        Int exp = 0;

        for (; !done() && ascii_isdigit(peek());) {
            exp = 10 * exp + next() - '0';
        }

        exp *= (2 * exp_minus - 1);
        assign_document_float(dest, minus, num, frac, exp);
    }

    void assign_document_float(
        Document &dest, bool minus, Int num, Float frac, Int exp)
    {
        Float float_value = (2 * minus - 1) * (num + frac) * std::pow(10, exp);
        Int int_value = float_value;

        if (float_value == int_value) {
            dest.assign(int_value);
        } else {
            dest.assign(float_value);
        }
    }

    void start_string(Document &dest)
    {
        dest.assign(String());
        start_string(dest.as_string());
    }

    void start_string(String &dest)
    {
        if (!has_next('"')) {
            set_invalid();
            return;
        }

        while (true) {
            if (done()) {
                set_invalid();
                return;
            }

            // clang-format off
            char c = next();
            switch (c) {
            case '"':
                return;
            case '\\':
                if (done()) {
                    set_invalid();
                    return;
                }
                read_string_escape(dest);
                if (has_error()) {
                    return;
                }
                break;
            case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05:
            case 0x06: case 0x07: case 0x08: case 0x09: case 0x0A: case 0x0B:
            case 0x0C: case 0x0D: case 0x0E: case 0x0F: case 0x10: case 0x11:
            case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
            case 0x18: case 0x19: case 0x1A: case 0x1B: case 0x1C: case 0x1D:
            case 0x1E: case 0x1F:
                set_invalid();
                return;
            default:
                dest.push_back(c);
                break;
            }
            // clang-format on
        }
    }

    void start_array(Document &dest)
    {
        if (has_next('[')) {
            dest.assign(Array());
            stack->push_back(std::addressof(dest));
        } else {
            set_invalid();
        }
    }

    void start_object(Document &dest)
    {
        if (has_next('{')) {
            dest.assign(Object());
            stack->push_back(std::addressof(dest));
        } else {
            set_invalid();
        }
    }

    void continue_document(Document &dest)
    {
        if (dest.is_array()) {
            continue_array(dest.as_array());
        } else {
            continue_object(dest.as_object());
        }
    }

    void continue_array(Array &dest)
    {
        if (peek() == ']') {
            skip();
            end_array();
        } else if (dest.size() && (!has_next(',') || !has_more_tokens())) {
            set_invalid();
        } else {
            start_document(dest.emplace_back());
        }
    }

    void continue_object(Object &dest)
    {
        if (peek() == '}') {
            skip();
            end_object();
        } else if (dest.size() && (!has_next(',') || !has_more_tokens())) {
            set_invalid();
        } else {
            String key(dest.get_allocator());

            start_string(key);

            auto [position, inserted] =
                dest.try_emplace(std::move(key), nullptr);

            if (!inserted) {
                set_duplicate_key();
            } else if (!has_more_tokens() || next() != ':' ||
                       !has_more_tokens()) {
                set_invalid();
            } else {
                start_document(position->second);
            }
        }
    }

    void end_array()
    {
        stack->pop_back();
    }

    void end_object()
    {
        stack->pop_back();
    }

    void read_string_escape(String &dest)
    {
        std::uint8_t hex[4];

        if (done()) {
            set_invalid();
            return;
        }

        switch (next()) {
        case '"':
            dest.push_back('"');
            break;
        case '\\':
            dest.push_back('\\');
            break;
        case '/':
            dest.push_back('/');
            break;
        case 'b':
            dest.push_back('\b');
            break;
        case 'f':
            dest.push_back('\f');
            break;
        case 'n':
            dest.push_back('\n');
            break;
        case 'r':
            dest.push_back('\r');
            break;
        case 't':
            dest.push_back('\t');
            break;
        case 'u':
            for (std::uint8_t &digit: hex) {
                if (done()) {
                    set_invalid();
                    return;
                }

                char c = next();
                if (c >= '0' && c <= '9') {
                    digit = c - '0';
                } else if (c >= 'a' && c <= 'f') {
                    digit = c - 'a' + 10;
                } else if (c >= 'A' && c <= 'F') {
                    digit = c - 'A' + 10;
                } else {
                    set_invalid();
                    return;
                }
            }

            dest.push_back(hex[0] << 4 | hex[1]);
            dest.push_back(hex[2] << 4 | hex[3]);
            break;
        default:
            set_invalid();
            break;
        }
    }

    void set_invalid()
    {
        err = std::errc::invalid_argument;
    }

    void set_overflow()
    {
        err = std::errc::value_too_large;
    }

    /// @todo Change error reporting for max depth.
    void set_max_depth()
    {
        err = std::errc::message_size;
    }

    /// @todo Change error reporting for duplicate key.
    void set_duplicate_key()
    {
        err = std::errc::invalid_argument;
    }
};

} // namespace hlib::json::detail

#endif

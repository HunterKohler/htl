#ifndef HLIB_JSON_PARSER_H_
#define HLIB_JSON_PARSER_H_

#include <cmath>
#include <cstdint>
#include <charconv>
#include <limits>
#include <system_error>
#include <hlib/algorithm.h>
#include <hlib/ascii.h>
#include <hlib/json/common.h>
#include <hlib/json/document.h>
#include <hlib/json/detail/parser.h>

namespace hlib::json {

template <class T, class I, class E>
struct ParseResult {
public:
    [[no_unique_address]] T value;
    [[no_unique_address]] I in;
    [[no_unique_address]] E err;

    template <class T2, class I2, class O2, class E2>
        requires std::convertible_to<const T &, T2> &&
            std::convertible_to<const I &, I2> &&
            std::convertible_to<const E &, E2>
    constexpr operator ParseResult<T2, I2, E2>() const &
    {
        return { value, in, err };
    }

    template <class T2, class I2, class O2, class E2>
        requires std::convertible_to<T, T2> && std::convertible_to<I, I2> &&
            std::convertible_to<E, E2>
    constexpr operator ParseResult<T2, I2, E2>() &&
    {
        return { std::move(value), std::move(in), std::move(err) };
    }
};

template <class T, class Allocator>
class BasicParser {
public:
    using Null = json::Null;
    using Bool = json::Bool;
    using Int = json::Int;
    using Float = json::Float;
    using String = json::BasicString<typename T::allocator_type>;
    using Array = json::BasicArray<typename T::allocator_type>;
    using Object = json::BasicObject<typename T::allocator_type>;
    using Document = json::BasicDocument<typename T::allocator_type>;

    using allocator_type = Allocator;

    BasicParser() noexcept(noexcept(Allocator())) : _stack(), _options() {}

    explicit BasicParser(const Allocator &alloc) noexcept
        : _stack(alloc), _options()
    {}

    explicit BasicParser(
        ParseOptions options, const Allocator &alloc = Allocator()) noexcept
        : _stack(alloc), _options(options)
    {}

    Allocator get_allocator() const noexcept
    {
        return _stack.get_allocator();
    }

    template <std::input_iterator I, std::sentinel_for<I> S>
        requires(std::is_same_v<std::iter_value_t<I>, char>)
    ParseResult<Document, I, std::error_code> parse(
        I first, S last,
        const typename T::allocator_type &alloc = typename T::allocator_type())
    {
        detail::ParseHandler<T, Allocator, I, S> context(
            _stack, _options, std::move(first), std::move(last), alloc);
        context.run();
        return { std::move(context.value), std::move(context.first),
                 std::make_error_code(context.err) };
    }

private:
    detail::ParseStack<T, Allocator> _stack;
    ParseOptions _options;
};

template <class Alloc = std::allocator<std::byte>, class ParseAlloc = Alloc,
          std::input_iterator I, std::sentinel_for<I> S>
    requires(std::is_same_v<std::iter_value_t<I>, char>)
ParseResult<BasicDocument<Alloc>, I, std::errc> parse(
    I first, S last, const Alloc &alloc = Alloc(),
    const ParseAlloc &parse_alloc = ParseAlloc())
{
    return BasicParser<BasicDocument<Alloc>, ParseAlloc>(parse_alloc)
        .parse(std::move(first), std::move(last), alloc);
}

using Parser = BasicParser<>;

namespace pmr {

using Parser =
    BasicParser<BasicDocument<std::pmr::polymorphic_allocator<std::byte>>,
                std::pmr::polymorphic_allocator<std::byte>>;

} // namespace pmr

} // namespace hlib::json

#endif

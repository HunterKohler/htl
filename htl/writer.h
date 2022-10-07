#ifndef HLIB_WRITER_H_
#define HLIB_WRITER_H_

#include <concepts>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <charconv>
#include <utility>
#include <hlib/utility.h>
#include <hlib/type_traits.h>

namespace hlib {

template <std::weakly_incrementable O>
class Writer {
public:
    Writer() = default;

    template <class U = O>
        requires(std::is_constructible_v<O, U> &&
                 !std::is_same_v<std::remove_cvref_t<U>, std::in_place_t> &&
                 !std::is_same_v<std::remove_cvref_t<U>, Writer>)
    explicit Writer(U &&u) noexcept(std::is_nothrow_constructible_v<O, U>)
        : _out(std::forward<U>(u))
    {}

    template <class... Args>
        requires(std::is_constructible_v<O, Args...>)
    explicit Writer(std::in_place_t, Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<O, Args...>)
        : _out(std::forward<Args>(args)...)
    {}

    template <class U, class... Args>
        requires(
            std::is_constructible_v<O, std::initializer_list<U> &, Args...>)
    explicit Writer(std::in_place_t, std::initializer_list<U> ilist,
                    Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<
                 O, std::initializer_list<U> &, Args...>)
        : _out(ilist, std::forward<Args>(args)...)
    {}

    O &out() &noexcept
    {
        return _out;
    }

    O &&out() &&noexcept
    {
        return std::move(_out);
    }

    const O &out() const &noexcept
    {
        return _out;
    }

    const O &&out() const &&noexcept
    {
        return _out;
    }

    template <class T>
        requires std::indirectly_writable<O, T>
    void write(T &&value)
    {
        *_out = std::forward<T>(value);
        ++_out;
    }

    template <std::input_iterator I, std::sentinel_for<I> S>
        requires(std::indirectly_copyable<I, O>)
    I write(I first, S last)
    {
        if constexpr (std::copyable<O>) {
            return std::ranges::copy(first, last, _out).in;
        } else {
            auto res = std::ranges::copy(first, last, std::move(_out));
            _out = std::move(res.out);
            return std::move(res.in);
        }
    }

    template <std::ranges::input_range R>
        requires(std::indirectly_copyable<std::ranges::iterator_t<R>, O> &&
                 !std::indirectly_writable<O, R>)
    std::ranges::borrowed_iterator_t<R> write(R &&r)
    {
        return write(std::ranges::begin(r), std::ranges::end(r));
    }

private:
    O _out;
};

template <std::weakly_incrementable O>
Writer(O) -> Writer<O>;

template <std::weakly_incrementable O>
class StringWriter : public Writer<O> {
private:
    using Base = Writer<O>;

public:
    using Base::Base;
    using Base::out;
    using Base::write;

    template <class CharT>
        requires(std::indirectly_copyable<const CharT *, O> &&is_char_v<CharT>)
    const CharT *write(const CharT *s)
    {
        return Base::write(std::basic_string_view(s));
    }

    template <class CharT, class Traits>
    const CharT *write(std::basic_string_view<CharT, Traits> s)
    {
        return Base::write(s);
    }

    void write_bool(bool value)
    {
        write(value ? "true" : "false");
    }

    template <std::integral T>
    void write_int(T value)
    {
        char buf[std::numeric_limits<T>::digits10 + 2];
        Base::write(buf, std::to_chars(buf, std::end(buf), value).ptr);
    }

    template <std::floating_point T>
    void write_float(T value)
    {
        char buf[std::numeric_limits<T>::max_digits10 + 2];
        Base::write(buf, std::to_chars(buf, std::end(buf), value).ptr);
    }
};

template <std::weakly_incrementable O>
StringWriter(O) -> StringWriter<O>;

} // namespace hlib

#endif

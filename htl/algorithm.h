#ifndef HLIB_ALGORITHM_H_
#define HLIB_ALGORITHM_H_

namespace hlib {

template <class I, class O, class T, class E>
struct InOutValueErrorResult {
    [[no_unique_address]] I in;
    [[no_unique_address]] O out;
    [[no_unique_address]] T value;
    [[no_unique_address]] E err;

    template <class I2, class O2, class T2, class E2>
        requires std::convertible_to<const I &, I2> &&
            std::convertible_to<const O &, O2> &&
            std::convertible_to<const T &, T2> &&
            std::convertible_to<const E &, E2>
    constexpr operator InOutErrorResult<I2, O2, T2, E2>() const &
    {
        return { in, out, value, err };
    }

    template <class I2, class O2, class T2, class E2>
        requires std::convertible_to<I, I2> && std::convertible_to<O, O2> &&
            std::convertible_to<T, T2> && std::convertible_to<E, E2>
    constexpr operator InOutErrorResult<I2, O2, T2, E2>() &&
    {
        return { std::move(in), std::move(out), std::move(value),
                 std::move(err) };
    }
};

template <class I, class O, class E>
struct InOutErrorResult {
    [[no_unique_address]] I in;
    [[no_unique_address]] O out;
    [[no_unique_address]] E err;

    template <class I2, class O2, class E2>
        requires std::convertible_to<const I &, I2> &&
            std::convertible_to<const O &, O2> &&
            std::convertible_to<const E &, E2>
    constexpr operator InOutErrorResult<I2, O2, E2>() const &
    {
        return { in, out, err };
    }

    template <class I2, class O2, class E2>
        requires std::convertible_to<I, I2> && std::convertible_to<O, O2> &&
            std::convertible_to<E, E2>
    constexpr operator InOutErrorResult<I2, O2, E2>() &&
    {
        return { std::move(in), std::move(out), std::move(err) };
    }
};

} // namespace hlib

#endif

#ifndef HTL_DETAIL_ITERATOR_H_
#define HTL_DETAIL_ITERATOR_H_

#include <utility>
#include <ranges>
#include <iterator>

namespace htl::detail {

template <class T, std::output_iterator<T> O>
constexpr void iter_write(O &out, T &&value)
{
    *out = std::forward<T>(value);
    ++out;
}

template <std::input_iterator I>
constexpr std::iter_reference_t<I> iter_read(I &it)
{
    std::iter_reference_t<I> ref{ *it };
    ++it;
    return ref;
}

} // namespace htl::detail

#endif

#ifndef HLIB_JSON_ARRAY_H_
#define HLIB_JSON_ARRAY_H_

#include <memory>
#include <unordered_map>
#include <hlib/json/common.h>
#include <hlib/json/initializer.h>

namespace hlib::json {

namespace detail {

template <class Allocator>
using BasicArrayBase =
    std::vector<BasicDocument<Allocator>,
                typename std::allocator_traits<Allocator>::rebind_alloc<
                    BasicDocument<Allocator>>>;

} // namespace detail

template <class Allocator>
class BasicArray : public detail::BasicArrayBase<Allocator> {
private:
    using Base = detail::BasicArrayBase<Allocator>;

    struct RangeTag {
        explicit RangeTag() = default;
    };

public:
    using Null = json::Null;
    using Bool = json::Bool;
    using Int = json::Int;
    using Float = json::Float;
    using String = json::BasicString<Allocator>;
    using Array = json::BasicArray<Allocator>;
    using Object = json::BasicObject<Allocator>;
    using Document = json::BasicDocument<Allocator>;

    BasicArray() noexcept(noexcept(Allocator{})) : Base{} {}

    explicit BasicArray(const Allocator &alloc) noexcept : Base{ alloc } {}

    explicit BasicArray(std::size_t n, const Document &value = Document{},
                        const Allocator &alloc = Allocator{})
        : Base{ n, value, alloc }
    {}

    explicit BasicArray(std::size_t n, const Allocator &alloc = Allocator{})
        : Base{ n, alloc }
    {}

    template <std::input_iterator I, std::sentinel_for<I> S>
        requires(std::convertible_to<std::iter_reference_t<I>, Document>)
    BasicArray(I first, S last, const Allocator &alloc = Allocator{})
        : BasicArray(RangeTag{}, std::move(first), std::move(last), alloc)
    {}

    template <std::ranges::input_range R>
        requires(
            std::convertible_to<std::ranges::range_reference_t<R>, Document>)
    explicit BasicArray(R &&r, const Allocator &alloc = Allocator{})
        : BasicArray(
              RangeTag{}, std::ranges::begin(r), std::ranges::end(r), alloc)
    {}

    BasicArray(std::initializer_list<DocumentInit<Allocator>> init_list,
               const Allocator &alloc = Allocator{})
        : Base{ alloc }
    {
        for (auto &value: init_list) {
            Base::emplace_back(value);
        }
    }

    BasicArray(const BasicArray &other) : Base{ other } {}

    BasicArray(const BasicArray &other, const Allocator &alloc)
        : Base{ other, alloc }
    {}

    BasicArray(BasicArray &&other) noexcept : Base{ std::move(other) } {}

    BasicArray(BasicArray &&other, const Allocator &alloc)
        : Base{ std::move(other), alloc }
    {}

    BasicArray &operator=(BasicArray &&other) = default;
    BasicArray &operator=(const BasicArray &other) = default;

private:
    using DocumentInitType = detail::DocumentInitType;

    template <class I>
    BasicArray(RangeTag, I first, I last, const Allocator &alloc)
        : Base{ std::move(first), std::move(last), alloc }
    {}

    template <class I, class S>
    BasicArray(RangeTag, I first, S last, const Allocator &alloc)
        : BasicArray{ std::common_iterator<I, S>(std::move(first)),
                      std::common_iterator<I, S>(std::move(last)), alloc }
    {}
};

template <class T, class StackAlloc = std::allocator<std::byte>,
          std::output_iterator<char> O>
inline O to_chars(const BasicArray<T> &value, O out,
                  const StackAlloc &stack_alloc = StackAlloc())
{
    BasicSerializer<BasicDocument<T>, StackAlloc> s{ stack_alloc };
    return s.serialize(value, std::move(out));
}

template <class T, class Alloc = std::allocator<char>,
          class StackAlloc = std::allocator<std::byte>>
inline std::basic_string<char, std::char_traits<char>, Alloc> //
to_string(const BasicArray<T> &value, const Alloc &alloc = Alloc(),
          const StackAlloc &stack_alloc = StackAlloc())
{
    std::basic_string<char, std::char_traits<char>, Alloc> result;
    to_chars(value, std::back_inserter(result), stack_alloc);
    return result;
}

template <class CharT, class Traits, class T>
inline std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os, const BasicArray<T> &value)
{
    to_chars(value, std::ostream_iterator<char>(os));
    return os;
}

} // namespace hlib::json

#endif

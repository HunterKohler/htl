#ifndef HLIB_JSON_OBJECT_H_
#define HLIB_JSON_OBJECT_H_

#include <memory>
#include <iterator>
#include <ranges>
#include <unordered_map>
#include <hlib/json/common.h>
#include <hlib/json/serializer.h>
#include <hlib/json/initializer.h>

namespace hlib::json {

namespace detail {

struct StringViewHash : std::hash<std::string_view> {
    using is_transparent = void;
};

template <class Allocator>
using BasicObjectBase = std::unordered_map<
    BasicString<Allocator>, BasicDocument<Allocator>, StringViewHash,
    std::equal_to<>,
    typename std::allocator_traits<Allocator>::rebind_alloc<
        std::pair<const BasicString<Allocator>, BasicDocument<Allocator>>>>;

} // namespace detail

template <class Allocator>
class BasicObject : public detail::BasicObjectBase<Allocator> {
private:
    using Base = detail::BasicObjectBase<Allocator>;

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

    BasicObject() : Base{} {}

    BasicObject(const Allocator &alloc) : Base{ alloc } {}

    template <std::input_iterator I, std::sentinel_for<I> S>
        requires(std::convertible_to<
                 std::iter_reference_t<I>, std::pair<String, Document>>)
    BasicObject(I first, S last, const Allocator &alloc = Allocator{})
        : BasicObject(RangeTag{}, std::move(first), std::move(last), alloc)
    {}

    template <std::ranges::input_range R>
        requires(std::convertible_to<std::ranges::range_reference_t<R>,
                                     std::pair<String, Document>>)
    explicit BasicObject(R &&r, const Allocator &alloc = Allocator{})
        : BasicObject(std::ranges::begin(r), std::ranges::end(r), alloc)
    {}

    BasicObject(std::initializer_list<ItemInit<Allocator>> init_list,
                const Allocator &alloc = Allocator{})
        : Base{ alloc }
    {
        _insert_init_list(init_list);
    }

    BasicObject(const BasicObject &other) : Base{ other } {}

    BasicObject(const BasicObject &other, const Allocator &alloc)
        : Base{ other, alloc }
    {}

    BasicObject(BasicObject &&other) noexcept : Base{ std::move(other) } {}

    BasicObject(BasicObject &&other, const Allocator &alloc)
        : Base{ std::move(other), alloc }
    {}

    BasicObject &operator=(BasicObject &&other) = default;
    BasicObject &operator=(const BasicObject &other) = default;

    BasicObject &operator=(std::initializer_list<ItemInit<Allocator>> init_list)
    {
        Base::clear();
        _insert_init_list(init_list);
    }

private:
    using DocumentInitType = detail::DocumentInitType;
    using StringInitType = detail::StringInitType;

    friend Array;
    friend Document;

    template <class I>
    BasicObject(RangeTag, I first, I last, const Allocator &alloc)
        : Base{ std::move(first), std::move(last), 1, alloc }
    {}

    template <class I, class S>
    BasicObject(RangeTag, I first, S last, const Allocator &alloc)
        : BasicObject{ std::common_iterator<I, S>(std::move(first)),
                       std::common_iterator<I, S>(std::move(last)), alloc }
    {}

    void _insert_init_list(std::initializer_list<ItemInit<Allocator>> init_list)
    {
        for (auto &[key, value]: init_list) {
            switch (key._type) {
            case StringInitType::Empty:
                _emplace_init("", value);
                break;
            case StringInitType::CharPtr:
                _emplace_init(key._char_ptr, value);
                break;
            case StringInitType::StringView:
                _emplace_init(key._string_view, value);
                break;
            case StringInitType::String:
                _emplace_init(std::move(key._string), value);
                break;
            case StringInitType::StringRef:
                _emplace_init(*key._string_ref, value);
                break;
            }
        }
    }

    template <class K>
    void _emplace_init(K &&key, const DocumentInit<Allocator> &init)
    {
        std::string_view sv{ key };

        if (Base::contains(sv)) {
            Base::find(sv)->second.assign(std::forward<K>(key));
        } else {
            Base::emplace(std::forward<K>(key), init);
        }
    }
};

template <class T, class StackAlloc = std::allocator<std::byte>,
          std::output_iterator<char> O>
inline O to_chars(const BasicObject<T> &value, O out,
                  const StackAlloc &stack_alloc = StackAlloc())
{
    BasicSerializer<BasicDocument<T>, StackAlloc> s{ stack_alloc };
    return s.serialize(value, std::move(out));
}

template <class T, class Alloc = std::allocator<char>,
          class StackAlloc = std::allocator<std::byte>>
inline std::basic_string<char, std::char_traits<char>, Alloc> //
to_string(const BasicObject<T> &value, const Alloc &alloc = Alloc(),
          const StackAlloc &stack_alloc = StackAlloc())
{
    std::basic_string<char, std::char_traits<char>, Alloc> result;
    to_chars(value, std::back_inserter(result), stack_alloc);
    return result;
}

template <class CharT, class Traits, class T>
inline std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os, const BasicObject<T> &value)
{
    to_chars(value, std::ostream_iterator<char>(os));
    return os;
}

} // namespace hlib::json

#endif

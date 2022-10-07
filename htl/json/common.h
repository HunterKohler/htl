#ifndef HLIB_JSON_COMMON_H_
#define HLIB_JSON_COMMON_H_

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory_resource>
#include <memory>
#include <ranges>
#include <string_view>
#include <string>

namespace hlib::json {

template <class Allocator = std::allocator<std::byte>>
using BasicString = std::basic_string<
    char, std::char_traits<char>,
    typename std::allocator_traits<Allocator>::rebind_alloc<char>>;

template <class Allocator = std::allocator<std::byte>>
class BasicArray;

template <class Allocator = std::allocator<std::byte>>
class BasicObject;

template <class Allocator = std::allocator<std::byte>>
class BasicDocument;

template <class T = BasicDocument<std::allocator<std::byte>>,
          class Allocator = T::allocator_type>
class BasicSerializer;

template <class T = BasicDocument<std::allocator<std::byte>>,
          class Allocator = T::allocator_type>
class BasicParser;

using Null = std::nullptr_t;
using Bool = bool;
using Int = long long;
using Float = double;
using String = BasicString<>;
using Array = BasicArray<>;
using Object = BasicObject<>;
using Document = BasicDocument<>;

struct ParseOptions {
    std::size_t max_depth = 250;
};

namespace pmr {

using String = BasicString<std::pmr::polymorphic_allocator<std::byte>>;
using Array = BasicArray<std::pmr::polymorphic_allocator<std::byte>>;
using Object = BasicObject<std::pmr::polymorphic_allocator<std::byte>>;
using Document = BasicDocument<std::pmr::polymorphic_allocator<std::byte>>;

} // namespace pmr
} // namespace hlib::json

#endif

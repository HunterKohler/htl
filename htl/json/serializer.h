#ifndef HTL_JSON_SERIALIZER_H_
#define HTL_JSON_SERIALIZER_H_

#include <charconv>
#include <concepts>
#include <limits>
#include <memory>
#include <vector>
#include <htl/ascii.h>
#include <htl/json/common.h>
#include <htl/json/type.h>
#include <htl/detail/iterator.h>

namespace htl::json {
namespace detail {

template <class T>
class SerializerStackValue {
public:
    using Array = json::BasicArray<typename T::allocator_type>;
    using Object = json::BasicObject<typename T::allocator_type>;
    using Document = json::BasicDocument<typename T::allocator_type>;
    using ArrayIterator = Array::const_iterator;
    using ObjectIterator = Object::const_iterator;

    SerializerStackValue(const Document &document, ArrayIterator it)
        : document{ std::addressof(document) }, array_it{ std::move(it) }
    {}

    SerializerStackValue(const Document &document, ObjectIterator it)
        : document{ std::addressof(document) }, object_it{ std::move(it) }
    {}

    ~SerializerStackValue() = default;

    ~SerializerStackValue() requires(
        !std::is_trivially_destructible_v<ArrayIterator> ||
        !std::is_trivially_destructible_v<ObjectIterator>)
    {
        if (document->is_array()) {
            std::ranges::destroy_at(std::addressof(array_it));
        } else {
            std::ranges::destroy_at(std::addressof(array_it));
        }
    }

    const Document *document;

    union {
        ArrayIterator array_it;
        ObjectIterator object_it;
    };
};

template <class T, class Allocator>
using SerializerStack =
    std::vector<SerializerStackValue<T>,
                typename std::allocator_traits<Allocator>::rebind_alloc<
                    SerializerStackValue<T>>>;

template <class T, class Allocator, class O>
struct SerializeHelper {
    using Null = json::Null;
    using Bool = json::Bool;
    using Int = json::Int;
    using Float = json::Float;
    using String = json::BasicString<typename T::allocator_type>;
    using Array = json::BasicArray<typename T::allocator_type>;
    using Object = json::BasicObject<typename T::allocator_type>;
    using Document = json::BasicDocument<typename T::allocator_type>;
    using ArrayIterator = Array::const_iterator;
    using ObjectIterator = Object::const_iterator;

    SerializerStack<T, Allocator> &stack;
    O &out;

    void serialize(Null)
    {
        write("null");
    }

    void serialize(Bool value)
    {
        write(value ? "true" : "false");
    }

    void serialize(std::integral auto value)
    {
        char buf[std::numeric_limits<decltype(value)>::digits10 + 2];
        auto res = std::to_chars(buf, std::end(buf), value);
        write(std::string_view(buf, res.ptr));
    }

    void serialize(std::floating_point auto value)
    {
        char buf[std::numeric_limits<decltype(value)>::max_digits10 + 2];
        auto res = std::to_chars(buf, std::end(buf), value);
        write(std::string_view(buf, res.ptr));
    }

    void serialize(std::string_view value)
    {
        static constexpr char hex[] = "0123456789ABCDEF";

        write("\"");

        for (auto c: value) {
            switch (c) {
            case '"':
                write("\\\"");
                break;
            case '\\':
                write("\\\\");
                break;
            case '\b':
                write("\\b");
                break;
            case '\f':
                write("\\f");
                break;
            case '\n':
                write("\\n");
                break;
            case '\r':
                write("\\r");
                break;
            case '\t':
                write("\\t");
                break;
            default:
                if (ascii_iscntrl(c)) {
                    write("\\u00");
                    write(hex[to_unsigned(c) >> 4]);
                    write(hex[to_unsigned(c) & 15]);
                } else {
                    write(c);
                }
                break;
            }
        }

        write("\"");
    }

    void serialize(const Array &value)
    {
        auto first = value.begin();
        auto last = value.end();

        write("[");

        if (first != last) {
            serialize(*first);

            while (++first != last) {
                write(",");
                serialize(*first);
            }
        }

        write("]");
    }

    void serialize(const Object &value)
    {
        auto first = value.begin();
        auto last = value.end();

        write("{");

        if (first != last) {
            serialize(first->first);
            write(":");
            serialize(first->second);

            while (++first != last) {
                write(",");
                serialize(first->first);
                write(":");
                serialize(first->second);
            }
        }

        write("}");
    }

    void serialize(const Document &value)
    {
        stack.clear();
        push_document(value);
        serialize_stack();
    }

    void serialize_stack()
    {
        while (stack.size()) {
            auto &value = stack.back();
            auto &document = *value.document;

            if (document.is_array()) {
                serialize_next_array(document.as_array(), value.array_it);
            } else {
                serialize_next_object(document.as_object(), value.object_it);
            }
        }
    }

    void serialize_next_array(const Array &value, ArrayIterator &it)
    {
        if (it == value.end()) {
            write("]");
            stack.pop_back();
        } else {
            if (it != value.begin()) {
                write(",");
            }

            // push may resize stack, must increment first
            auto &elem = *it;
            ++it;
            push_document(elem);
        }
    }

    void serialize_next_object(const Object &value, ObjectIterator &it)
    {
        if (it == value.end()) {
            write("}");
            stack.pop_back();
        } else {
            if (it != value.begin()) {
                write(",");
            }

            // push may resize stack, must increment first
            auto &elem = *it;
            ++it;
            push_entry(elem);
        }
    }

    void push_document(const Document &value)
    {
        switch (value.type()) {
        case Type::Null:
            serialize(nullptr);
            break;
        case Type::Bool:
            serialize(value.as_bool());
            break;
        case Type::Int:
            serialize(value.as_int());
            break;
        case Type::Float:
            serialize(value.as_float());
            break;
        case Type::String:
            serialize(value.as_string());
            break;
        case Type::Array:
            push_array(value);
            break;
        case Type::Object:
            push_object(value);
            break;
        }
    }

    void push_array(const Document &value)
    {
        write("[");
        stack.emplace_back(value, value.as_array().begin());
    }

    void push_object(const Document &value)
    {
        write("{");
        stack.emplace_back(value, value.as_object().begin());
    }

    void push_entry(const std::pair<const String, Document> &value)
    {
        serialize(value.first);
        write(":");
        push_document(value.second);
    }

    void write(char c)
    {
        *out = c;
        ++out;
    }

    void write(std::string_view value)
    {
        for (auto c: value) {
            write(c);
        }
    }
};

} // namespace detail

template <class T, class Allocator>
class BasicSerializer {
public:
    using Null = json::Null;
    using Bool = json::Bool;
    using Int = json::Int;
    using Float = json::Float;
    using String = json::BasicString<typename T::allocator_type>;
    using Array = json::BasicArray<typename T::allocator_type>;
    using Object = json::BasicObject<typename T::allocator_type>;
    using Document = json::BasicDocument<typename T::allocator_type>;

    BasicSerializer() noexcept(noexcept(Allocator())) : _stack{} {}

    explicit BasicSerializer(const Allocator &alloc) noexcept : _stack{ alloc }
    {}

    template <std::output_iterator<char> O>
    O serialize(Bool value, O out)
    {
        return _serialize_impl(value, out);
    }

    template <std::output_iterator<char> O>
    O serialize(std::integral auto value, O out)
    {
        return _serialize_impl(value, out);
    }

    template <std::output_iterator<char> O>
    O serialize(std::floating_point auto value, O out)
    {
        return _serialize_impl(value, out);
    }

    template <std::output_iterator<char> O>
    O serialize(std::string_view value, O out)
    {
        return _serialize_impl(value, out);
    }

    template <std::output_iterator<char> O>
    O serialize(const Array &value, O out)
    {
        return _serialize_impl(value, out);
    }

    template <std::output_iterator<char> O>
    O serialize(const Object &value, O out)
    {
        return _serialize_impl(value, out);
    }

    template <std::output_iterator<char> O>
    O serialize(const Document &value, O out)
    {
        return _serialize_impl(value, out);
    }

private:
    detail::SerializerStack<T, Allocator> _stack;

    template <class U, class O>
    O _serialize_impl(const U &value, O &out)
    {
        using Helper = detail::SerializeHelper<Document, Allocator, O>;

        Helper{ _stack, out }.serialize(value);

        return std::move(out);
    }
};

} // namespace htl::json

#endif

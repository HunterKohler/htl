#ifndef HLIB_JSON_DOCUMENT_H_
#define HLIB_JSON_DOCUMENT_H_

#include <algorithm>
#include <memory>
#include <hlib/utility.h>
#include <hlib/json/type.h>
#include <hlib/json/array.h>
#include <hlib/json/object.h>
#include <hlib/json/serializer.h>
#include <hlib/json/initializer.h>
#include <hlib/detail/simple_hash.h>

namespace hlib::json {

template <class Allocator>
class BasicDocument {
public:
    using Null = json::Null;
    using Bool = json::Bool;
    using Int = json::Int;
    using Float = json::Float;
    using String = json::BasicString<Allocator>;
    using Array = json::BasicArray<Allocator>;
    using Object = json::BasicObject<Allocator>;
    using Document = json::BasicDocument<Allocator>;

    using InitializerList = std::initializer_list<DocumentInit<Allocator>>;

    using allocator_type = Allocator;

    BasicDocument() noexcept(noexcept(Allocator{}))
        : _alloc{}, _type{ Type::Null }
    {}

    explicit BasicDocument(const Allocator &alloc) noexcept
        : _alloc{ alloc }, _type{ Type::Null }
    {}

    explicit BasicDocument(Null, const Allocator &alloc = Allocator{}) noexcept
        : _alloc{ alloc }, _type{ Type::Null }
    {}

    explicit BasicDocument(
        Bool value, const Allocator &alloc = Allocator{}) noexcept
        : _alloc{ alloc }, _type{ Type::Bool }, _bool{ value }
    {}

    explicit BasicDocument(
        std::integral auto value, const Allocator &alloc = Allocator{}) noexcept
        : _alloc{ alloc }, _type{ Type::Int }, _int(value)
    {}

    explicit BasicDocument(std::floating_point auto value,
                           const Allocator &alloc = Allocator{}) noexcept
        : _alloc{ alloc }, _type{ Type::Float }, _float(value)
    {}

    explicit BasicDocument(
        const char *value, const Allocator &alloc = Allocator{})
        : _alloc{ alloc }, _type{ Type::String }, _string{ _new<String>(value) }
    {}

    explicit BasicDocument(
        std::string_view value, const Allocator &alloc = Allocator{})
        : _alloc{ alloc }, _type{ Type::String }, _string{ _new<String>(value) }
    {}

    explicit BasicDocument(
        const String &value, const Allocator &alloc = Allocator{})
        : _alloc{ alloc }, _type{ Type::String }, _string{ _new<String>(value) }
    {}

    explicit BasicDocument(
        const Array &value, const Allocator &alloc = Allocator{})
        : _alloc{ alloc }, _type{ Type::Array }, _array{ _new<Array>(value) }
    {}

    explicit BasicDocument(
        const Object &value, const Allocator &alloc = Allocator{})
        : _alloc{ alloc }, _type{ Type::Object }, _object{ _new<Object>(value) }
    {}

    explicit BasicDocument(String &&value, const Allocator &alloc = Allocator{})
        : _alloc{ alloc }, _type{ Type::String }, //
          _string{ _new<String>(std::move(value)) }
    {}

    explicit BasicDocument(Array &&value, const Allocator &alloc = Allocator{})
        : _alloc{ alloc }, _type{ Type::Array }, //
          _array{ _new<Array>(std::move(value)) }
    {}

    explicit BasicDocument(Object &&value, const Allocator &alloc = Allocator{})
        : _alloc{ alloc }, _type{ Type::Object }, //
          _object{ _new<Object>(std::move(value)) }
    {}

    explicit BasicDocument(const DocumentInit<Allocator> &value,
                           const Allocator &alloc = Allocator{})
        : _alloc{ alloc }
    {
        _construct(value);
    }

    BasicDocument(InitializerList value, const Allocator &alloc = Allocator{})
        : _alloc{ alloc }
    {
        _construct(value);
    }

    BasicDocument(const BasicDocument &other)
        : BasicDocument(
              other, std::allocator_traits<Allocator>::
                         select_on_container_copy_construction(other._alloc))
    {}

    BasicDocument(const BasicDocument &other, const Allocator &alloc)
        : _alloc{ alloc }
    {
        _construct(other);
    }

    BasicDocument(BasicDocument &&other) noexcept
        : _alloc{ std::move(other._alloc) }
    {
        _move_members(std::move(other));
    }

    BasicDocument(BasicDocument &&other, const Allocator &alloc) //
        noexcept(std::allocator_traits<Allocator>::is_always_equal::value)
        : _alloc{ alloc }
    {
        _construct(std::move(other));
    }

    ~BasicDocument() { _destroy(); }

    BasicDocument &operator=(InitializerList value)
    {
        assign(value);
        return *this;
    }

    BasicDocument &operator=(const BasicDocument &other)
    {
        assign(other);
        return *this;
    }

    BasicDocument &operator=(BasicDocument &&other)
    {
        assign(std::move(other));
        return *this;
    }

    Allocator get_allocator() const noexcept { return _alloc; }

    Type type() const noexcept { return _type; }

    void assign(Null) { _reset(nullptr); }
    void assign(Bool value) { _reset(value); }
    void assign(std::floating_point auto value) { _reset(value); }
    void assign(std::integral auto value) { _reset(value); }
    void assign(const char *value) { _reset(value); }
    void assign(std::string_view value) { _reset(value); }
    void assign(const String &value) { _assign_string(value); }
    void assign(const Array &value) { _assign_array(value); }
    void assign(const Object &value) { _assign_object(value); }
    void assign(const Document &value) { _assign_document(value); }
    void assign(String &&value) { _assign_string(std::move(value)); }
    void assign(Array &&value) { _assign_array(std::move(value)); }
    void assign(Object &&value) { _assign_object(std::move(value)); }
    void assign(Document &&value) { _assign_document(std::move(value)); }
    void assign(const DocumentInit<Allocator> &value) { _reset(value); }
    void assign(InitializerList value) { _reset(value); }

    bool is_null() const noexcept { return _type == Type::Null; }
    bool is_bool() const noexcept { return _type == Type::Bool; }
    bool is_int() const noexcept { return _type == Type::Int; }
    bool is_float() const noexcept { return _type == Type::Float; }
    bool is_string() const noexcept { return _type == Type::String; }
    bool is_array() const noexcept { return _type == Type::Array; }
    bool is_object() const noexcept { return _type == Type::Object; }

    Bool &as_bool() &
    {
        assert(is_bool());
        return _bool;
    }

    Int &as_int() &
    {
        assert(is_int());
        return _int;
    }

    Float &as_float() &
    {
        assert(is_float());
        return _float;
    }

    String &as_string() &
    {
        assert(is_string());
        return *_string;
    }

    Array &as_array() &
    {
        assert(is_array());
        return *_array;
    }

    Object &as_object() &
    {
        assert(is_object());
        return *_object;
    }

    const Bool &as_bool() const &
    {
        assert(is_bool());
        return _bool;
    }

    const Int &as_int() const &
    {
        assert(is_int());
        return _int;
    }

    const Float &as_float() const &
    {
        assert(is_float());
        return _float;
    }

    const String &as_string() const &
    {
        assert(is_string());
        return *_string;
    }

    const Array &as_array() const &
    {
        assert(is_array());
        return *_array;
    }

    const Object &as_object() const &
    {
        assert(is_object());
        return *_object;
    }

    Bool &&as_bool() &&
    {
        assert(is_bool());
        return std::move(_bool);
    }

    Int &&as_int() &&
    {
        assert(is_int());
        return std::move(_int);
    }

    Float &&as_float() &&
    {
        assert(is_float());
        return std::move(_float);
    }

    String &&as_string() &&
    {
        assert(is_string());
        return std::move(*_string);
    }

    Array &&as_array() &&
    {
        assert(is_array());
        return std::move(*_array);
    }

    Object &&as_object() &&
    {
        assert(is_object());
        return std::move(*_object);
    }

    const Bool &&as_bool() const &&
    {
        assert(is_bool());
        return std::move(_bool);
    }

    const Int &&as_int() const &&
    {
        assert(is_int());
        return std::move(_int);
    }

    const Float &&as_float() const &&
    {
        assert(is_float());
        return std::move(_float);
    }

    const String &&as_string() const &&
    {
        assert(is_string());
        return std::move(*_string);
    }

    const Array &&as_array() const &&
    {
        assert(is_array());
        return std::move(*_array);
    }

    const Object &&as_object() const &&
    {
        assert(is_object());
        return std::move(*_object);
    }

private:
    friend Array;
    friend Object;

    using DocumentInitType = detail::DocumentInitType;

    template <class T>
    using AllocatorRebind =
        typename std::allocator_traits<Allocator>::rebind_alloc<T>;

    template <class T>
    using AllocatorPointer =
        typename std::allocator_traits<Allocator>::rebind_traits<T>::pointer;

    template <class T, class... Args>
    auto _new(Args &&...args)
    {
        using NewAllocator = AllocatorRebind<T>;
        using NewAllocatorTraits = std::allocator_traits<NewAllocator>;
        using NewAllocatorPointer = NewAllocatorTraits::pointer;

        NewAllocator new_alloc{ _alloc };
        NewAllocatorPointer p{ NewAllocatorTraits::allocate(new_alloc, 1) };
        NewAllocatorTraits::construct(
            new_alloc, std::to_address(p), std::forward<Args>(args)...);

        return p;
    }

    template <class Ptr>
    void _delete(Ptr p)
    {
        using DeleteAllocator =
            AllocatorRebind<typename std::pointer_traits<Ptr>::element_type>;
        using DeleteAllocatorTraits = std::allocator_traits<DeleteAllocator>;

        DeleteAllocator delete_alloc{ _alloc };
        DeleteAllocatorTraits::destroy(delete_alloc, std::to_address(p));
        DeleteAllocatorTraits::deallocate(delete_alloc, p, 1);
    }

    void _construct(Null) { _type = Type::Null; }

    void _construct(Bool value)
    {
        _type = Type::Bool;
        _bool = value;
    }

    void _construct(std::integral auto value)
    {
        _type = Type::Int;
        _int = value;
    }

    void _construct(std::floating_point auto value)
    {
        _type = Type::Float;
        _float = value;
    }

    void _construct(const char *value) { _construct_string(value); }

    void _construct(std::string_view value) { _construct_string(value); }

    void _construct(const String &value) { _construct_string(value); }

    void _construct(const Array &value) { _construct_array(value); }

    void _construct(const Object &value) { _construct_object(value); }

    void _construct(const Document &value) { _construct_document(value); }

    void _construct(String &&value) { _construct_string(std::move(value)); }

    void _construct(Array &&value) { _construct_array(std::move(value)); }

    void _construct(Object &&value) { _construct_object(std::move(value)); }

    void _construct(Document &&value) { _construct_document(std::move(value)); }

    void _construct(const DocumentInit<Allocator> &value)
    {
        switch (value._type) {
        case DocumentInitType::Null:
            _construct(nullptr);
            break;
        case DocumentInitType::Bool:
            _construct(value._bool);
            break;
        case DocumentInitType::Int:
            _construct(value._int);
            break;
        case DocumentInitType::Float:
            _construct(value._float);
            break;
        case DocumentInitType::CharPtr:
            _construct(value._char_ptr);
            break;
        case DocumentInitType::StringView:
            _construct(value._string_view);
            break;
        case DocumentInitType::String:
            _construct(std::move(value._string));
            break;
        case DocumentInitType::Array:
            _construct(std::move(value._array));
            break;
        case DocumentInitType::Object:
            _construct(std::move(value._object));
            break;
        case DocumentInitType::Document:
            _construct(std::move(value._document));
            break;
        case DocumentInitType::StringRef:
            _construct(*value._string_ref);
            break;
        case DocumentInitType::ArrayRef:
            _construct(*value._array_ref);
            break;
        case DocumentInitType::ObjectRef:
            _construct(*value._object_ref);
            break;
        case DocumentInitType::DocumentRef:
            _construct(*value._document_ref);
            break;
        case DocumentInitType::InitList:
            _construct(value._init_list);
            break;
        }
    }

    void _construct(std::initializer_list<DocumentInit<Allocator>> init_list)
    {
        bool object_like = std::ranges::all_of(
            init_list, &DocumentInit<Allocator>::_is_entry_like);

        if (object_like) {
            _construct_object();

            for (auto &item: init_list) {
                auto &key = item._init_list.begin()[0];
                auto &value = item._init_list.begin()[1];

                switch (key._type) {
                case DocumentInitType::CharPtr:
                    as_object()._emplace_init(key._char_ptr, value);
                    break;
                case DocumentInitType::StringView:
                    as_object()._emplace_init(key._string_view, value);
                    break;
                case DocumentInitType::String:
                    as_object()._emplace_init(std::move(key._string), value);
                    break;
                case DocumentInitType::StringRef:
                    as_object()._emplace_init(*key._string_ref, value);
                    break;
                }
            }
        } else {
            _construct_array(init_list);
        }
    }

    template <class... Args>
    void _construct_string(Args &&...args)
    {
        _type = Type::String;
        std::ranges::construct_at(
            std::addressof(_string), _new<String>(std::forward<Args>(args)...));
    }

    template <class... Args>
    void _construct_array(Args &&...args)
    {
        _type = Type::Array;
        std::ranges::construct_at(
            std::addressof(_array), _new<Array>(std::forward<Args>(args)...));
    }

    template <class... Args>
    void _construct_object(Args &&...args)
    {
        _type = Type::Object;
        std::ranges::construct_at(
            std::addressof(_object), _new<Object>(std::forward<Args>(args)...));
    }

    void _construct_document(const BasicDocument &other)
    {
        switch (other._type) {
        case Type::Null:
            _construct(nullptr);
            break;
        case Type::Bool:
            _construct(other._bool);
            break;
        case Type::Int:
            _construct(other._int);
            break;
        case Type::Float:
            _construct(other._float);
            break;
        case Type::String:
            _construct(*other._string);
            break;
        case Type::Array:
            _construct(*other._array);
            break;
        case Type::Object:
            _construct(*other._object);
            break;
        }
    }

    void _construct_document(BasicDocument &&other) noexcept(
        std::allocator_traits<Allocator>::is_always_equal::value)
    {
        if (_alloc_equals(other._alloc)) {
            _move_members(std::move(other));
        } else {
            _move_values(std::move(other));
        }
    }

    void _destroy()
    {
        switch (_type) {
        case Type::String:
            _destroy_string();
            break;
        case Type::Array:
            _destroy_array();
            break;
        case Type::Object:
            _destroy_object();
            break;
        }
    }

    void _destroy_string()
    {
        _delete(_string);
        std::ranges::destroy_at(std::addressof(_string));
    }

    void _destroy_array()
    {
        _delete(_array);
        std::ranges::destroy_at(std::addressof(_array));
    }

    void _destroy_object()
    {
        _delete(_object);
        std::ranges::destroy_at(std::addressof(_object));
    }

    void _copy_values(const BasicDocument &other)
    {
        switch (_type) {
        case Type::Bool:
            _bool = other._bool;
            break;
        case Type::Int:
            _int = other._int;
            break;
        case Type::Float:
            _float = other._float;
            break;
        case Type::String:
            *_string = *other._string;
            break;
        case Type::Array:
            *_array = *other._array;
            break;
        case Type::Object:
            *_object = *other._object;
            break;
        }
    }

    void _move_members(BasicDocument &&other) noexcept
    {
        switch (other._type) {
        case Type::Bool:
            _bool = other._bool;
            break;
        case Type::Int:
            _int = other._int;
            break;
        case Type::Float:
            _float = other._float;
            break;
        case Type::String:
            std::ranges::construct_at(
                std::addressof(_string), std::move(other._string));
            std::ranges::destroy_at(std::addressof(other._string));
            break;
        case Type::Array:
            std::ranges::construct_at(
                std::addressof(_array), std::move(other._array));
            std::ranges::destroy_at(std::addressof(other._array));
            break;
        case Type::Object:
            std::ranges::construct_at(
                std::addressof(_object), std::move(other._object));
            std::ranges::destroy_at(std::addressof(other._object));
            break;
        }

        _type = other._type;
        other._type = Type::Null;
    }

    void _move_values(BasicDocument &&other)
    {
        switch (other._type) {
        case Type::Bool:
            _construct(other._bool);
            break;
        case Type::Int:
            _construct(other._int);
            break;
        case Type::Float:
            _construct(other._float);
            break;
        case Type::String:
            _construct(std::move(*other._string));
            other._destroy_string();
            break;
        case Type::Array:
            _construct(std::move(*other._array));
            other._destroy_array();
            break;
        case Type::Object:
            _construct(std::move(*other._object));
            other._destroy_object();
            break;
        }

        other._type = Type::Null;
    }

    template <class T>
    void _reset(T &&value)
    {
        _destroy();
        _construct(std::forward<T>(value));
    }

    template <class T>
    void _assign_string(T &&value)
    {
        if (is_string()) {
            *_string = std::forward<T>(value);
        } else {
            _reset(std::forward<T>(value));
        }
    }

    template <class T>
    void _assign_array(T &&value)
    {
        if (is_array()) {
            *_array = std::forward<T>(value);
        } else {
            _reset(std::forward<T>(value));
        }
    }

    template <class T>
    void _assign_object(T &&value)
    {
        if (is_object()) {
            *_object = std::forward<T>(value);
        } else {
            _reset(std::forward<T>(value));
        }
    }

    void _assign_document(const Document &other)
    {
        if constexpr (std::allocator_traits<Allocator>::
                          propagate_on_container_copy_assignment::value) {
            if (_alloc_equals(other._alloc) && _type == other._type) {
                _alloc = other._alloc;
                _copy_values(other);
            } else {
                _destroy();
                _alloc = other._alloc;
                _construct_document(other);
            }
        } else if (_type == other._type) {
            _copy_values(other);
        } else {
            _destroy();
            _copy_construct(other);
        }
    }

    void _assign_document(Document &&other)
    {
        if constexpr (std::allocator_traits<Allocator>::
                          propagate_on_container_move_assignment::value) {
            _destroy();
            _alloc = std::move(other._alloc);
            _move_construct(std::move(other));
        } else if (_alloc_equals(other._alloc)) {
            _destroy();
            _move_construct(std::move(other));
        } else if (_type == other._type) {
            _move_assign_by_value(std::move(other));
        } else {
            _destroy();
            _move_construct_by_value(std::move(other));
        }
    }

    template <class T>
    void _as_object_emplace_or_assign(T &&key, auto &value)
    {
        assert(is_object());

        std::string_view sv(key);

        if (as_object().contains(sv)) {
            auto it = as_object().find(sv);
            it->second.assign(value);
        } else {
            as_object().emplace(std::forward<T>(key), value);
        }
    }

    bool _alloc_equals(const Allocator &other) noexcept(
        std::allocator_traits<Allocator>::is_always_equal::value)
    {
        if constexpr (std::allocator_traits<
                          Allocator>::is_always_equal::value) {
            return true;
        } else {
            return _alloc = other;
        }
    }

    Allocator _alloc;
    Type _type;

    union {
        Bool _bool;
        Int _int;
        Float _float;
        AllocatorPointer<String> _string;
        AllocatorPointer<Array> _array;
        AllocatorPointer<Object> _object;
    };
};

template <class T>
inline bool operator==(const BasicDocument<T> &a, const BasicDocument<T> &b)
{
    if (a.type() == b.type()) {
        switch (a.type()) {
        case Type::Null:
            return true;
        case Type::Int:
            return a.as_int() == b.as_int();
        case Type::Float:
            return a.as_float() == b.as_float();
        case Type::String:
            return a.as_string() == b.as_string();
        case Type::Array:
            return a.as_array() == b.as_array();
        case Type::Object:
            return a.as_object() == b.as_object();
        }
    }

    return false;
}

template <class T, class StackAlloc = std::allocator<std::byte>,
          std::output_iterator<char> O>
inline O to_chars(const BasicDocument<T> &value, O out,
                  const StackAlloc &stack_alloc = StackAlloc{})
{
    BasicSerializer<BasicDocument<T>, StackAlloc> s{ stack_alloc };
    return s.serialize(value, std::move(out));
}

template <class T, class Alloc = std::allocator<char>,
          class StackAlloc = std::allocator<std::byte>>
inline std::basic_string<char, std::char_traits<char>, Alloc> //
to_string(const BasicDocument<T> &value, const Alloc &alloc = Alloc(),
          const StackAlloc &stack_alloc = StackAlloc())
{
    std::basic_string<char, std::char_traits<char>, Alloc> result;
    to_chars(value, result, stack_alloc);
    return result;
}

template <class CharT, class Traits, class T>
inline std::basic_ostream<CharT, Traits> &
operator<<(std::basic_ostream<CharT, Traits> &os, const BasicDocument<T> &value)
{
    to_chars(value, std::ostream_iterator<char>(os));
    return os;
}

} // namespace hlib::json

#endif

#ifndef HTL_INITIALIZER_H_
#define HTL_INITIALIZER_H_

#include <htl/json/common.h>

namespace htl::json {
namespace detail {

enum class DocumentInitType : std::uint8_t {
    Null,
    Bool,
    Int,
    Float,
    CharPtr,
    StringView,
    String,
    Array,
    Object,
    Document,
    StringRef,
    ArrayRef,
    ObjectRef,
    DocumentRef,
    InitList,
};

enum class StringInitType : std::uint8_t {
    Empty,
    CharPtr,
    StringView,
    String,
    StringRef,
};

} // namespace detail

template <class Allocator>
class DocumentInit {
public:
    using Null = json::Null;
    using Bool = json::Bool;
    using Int = json::Int;
    using Float = json::Float;
    using String = json::BasicString<Allocator>;
    using Array = json::BasicArray<Allocator>;
    using Object = json::BasicObject<Allocator>;
    using Document = json::BasicDocument<Allocator>;

    DocumentInit() : _type{ DocumentInitType::Null } {}

    DocumentInit(Null) : _type{ DocumentInitType::Null } {}

    DocumentInit(Bool value) : _type{ DocumentInitType::Bool }, _bool{ value }
    {}

    DocumentInit(std::integral auto value)
        : _type{ DocumentInitType::Int }, _int(value)
    {}

    DocumentInit(std::floating_point auto value)
        : _type{ DocumentInitType::Float }, _float(value)
    {}

    DocumentInit(const char *value)
        : _type{ DocumentInitType::CharPtr }, _char_ptr{ value }
    {}

    DocumentInit(std::string_view value)
        : _type{ DocumentInitType::StringView }, _string_view{ value }
    {}

    DocumentInit(String &&value)
        : _type{ DocumentInitType::String }, _string{ std::move(value) }
    {}

    DocumentInit(Array &&value)
        : _type{ DocumentInitType::Array }, _array(std::move(value))
    {}

    DocumentInit(Object &&value)
        : _type{ DocumentInitType::Object }, _object(std::move(value))
    {}

    DocumentInit(Document &&value)
        : _type{ DocumentInitType::Document }, _document(std::move(value))
    {}

    DocumentInit(const String &value)
        : _type{ DocumentInitType::StringRef },
          _string_ref{ std::addressof(value) }
    {}

    DocumentInit(const Array &value)
        : _type{ DocumentInitType::ArrayRef },
          _array_ref{ std::addressof(value) }
    {}

    DocumentInit(const Object &value)
        : _type{ DocumentInitType::ObjectRef },
          _object_ref{ std::addressof(value) }
    {}

    DocumentInit(const Document &value)
        : _type{ DocumentInitType::DocumentRef },
          _document_ref{ std::addressof(value) }
    {}

    DocumentInit(std::initializer_list<DocumentInit> init_list)
        : _type{ DocumentInitType::InitList }, _init_list(init_list)
    {}

    ~DocumentInit()
    {
        switch (_type) {
        case DocumentInitType::String:
            std::ranges::destroy_at(std::addressof(_string));
            break;
        case DocumentInitType::Array:
            std::ranges::destroy_at(std::addressof(_array));
            break;
        case DocumentInitType::Object:
            std::ranges::destroy_at(std::addressof(_object));
            break;
        case DocumentInitType::Document:
            std::ranges::destroy_at(std::addressof(_document));
            break;
        }
    }

private:
    using DocumentInitType = detail::DocumentInitType;

    friend Array;
    friend Object;
    friend Document;

    bool _is_entry_like() const
    {
        return _type == DocumentInitType::InitList && _init_list.size() == 2 &&
               _init_list.begin()[0]._is_string_like();
    }

    bool _is_string_like() const
    {
        switch (_type) {
        case DocumentInitType::CharPtr:
        case DocumentInitType::StringView:
        case DocumentInitType::String:
        case DocumentInitType::StringRef:
            return true;
        default:
            return false;
        }
    }

    DocumentInitType _type;

    union {
        Bool _bool;
        Int _int;
        Float _float;
        const char *_char_ptr;
        std::string_view _string_view;
        String _string;
        Array _array;
        Object _object;
        Document _document;
        const String *_string_ref;
        const Array *_array_ref;
        const Object *_object_ref;
        const Document *_document_ref;
        std::initializer_list<DocumentInit> _init_list;
    };
};

template <class Allocator>
class StringInit {
public:
    using Null = json::Null;
    using Bool = json::Bool;
    using Int = json::Int;
    using Float = json::Float;
    using String = json::BasicString<Allocator>;
    using Array = json::BasicArray<Allocator>;
    using Object = json::BasicObject<Allocator>;
    using Document = json::BasicDocument<Allocator>;

    StringInit(Null) = delete;

    StringInit(const char *value)
        : _type{ StringInitType::CharPtr }, _char_ptr{ value }
    {}

    StringInit(std::string_view value)
        : _type{ StringInitType::StringView }, _string_view{ value }
    {}

    StringInit(String &&value)
        : _type{ StringInitType::String }, _string{ std::move(value) }
    {}

    StringInit(const String &value)
        : _type{ StringInitType::StringRef },
          _string_ref{ std::addressof(value) }
    {}

    ~StringInit()
    {
        switch (_type) {
        case StringInitType::String:
            std::ranges::destroy_at(std::addressof(_string));
            break;
        }
    }

private:
    friend Array;
    friend Object;
    friend Document;

    using StringInitType = detail::StringInitType;

    StringInitType _type;

    union {
        const char *_char_ptr;
        std::string_view _string_view;
        String _string;
        const String *_string_ref;
    };
};

template <class Allocator>
using ItemInit = std::pair<StringInit<Allocator>, DocumentInit<Allocator>>;

} // namespace htl::json

#endif

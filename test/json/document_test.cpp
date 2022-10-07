#include <gtest/gtest.h>
#include <htl/json/document.h>
#include "../util/allocator.h"

namespace htl::test {

using namespace htl::json;

template <class Allocator>
static void test_document_is_type(BasicDocument<Allocator> &alloc, Type type)
{
    ASSERT_EQ(alloc.type(), type);
    ASSERT_EQ(alloc.is_null(), type == Type::Null);
    ASSERT_EQ(alloc.is_bool(), type == Type::Bool);
    ASSERT_EQ(alloc.is_int(), type == Type::Int);
    ASSERT_EQ(alloc.is_float(), type == Type::Float);
    ASSERT_EQ(alloc.is_string(), type == Type::String);
    ASSERT_EQ(alloc.is_array(), type == Type::Array);
    ASSERT_EQ(alloc.is_object(), type == Type::Object);
}

TEST(JSONDocumentTest, DefaultInit)
{
    using Document = BasicDocument<DefaultInitAllocator<std::byte>>;

    alignas(Document) std::uint8_t buffer[sizeof(Document)];

    std::ranges::fill(buffer, ~0);

    auto *value = new (buffer) Document;

    test_document_is_type(*value, Type::Null);
    ASSERT_EQ(value->get_allocator().state, 0);
}

TEST(JSONDocumentTest, ValueInit)
{
    using Document = BasicDocument<DefaultInitAllocator<std::byte>>;

    alignas(Document) std::uint8_t buffer[sizeof(Document)];

    std::ranges::fill(buffer, ~0);

    auto *value = new (buffer) Document();

    test_document_is_type(*value, Type::Null);
    ASSERT_EQ(value->get_allocator().state, 0);
}

TEST(JSONDocumentTest, AllocatorContructor)
{
    BasicDocument value{ IdentityAllocator<std::byte>{ 10 } };

    test_document_is_type(value, Type::Null);
    ASSERT_EQ(value.get_allocator().id, 10);
}

TEST(JSONDocumentTest, NullContructor)
{
    BasicDocument value{ nullptr, IdentityAllocator<std::byte>{ 10 } };

    test_document_is_type(value, Type::Null);
    ASSERT_EQ(value.get_allocator().id, 10);
}

TEST(JSONDocumentTest, BoolConstructor)
{
    BasicDocument value{ true, IdentityAllocator<std::byte>{ 10 } };

    test_document_is_type(value, Type::Bool);
    ASSERT_EQ(value.get_allocator().id, 10);
    ASSERT_EQ(value.as_bool(), true);
}

template <class T>
static void test_int_constructor()
{
    BasicDocument value{ T{ 10 }, IdentityAllocator<std::byte>{ 10 } };

    test_document_is_type(value, Type::Int);
    ASSERT_EQ(value.get_allocator().id, 10);
    ASSERT_EQ(value.as_int(), 10);
}

TEST(JSONDocumentTest, IntContructor)
{
    test_int_constructor<short>();
    test_int_constructor<int>();
    test_int_constructor<long>();
    test_int_constructor<long long>();
    test_int_constructor<unsigned short>();
    test_int_constructor<unsigned int>();
    test_int_constructor<unsigned long>();
    test_int_constructor<unsigned long long>();
}

template <class T>
static void test_float_constructor()
{
    BasicDocument value{ T{ 10 }, IdentityAllocator<std::byte>{ 10 } };

    test_document_is_type(value, Type::Float);
    ASSERT_EQ(value.get_allocator().id, 10);
    ASSERT_EQ(value.as_float(), 10);
}

TEST(JSONDocumentTest, FloatConstructor)
{
    test_float_constructor<float>();
    test_float_constructor<double>();
    test_float_constructor<long double>();
}

TEST(JSONDocumentTest, CharPtrConstructor)
{
    const char data[]{ "abc123" };
    BasicDocument value{ data, IdentityAllocator<std::byte>{ 10 } };

    test_document_is_type(value, Type::String);
    ASSERT_EQ(value.get_allocator().id, 10);
    ASSERT_EQ(value.as_string(), data);
}

TEST(JSONDocumentTest, StringViewConstructor)
{
    std::string_view data{ "abc123" };
    BasicDocument value{ data, IdentityAllocator<std::byte>{ 10 } };

    test_document_is_type(value, Type::String);
    ASSERT_EQ(value.get_allocator().id, 10);
    ASSERT_EQ(value.as_string(), data);
}

TEST(JSONDocumentTest, StringCopyConstructor)
{
    using Allocator = IdentityAllocator<std::byte>;
    using Document = BasicDocument<Allocator>;
    using String = BasicString<Allocator>;

    String data{ "abc123" };
    Document value{ data, Allocator{ 10 } };

    test_document_is_type(value, Type::String);
    ASSERT_EQ(value.get_allocator().id, 10);
    ASSERT_EQ(value.as_string(), data);
}

TEST(JSONDocumentTest, StringMoveConstructor)
{
    using Allocator = IdentityAllocator<std::byte>;
    using Document = BasicDocument<Allocator>;
    using String = BasicString<Allocator>;

    String data{ "abc123" };
    String src{ data };
    Document value{ std::move(src), Allocator{ 10 } };

    test_document_is_type(value, Type::String);
    ASSERT_EQ(value.get_allocator().id, 10);
    ASSERT_EQ(value.as_string(), data);
}

TEST(JSONDocumentTest, ArrayCopyConstructor)
{
    using Allocator = IdentityAllocator<std::byte>;
    using Document = BasicDocument<Allocator>;
    using Array = BasicArray<Allocator>;

    Array data{ 1, 2, 3 };
    Document value{ data, Allocator{ 10 } };

    test_document_is_type(value, Type::Array);
    ASSERT_EQ(value.get_allocator().id, 10);
    ASSERT_EQ(value.as_array(), data);
}

TEST(JSONDocumentTest, ArrayMoveConstructor)
{
    using Allocator = IdentityAllocator<std::byte>;
    using Document = BasicDocument<Allocator>;
    using Array = BasicArray<Allocator>;

    Array data{ 1, 2, 3 };
    Array src(data);
    Document value{ std::move(src), Allocator{ 10 } };

    test_document_is_type(value, Type::Array);
    ASSERT_EQ(value.get_allocator().id, 10);
    ASSERT_EQ(value.as_array(), data);
}

TEST(JSONDocumentTest, ObjectCopyConstructor)
{
    using Allocator = IdentityAllocator<std::byte>;
    using Document = BasicDocument<Allocator>;
    using Object = BasicObject<Allocator>;

    Object data{
        { "a", nullptr },
        { "b", 123 },
        { "c", "value" },
    };

    Document value{ data, Allocator{ 10 } };

    test_document_is_type(value, Type::Object);
    ASSERT_EQ(value.get_allocator().id, 10);
    ASSERT_EQ(value.as_object(), data);
}

TEST(JSONDocumentTest, ObjectMoveConstructor)
{
    using Allocator = IdentityAllocator<std::byte>;
    using Document = BasicDocument<Allocator>;
    using Object = BasicObject<Allocator>;

    Object data{
        { "a", nullptr },
        { "b", 123 },
        { "c", "value" },
    };

    Object src(data);
    Document value{ std::move(src), Allocator{ 10 } };

    test_document_is_type(value, Type::Object);
    ASSERT_EQ(value.get_allocator().id, 10);
    ASSERT_EQ(value.as_object(), data);
}

} // namespace htl::test

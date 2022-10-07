// #include <vector>
// #include <memory_resource>
// #include <gtest/gtest.h>
// #include <hlib/json.h>
// #include <hlib/test/allocator.h>

// namespace hlib::test {

// using namespace hlib::json;

// TEST(BasicDocumentTest, DefaultConstructor)
// {
//     using allocator_type = DefaultInitAllocator<std::byte>;
//     using document_type = BasicDocument<allocator_type>;

//     alignas(document_type) std::uint8_t buffer[sizeof(document_type)];

//     { // Default initializtaion
//         std::ranges::fill(buffer, ~0);

//         document_type *document = new (buffer) document_type;

//         ASSERT_TRUE(document->is_null());
//         ASSERT_EQ(document->get_allocator().state, 0);

//         std::destroy_at(document);
//     }

//     { // Value initialization
//         std::ranges::fill(buffer, ~0);

//         document_type *document = new (buffer) document_type{};

//         ASSERT_TRUE(document->is_null());
//         ASSERT_EQ(document->get_allocator().state, 0);

//         std::destroy_at(document);
//     }
// }

// TEST(BasicDocumentTest, AllocatorConstructor)
// {
//     using allocator_type = IdentityAllocator<std::byte>;
//     using document_type = BasicDocument<allocator_type>;

//     document_type document{ allocator_type{ 1 } };

//     ASSERT_TRUE(document.is_null());
//     ASSERT_EQ(document.get_allocator().id, 1);
// }

// TEST(BasicDocumentTest, NullValueConstructor)
// {
//     using allocator_type = IdentityAllocator<std::byte>;
//     using document_type = BasicDocument<allocator_type>;

//     document_type document{ nullptr, allocator_type{ 1 } };

//     ASSERT_TRUE(document.is_null());
//     ASSERT_EQ(document.get_allocator().id, 1);
// }

// TEST(BasicDocumentTest, BoolValueConstructor)
// {
//     using allocator_type = IdentityAllocator<std::byte>;
//     using document_type = BasicDocument<allocator_type>;

//     document_type document{ true, allocator_type{ 1 } };

//     ASSERT_TRUE(document.is_bool());
//     ASSERT_EQ(document.as_bool(), true);
//     ASSERT_EQ(document.get_allocator().id, 1);
// }

// TEST(BasicDocumentTest, IntValueConstructor)
// {
//     using allocator_type = IdentityAllocator<std::byte>;
//     using document_type = BasicDocument<allocator_type>;

//     document_type document{ 123, allocator_type{ 1 } };

//     ASSERT_TRUE(document.is_int());
//     ASSERT_EQ(document.as_int(), 123);
//     ASSERT_EQ(document.get_allocator().id, 1);
// }

// TEST(BasicDocumentTest, FloatValueConstructor)
// {
//     using allocator_type = IdentityAllocator<std::byte>;
//     using document_type = BasicDocument<allocator_type>;

//     document_type document{ 123.123, allocator_type{ 1 } };

//     ASSERT_TRUE(document.is_float());
//     ASSERT_EQ(document.as_float(), 123.123);
//     ASSERT_EQ(document.get_allocator().id, 1);
// }

// TEST(BasicDocumentTest, CharPointerConstructor)
// {
//     using allocator_type = IdentityAllocator<std::byte>;
//     using document_type = BasicDocument<allocator_type>;

//     const char *str = "str";

//     document_type document{ str, allocator_type{ 1 } };

//     ASSERT_TRUE(document.is_string());
//     ASSERT_EQ(document.as_string(), str);
//     ASSERT_EQ(document.get_allocator().id, 1);
// }

// TEST(BasicDocumentTest, StringViewConstructor)
// {
//     using allocator_type = IdentityAllocator<std::byte>;
//     using document_type = BasicDocument<allocator_type>;

//     std::string_view str = "str";

//     document_type document{ str, allocator_type{ 1 } };

//     ASSERT_TRUE(document.is_string());
//     ASSERT_EQ(document.as_string(), str);
//     ASSERT_EQ(document.get_allocator().id, 1);
// }

// TEST(BasicDocumentTest, StringValueConstructor)
// {
//     using allocator_type = IdentityAllocator<std::byte>;
//     using document_type = BasicDocument<allocator_type>;
//     using string_type = document_type::String;

//     string_type str{ "str" };
//     string_type str_copy{ str };

//     document_type document{ std::move(str), allocator_type{ 1 } };

//     ASSERT_TRUE(document.is_string());
//     ASSERT_EQ(document.as_string(), str_copy);
//     ASSERT_EQ(document.get_allocator().id, 1);
// }

// TEST(BasicDocumentTest, ObjectValueConstructor)
// {
//     using allocator_type = IdentityAllocator<std::byte>;
//     using document_type = BasicDocument<allocator_type>;
//     using object_type = document_type::Object;

//     // object_type obj{ { "key1", 123 }, { "key2", 123 } };
//     // object_type str_copy{ str };

//     // document_type document{ std::move(str), allocator_type{ 1 } };

//     // ASSERT_TRUE(document.is_string());
//     // ASSERT_EQ(document.as_string(), str_copy);
//     // ASSERT_EQ(document.get_allocator().id, 1);
// }

// // TEST(BasicDocumentTest, AllocatorConstructAllocator)
// // {
// //     BasicDocument document{ IdentityAllocator<std::byte>{ 1 } };

// //     ASSERT_EQ(document.get_allocator().id, 1);
// //     ASSERT_EQ(document.get_type(), Type::Null);
// //     ASSERT_TRUE(document.is_null());
// // }

// // TEST(BasicDocumentTest, AllocatorConstructNull)
// // {
// //     BasicDocument document{ nullptr, IdentityAllocator<std::byte>{ 1 } };

// //     ASSERT_EQ(document.get_allocator().id, 1);
// //     ASSERT_EQ(document.get_type(), Type::Null);
// //     ASSERT_TRUE(document.is_null());
// // }

// // TEST(BasicDocumentTest, AllocatorConstructBool)
// // {
// //     BasicDocument document{ true, IdentityAllocator<std::byte>{ 1 } };

// //     ASSERT_EQ(document.get_allocator().id, 1);
// //     ASSERT_EQ(document.get_type(), Type::Bool);
// //     ASSERT_TRUE(document.is_bool());
// //     ASSERT_EQ(document.as_bool(), true);
// // }

// // TEST(BasicDocumentTest, AllocatorConstructInt)
// // {
// //     BasicDocument document{ 123, IdentityAllocator<std::byte>{ 1 } };

// //     ASSERT_EQ(document.get_allocator().id, 1);
// //     ASSERT_EQ(document.get_type(), Type::Int);
// //     ASSERT_TRUE(document.is_int());
// //     ASSERT_EQ(document.as_int(), 123);
// // }

// // TEST(BasicDocumentTest, AllocatorConstructFloat)
// // {
// //     BasicDocument document{ 123.123, IdentityAllocator<std::byte>{ 1 } };

// //     ASSERT_EQ(document.get_allocator().id, 1);
// //     ASSERT_EQ(document.get_type(), Type::Float);
// //     ASSERT_TRUE(document.is_float());
// //     ASSERT_EQ(document.as_float(), 123.123);
// // }

// // TEST(BasicDocumentTest, AllocatorCopyConstructNonPropagating)
// // {
// //     using allocator_type = PropagatingAllocator<std::byte, false>;

// //     BasicDocument document{ allocator_type{ 1 } };
// //     BasicDocument document_copy{ document };

// //     ASSERT_EQ(document.get_allocator().id, 1);
// //     ASSERT_EQ(document_copy.get_allocator().id, 0);
// // }

// // TEST(BasicDocumentTest, AllocatorCopyConstructPropagating)
// // {
// //     using allocator_type = PropagatingAllocator<std::byte, true>;

// //     BasicDocument document{ allocator_type{ 1 } };
// //     BasicDocument document_copy{ document };

// //     ASSERT_EQ(document.get_allocator().id, 1);
// //     ASSERT_EQ(document_copy.get_allocator().id, 1);
// // }

// // TEST(BasicDocumentTest, AllocatorExtendedCopyConstructPropagating)
// // {
// //     using allocator_type = PropagatingAllocator<std::byte, true>;

// //     BasicDocument document{ allocator_type{ 1 } };
// //     BasicDocument document_copy{ document, allocator_type{ 2 } };
// //     ASSERT_EQ(document.get_allocator().id, 1);
// //     ASSERT_EQ(document_copy.get_allocator().id, 2);
// // }

// // TEST(BasicDocumentTest, AllocatorMoveConstruct)
// // {
// //     using allocator_type = PropagatingAllocator<std::byte, true>;

// //     BasicDocument document{ allocator_type{ 1 } };
// //     BasicDocument document_copy{ std::move(document) };
// //     ASSERT_EQ(document.get_allocator().id, 1);
// //     ASSERT_EQ(document_copy.get_allocator().id, 1);
// // }

// // TEST(BasicDocumentTest, AllocatorExtendedMoveConstruct)
// // {
// //     using allocator_type = PropagatingAllocator<std::byte, true>;

// //     BasicDocument document{ allocator_type{ 1 } };
// //     BasicDocument document_copy{ std::move(document), allocator_type{ 2 } };
// //     ASSERT_EQ(document.get_allocator().id, 1);
// //     ASSERT_EQ(document_copy.get_allocator().id, 2);
// // }

// // TEST(BasicDocumentTest, CopyConstructNull)
// // {
// //     Document document{};
// //     Document other{ document };

// //     ASSERT_TRUE(other.is_null());
// // }

// // TEST(BasicDocumentTest, CopyConstructBool)
// // {
// //     Document document{ true };
// //     Document other{ document };

// //     ASSERT_TRUE(other.is_bool());
// //     ASSERT_EQ(other.as_bool(), true);
// // }

// // TEST(BasicDocumentTest, CopyConstructInt)
// // {
// //     Document document{ 123 };
// //     Document other{ document };

// //     ASSERT_TRUE(other.is_int());
// //     ASSERT_EQ(other.as_int(), 123);
// // }

// // TEST(BasicDocumentTest, CopyConstructFloat)
// // {
// //     Document document{ 123.123 };
// //     Document other{ document };

// //     ASSERT_TRUE(other.is_float());
// //     ASSERT_EQ(other.as_float(), 123.123);
// // }

// // TEST(BasicDocumentTest, CopyConstructString)
// // {
// //     Document document{ "str" };
// //     Document other{ document };

// //     ASSERT_TRUE(other.is_string());
// //     ASSERT_EQ(other.as_string(), "str");
// // }

// // TEST(BasicDocumentTest, CopyConstructArray)
// // {
// //     GTEST_SKIP();
// // }

// // TEST(BasicDocumentTest, CopyConstructObject)
// // {
// //     GTEST_SKIP();
// // }

// // TEST(BasicDocumentTest, MoveConstructNull)
// // {
// //     Document document{};
// //     Document other{ std::move(document) };

// //     ASSERT_TRUE(document.is_null());
// //     ASSERT_TRUE(other.is_null());
// // }

// // TEST(BasicDocumentTest, MoveConstructBool)
// // {
// //     Document document{ true };
// //     Document other{ std::move(document) };

// //     ASSERT_TRUE(document.is_null());
// //     ASSERT_TRUE(other.is_bool());
// //     ASSERT_EQ(other.as_bool(), true);
// // }

// // TEST(BasicDocumentTest, MoveConstructtInt)
// // {
// //     Document document{ 123 };
// //     Document other{ std::move(document) };

// //     ASSERT_TRUE(document.is_null());
// //     ASSERT_TRUE(other.is_int());
// //     ASSERT_EQ(other.as_int(), 123);
// // }

// // TEST(BasicDocumentTest, MoveConstructloat)
// // {
// //     Document document{ 123.123 };
// //     Document other{ std::move(document) };

// //     ASSERT_TRUE(document.is_null());
// //     ASSERT_TRUE(other.is_float());
// //     ASSERT_EQ(other.as_float(), 123.123);
// // }

// // TEST(BasicDocumentTest, MoveConstructString)
// // {
// //     GTEST_SKIP();
// // }

// // TEST(BasicDocumentTest, MoveConstructArray)
// // {
// //     GTEST_SKIP();
// // }

// // TEST(BasicDocumentTest, MoveConstructObject)
// // {
// //     GTEST_SKIP();
// // }

// } // namespace hlib::test

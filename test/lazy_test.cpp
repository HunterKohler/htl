#include <gtest/gtest.h>
#include <hlib/lazy.h>

namespace hlib::test {

int global_value = 0;

int increment_global()
{
    return global_value++;
}

TEST(LazyTest, StoresFunctionResult)
{
    Lazy lazy(increment_global);

    ASSERT_EQ(lazy.value(), 0);
    ASSERT_EQ(std::as_const(lazy).value(), 0);
    ASSERT_EQ(global_value, 1);
}

TEST(LazyTest, StoresLambdaResultByReference)
{
    auto value = 0;
    auto func = [&]() { return value++; };
    Lazy lazy(func);

    ASSERT_EQ(lazy.value(), 0);
    ASSERT_EQ(std::as_const(lazy).value(), 0);
    ASSERT_EQ(value, 1);
}

TEST(LazyTest, StoresLambdaResultByValue)
{
    auto value = 0;
    auto func = [=]() mutable { return ++value; };
    Lazy lazy(func);

    ASSERT_EQ(lazy.value(), 1);
    ASSERT_EQ(std::as_const(lazy).value(), 1);
    ASSERT_EQ(value, 0);
}

TEST(LazyTest, StoresReferenceResultType)
{
    auto value = 0;
    auto func = [&]() -> decltype(auto) { return value; };

    Lazy lazy(func);

    static_assert(std::same_as<decltype(lazy.value()), int &>);

    ASSERT_EQ(&lazy.value(), &value);
    ASSERT_EQ(&std::as_const(lazy).value(), &value);

    lazy.value()++;

    ASSERT_EQ(value, 1);
}

} // namespace hlib::test

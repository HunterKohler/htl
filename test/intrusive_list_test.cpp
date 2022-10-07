#include <vector>
#include <functional>
#include <utility>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <htl/intrusive_list.h>

namespace htl::test {

using testing::ElementsAre;
using testing::ElementsAreArray;
using testing::Ref;
using testing::Pointee;

struct IdNode : IntrusiveListNode<IdNode> {};

using IdList = IntrusiveList<IdNode>;

inline bool operator==(const IdNode &a, const IdNode &b)
{
    return std::addressof(a) && std::addressof(b);
}

static_assert(std::movable<IdNode>);
static_assert(std::movable<IdList>);
static_assert(
    std::constructible_from<IdList::const_iterator, IdList::iterator>);
static_assert(std::constructible_from<IdList::iterator, IdNode &>);
static_assert(std::constructible_from<IdList::const_iterator, const IdNode &>);

inline constexpr auto RefView = std::views::transform(Ref<IdNode>);

TEST(IntrusiveListTest, DefaultConstruct)
{
    alignas(IdList) std::byte buffer[sizeof(IdList)];
    std::ranges::fill(buffer, ~std::byte{});
    IdList *list = new (buffer) IdList;

    ASSERT_TRUE(list->empty());
}

TEST(IntrusiveListTest, ValueConstruct)
{
    alignas(IdList) std::byte buffer[sizeof(IdList)];
    std::ranges::fill(buffer, ~std::byte{});
    IdList *list = new (buffer) IdList();

    ASSERT_TRUE(list->empty());
}

TEST(IntruiveListTest, RangeConstructor)
{
    IdNode n1, nodes[1000];
    IdList l1(std::views::empty<IdNode>), l2(std::to_array({ std::ref(n1) })),
        l3(nodes);

    ASSERT_THAT(l1, ElementsAre());
    ASSERT_THAT(l2, ElementsAre(Ref(n1)));
    ASSERT_THAT(l3, ElementsAreArray(nodes | RefView));
}

TEST(IntrusiveListTest, InitializerListContructor)
{
    IdNode n1, n2, n3, n4;
    IdList l1{}, l2{ n1 }, l3{ n2, n3, n4 };

    ASSERT_THAT(l1, ElementsAre());
    ASSERT_THAT(l2, ElementsAre(Ref(n1)));
    ASSERT_THAT(l3, ElementsAre(Ref(n2), Ref(n3), Ref(n4)));
}

TEST(IntrusiveListTest, MoveConstructor)
{
    IdNode n1, n2, n3;
    IdList l1{ n1, n2, n3 }, l2, l3(std::move(l1)), l4(std::move(l2));

    ASSERT_THAT(l3, ElementsAre(Ref(n1), Ref(n2), Ref(n3)));
    ASSERT_THAT(l4, ElementsAre());
}

TEST(IntrusiveListTest, MoveAssignment)
{
    IdNode n1, n2, n3;
    IdList l1{ n1, n2, n3 }, l2, l3, l4;

    l3 = std::move(l1);
    l4 = std::move(l2);

    ASSERT_THAT(l3, ElementsAre(Ref(n1), Ref(n2), Ref(n3)));
    ASSERT_THAT(l4, ElementsAre());
}

TEST(IntrusiveListTest, PushBack)
{
    IdNode n1, n2, n3, nodes[1000];
    IdList l1, l2, l3;

    l1.push_back(n1);
    l2.push_back(n2);
    l2.push_back(n3);

    for (auto &node: nodes) {
        l3.push_back(node);
    }

    ASSERT_THAT(l1, ElementsAre(Ref(n1)));
    ASSERT_THAT(l2, ElementsAre(Ref(n2), Ref(n3)));
    ASSERT_THAT(l3, ElementsAreArray(nodes | RefView));
}

TEST(IntrusiveListTest, PushFront)
{
    IdNode n1, n2, n3, nodes[1000];
    IdList l1, l2, l3;

    l1.push_front(n1);
    l2.push_front(n2);
    l2.push_front(n3);

    for (auto &node: nodes) {
        l3.push_front(node);
    }

    ASSERT_THAT(l1, ElementsAre(Ref(n1)));
    ASSERT_THAT(l2, ElementsAre(Ref(n3), Ref(n2)));
    ASSERT_THAT(l3, ElementsAreArray(nodes | std::views::reverse | RefView));
}

TEST(IntrusiveListTest, PopBack)
{
    IdNode n1, n2, n3, nodes[1000];
    IdList l1{ n1 }, l2{ n2, n3 }, l3(nodes);
    std::size_t drop = std::size(nodes) / 4;
    std::size_t take = std::size(nodes) - drop;

    l1.pop_back();
    l2.pop_back();

    for (int i = 0; i < drop; i++) {
        l3.pop_back();
    }

    ASSERT_THAT(l1, ElementsAre());
    ASSERT_THAT(l2, ElementsAre(Ref(n2)));
    ASSERT_THAT(l3, ElementsAreArray(nodes | std::views::take(take) | RefView));
}

TEST(IntrusiveListTest, PopFront)
{
    IdNode n1, n2, n3, nodes[1000];
    IdList l1{ n1 }, l2{ n2, n3 }, l3(nodes);
    std::size_t drop = std::size(nodes) / 4;

    l1.pop_front();
    l2.pop_front();

    for (int i = 0; i < drop; i++) {
        l3.pop_front();
    }

    ASSERT_THAT(l1, ElementsAre());
    ASSERT_THAT(l2, ElementsAre(Ref(n3)));
    ASSERT_THAT(l3, ElementsAreArray(nodes | std::views::drop(drop) | RefView));
}

TEST(IntrusiveListTest, InsertNode)
{
    IdNode n1, n2, n3;
    IdList l1{ n1, n2 };

    auto it = l1.insert(std::ranges::next(l1.begin(), 1), n3);

    ASSERT_THAT(*it, Ref(n3));
    ASSERT_THAT(l1, ElementsAre(Ref(n1), Ref(n3), Ref(n2)));
}

TEST(IntrusiveListTest, InsertRange)
{
    IdNode n1, n2, nodes[1000];
    IdList l1{ n1, n2 };

    auto mid = std::ranges::next(l1.begin(), 1);

    ASSERT_EQ(l1.insert(mid, std::ranges::empty_view<IdNode>()), mid);

    auto ins = l1.insert(mid, nodes);
    auto cursor = l1.begin();

    ASSERT_THAT(*ins, Ref(nodes[0]));
    ASSERT_THAT(*cursor++, Ref(n1));

    for (auto &node: nodes) {
        ASSERT_THAT(*cursor++, Ref(node));
    }

    ASSERT_THAT(*cursor++, Ref(n2));
    ASSERT_EQ(cursor, l1.end());
}

TEST(IntrusiveListTest, InsertInitializerList)
{
    IdNode n1, n2, n3, n4, n5;
    IdList l1{ n1, n2 };

    auto mid = std::ranges::next(l1.begin(), 1);

    ASSERT_EQ(l1.insert(mid, {}), mid);

    auto ins = l1.insert(mid, { n3, n4, n5 });

    ASSERT_THAT(*ins, Ref(n3));
    ASSERT_THAT(l1, ElementsAre(Ref(n1), Ref(n3), Ref(n4), Ref(n5), Ref(n2)));
}

TEST(IntrusiveListTest, Front)
{
    IdNode n1, n2, n3;
    IdList l1{ n1, n2, n3 };

    ASSERT_THAT(l1.front(), Ref(n1));
}

TEST(IntrusiveListTest, Back)
{
    IdNode n1, n2, n3;
    IdList l1{ n1, n2, n3 };

    ASSERT_THAT(l1.back(), Ref(n3));
}

TEST(IntrusiveListTest, Iterators)
{
    IdNode n1, n2, n3;
    IdList l1{ n1, n2, n3 };

    auto test_iterators =
        [&](auto first, auto last,
            std::initializer_list<std::reference_wrapper<IdNode>> nodes) {
        ASSERT_EQ(std::ranges::distance(first, last), nodes.size());

        for (auto &node: nodes) {
            ASSERT_THAT(*first, Ref(node));
            ASSERT_THAT(std::to_address(first), Pointee(Ref(node)));

            ++first;
        }

        ASSERT_EQ(first, last);

        for (auto &node: nodes | std::views::reverse) {
            --first;

            ASSERT_THAT(*first, Ref(node));
            ASSERT_THAT(std::to_address(first), Pointee(Ref(node)));
        }
    };

    test_iterators(l1.begin(), l1.end(), { n1, n2, n3 });
    test_iterators(l1.cbegin(), l1.cend(), { n1, n2, n3 });
    test_iterators(l1.rbegin(), l1.rend(), { n3, n2, n1 });
    test_iterators(l1.crbegin(), l1.crend(), { n3, n2, n1 });
    test_iterators(
        std::as_const(l1).begin(), std::as_const(l1).end(), { n1, n2, n3 });
    test_iterators(
        std::as_const(l1).rbegin(), std::as_const(l1).rend(), { n3, n2, n1 });
}

} // namespace htl::test

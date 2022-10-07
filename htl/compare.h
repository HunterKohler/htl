#ifndef HTL_COMPARE_H_
#define HTL_COMPARE_H_

#include <compare>
#include <concepts>
#include <type_traits>
#include <htl/concepts.h>

namespace htl {
namespace detail {

// clang-format off
struct SynthThreeWayFn {
public:
    template <class T, class U>
    [[nodiscard]] constexpr auto operator()(const T &t, const U &u) const
        noexcept(std::three_way_comparable_with<T, U>
            ? noexcept(t <=> u)
            : noexcept(t < u) && noexcept(u < t))
        requires requires {
            { t < u } -> BooleanTestable;
            { u < t } -> BooleanTestable;
        }
    {
        if constexpr (std::three_way_comparable_with<T, U>) {
            return t <=> u;
        } else if (t < u) {
            return std::weak_ordering::less;
        } else if (u < t) {
            return std::weak_ordering::greater;
        } else {
            return std::weak_ordering::equivalent;
        }
    }
};
// clang-format on

} // namespace detail

inline constexpr detail::SynthThreeWayFn synth_three_way{};

template <class T, class U = T>
using SynthThreeWayResult =
    decltype(synth_three_way(std::declval<T &>(), std::declval<U &>()));

// clang-format off
template <class T, class Cat = std::partial_ordering>
concept SynthThreeWayComparable =
    requires (const std::remove_reference_t<T> &a,
              const std::remove_reference_t<T> &b) {
        { a < b } -> BooleanTestable;
        { b < a } -> BooleanTestable;
    } && std::same_as<std::common_comparison_category_t<Cat,
        SynthThreeWayResult<T>>, Cat>;

template <class T, class U, class Cat = std::partial_ordering>
concept SynthThreeWayComparableWith =
    SynthThreeWayComparable<T, Cat> &&
    SynthThreeWayComparable<U, Cat> &&
    std::common_reference_with<
        const std::remove_reference_t<T> &,
        const std::remove_reference_t<U> &> &&
    SynthThreeWayComparable<
        std::common_reference_t<
            const std::remove_reference_t<T> &,
            const std::remove_reference_t<U> &>, Cat>;
// clang-format on

} // namespace htl

#endif

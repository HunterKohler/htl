#ifndef HTL_LAZY_H_
#define HTL_LAZY_H_

#include <concepts>
#include <functional>
#include <atomic>
#include <optional>
#include <type_traits>
#include <utility>

namespace htl {

template <class T>
    requires std::is_object_v<T> && std::is_move_constructible_v<T>
class DefaultLazyInit {
public:
    constexpr T operator()() noexcept(noexcept(T()))
    {
        return T();
    }
};

template <class T, class F = DefaultLazyInit<T>>
concept LazyInitializable =
    std::invocable<F> && std::convertible_to<std::invoke_result_t<F>, T> &&
    !std::is_pointer_v<F> &&
    !std::is_void_v<T>;

template <class T, std::invocable F = DefaultLazyInit<T>>
    requires LazyInitializable<T, F>
class Lazy {
public:
    using value_type = std::remove_reference_t<T>;

    constexpr Lazy() //
        noexcept(std::is_nothrow_default_constructible_v<F>) //
        requires(std::is_default_constructible_v<F>)
        : _fn(), _box()
    {}

    constexpr explicit Lazy(const F &fn) //
        noexcept(std::is_nothrow_copy_constructible_v<F> ||
                 std::is_function_v<F>) //
        requires(std::is_copy_constructible_v<F> || std::is_function_v<F>)
        : _fn(fn), _box()
    {}

    constexpr explicit Lazy(F &&fn) //
        noexcept(std::is_nothrow_move_constructible_v<F>) //
        requires(std::is_move_constructible_v<F>)
        : _fn(std::move(fn)), _box()
    {}

    template <class... Args>
        requires(std::constructible_from<F, Args...>)
    constexpr explicit Lazy(std::in_place_t, Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<F, Args...>)
        : _fn(std::forward<Args>(args)...), _box()
    {}

    template <class U, class... Args>
        requires(
            std::constructible_from<F, std::initializer_list<U> &, Args...>)
    constexpr Lazy(std::in_place_t, std::initializer_list<U> ilist,
                   Args &&...args) //
        noexcept(std::is_nothrow_constructible_v<F, Args...>)
        : _fn(ilist, std::forward<Args>(args)...), _box()
    {}

    constexpr value_type *operator->() //
        noexcept(std::is_nothrow_invocable_v<F>)
    {
        return std::addressof(_unwrap());
    }

    constexpr const value_type *operator->() const //
        noexcept(std::is_nothrow_invocable_v<F>)
    {
        return std::addressof(_unwrap());
    }

    constexpr value_type &operator*() //
        noexcept(std::is_nothrow_invocable_v<F>)
    {
        return _unwrap();
    }

    constexpr const value_type &operator*() const //
        noexcept(std::is_nothrow_invocable_v<F>)
    {
        return _unwrap();
    }

    constexpr value_type &value() //
        noexcept(std::is_nothrow_invocable_v<F>)
    {
        return _unwrap();
    }

    constexpr const value_type &value() const //
        noexcept(std::is_nothrow_invocable_v<F>)
    {
        return _unwrap();
    }

private:
    using Func = std::conditional_t<
        std::is_reference_v<F>,
        std::reference_wrapper<std::remove_reference_t<F>>,
        std::conditional_t<std::is_function_v<F>, std::add_pointer_t<F>, F>>;

    using Box = std::optional<std::conditional_t<
        std::is_reference_v<T>,
        std::reference_wrapper<std::remove_reference_t<T>>, T>>;

    [[no_unique_address]] mutable Func _fn;
    [[no_unique_address]] mutable Box _box;

    constexpr value_type &_unwrap() const
        noexcept(std::is_nothrow_invocable_v<F>)
    {
        if (!_box) {
            _box.emplace(_fn());
        }

        return *_box;
    }
};

template <std::invocable F>
Lazy(const F &) -> Lazy<std::invoke_result_t<F>, F>;

template <std::invocable F>
Lazy(F &&) -> Lazy<std::invoke_result_t<F>, std::remove_reference_t<F>>;

} // namespace htl

#endif

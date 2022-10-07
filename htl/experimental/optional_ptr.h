#ifndef OPTIONAL_PTR_H_
#define OPTIONAL_PTR_H_

#include <type_traits>
#include <optional>
#include <stdexcept>

template <class T>
    requires std::is_object_v<T>
class OptionalPtr {
public:
    OptionalPtr(T &value) : _ptr(std::addressof(value)) {}

    template <class U>
        requires(std::is_constructible_v<T &, U &>)
    OptionalPtr(OptionalPtr<U> other) : _ptr(other._ptr) {}

    T *operator->() noexcept
    {
        return _ptr;
    }

    const T *operator->() const noexcept
    {
        return _ptr;
    }

    T &operator*() &noexcept
    {
        return *_ptr;
    }

    T &&operator*() &&noexcept
    {
        return *_ptr;
    }

    const T &operator*() const &noexcept
    {
        return *_ptr;
    }

    const T &&operator*() const &&noexcept
    {
        return *_ptr;
    }

    T &value() &
    {
        _check();
        return *_ptr;
    }

    T &&value() &&
    {
        _check();
        return std::move(*_ptr);
    }

    const T &value() const &
    {
        _check();
        return *_ptr;
    }

    const T &&value() const &&
    {
        _check();
        return std::move(*_ptr);
    }

private:
    T *_ptr;

    void _check() const
    {
        if (!_ptr) {
            throw std::bad_optional_access();
        }
    }
};

#endif

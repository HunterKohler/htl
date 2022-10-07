#ifndef HLIB_EXPERIMENTAL_ALIGNED_UNIQUE_PTR_H_
#define HLIB_EXPERIMENTAL_ALIGNED_UNIQUE_PTR_H_

#include <memory>
#include <new>
#include <span>

namespace hlib {

inline constexpr std::size_t dynamic_alignment;

namespace detail {

template <std::size_t Alignment>
class AlignmentStorage {
public:
    constexpr explicit AlignmentStorage(std::size_t) noexcept {}

    constexpr std::size_t alignment() const noexcept { return Alignment; }
};

template <>
class AlignmentStorage<dynamic_alignment> {
public:
    constexpr explicit AlignmentStorage(std::size_t alignment) noexcept
        : _alignment{ alignment }
    {}

    constexpr std::size_t alignment() const noexcept { return _alignment; }

private:
    std::size_t _alignment;
};

} // namespace detail

template <class T, std::size_t Alignment = dynamic_alignment>
    requires std::is_object_v<T>
class AlignedDelete {
public:
    constexpr AlignedDelete() noexcept
        requires(Alignment != dynamic_alignment) = default;

    constexpr explicit AlignedDelete(std::size_t alignment) //
        requires(Alignment == dynamic_alignment)
        : _storage(alignment)
    {}

    template <class U, std::size_t Align>
        requires(!std::is_array_v<T> && std::convertible_to<U *, T *> &&
                 (Alignment == Align || Alignment == dynamic_alignment))
    constexpr AlignedDelete(const AlignedDelete<U, Align> &other) noexcept
        : _storage(other.alignment())
    {}

    template <class U, std::size_t Align>
        requires(std::is_array_v<T> &&std::convertible_to<U (*)[], T (*)[]> &&
                 (Alignment == Align || Alignment == dynamic_alignment))
    constexpr AlignedDelete(const AlignedDelete<U[], Align> &other) noexcept
        requires(std::is_array_v<T>)
        : _storage(other.alignment())
    {}

    constexpr std::size_t alignment() const noexcept
    {
        return _storage.alignment();
    }

    constexpr void operator()(T *p) requires(!std::is_array_v<T>)
    {
        operator delete(p, std::align_val_t(alignment()));
    }

    template <class U>
        requires(std::is_array_v<T> &&std::convertible_to<U (*)[], T (*)[]>)
    constexpr void operator()(U *p)
    {
        operator delete(p, std::align_val_t(alignment()));
    }

private:
    [[no_unique_address]] detail::AlignmentStorage<Alignment> _storage;
};

template <class T>
using AlignedUniquePtr = std::unique_ptr<T, AlignedDelete<T>>;

} // namespace hlib

#endif

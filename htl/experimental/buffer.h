#ifndef HTL_EXPERIMENTAL_BUFFER_H_
#define HTL_EXPERIMENTAL_BUFFER_H_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <iterator>

namespace htl {

template <class T, class Allocator = std::allocator<T>>
    requires std::is_trivially_destructible_v<T>
class Buffer {
private:
    using AllocatorTraits = std::allocator_traits<Allocator>;

    template <bool Const>
    class BufferIterator {
    public:
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = std::conditional_t<
            Const, AllocatorTraits::const_pointer, AllocatorTraits::pointer>;
        using reference =
            std::conditional_t<Const, AllocatorTraits::const_reference,
                               AllocatorTraits::reference>;
        using iterator_category = std::contiguous_iterator_tag;
        using iterator_concept = std::contiguous_iterator_tag;

        BufferIterator() = default;

        explicit BufferIterator(pointer pos) noexcept : _pos(pos) {}

        BufferIterator(BufferIterator<!Const> other) noexcept requires(Const)
            : _pos(other._pos)
        {}

        reference operator*() const noexcept
        {
            return *_pos;
        }

        pointer operator->() const noexcept
        {
            return _pos;
        }

        reference operator[](difference_type n) const noexcept
        {
            return _pos[n];
        }

        BufferIterator &operator+=(difference_type n) noexcept
        {
            _pos += n;
            return *this;
        }

        BufferIterator &operator-=(difference_type n) noexcept
        {
            _pos -= n;
            return *this;
        }

        friend BufferIterator operator+(
            BufferIterator a, difference_type n) noexcept
        {
            return a += n;
        }

        friend BufferIterator operator-(
            BufferIterator a, difference_type n) noexcept
        {
            return a -= n;
        }

        friend BufferIterator operator+(
            difference_type n, BufferIterator a) noexcept
        {
            return a += n;
        }

        friend BufferIterator operator-(
            difference_type n, BufferIterator a) noexcept
        {
            return a -= n;
        }

        friend difference_type operator-(
            BufferIterator a, BufferIterator b) noexcept
        {
            return a._pos - b._pos;
        }

        friend bool operator==(BufferIterator a, BufferIterator b) = default;

        friend std::strong_ordering operator<=>(
            BufferIterator a, BufferIterator b) = default;

    private:
        pointer _pos;
    };

public:
    using allocator_type = Allocator;
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = AllocatorTraits::pointer;
    using const_pointer = AllocatorTraits::const_pointer;
    using reference = AllocatorTraits::reference;
    using const_reference = AllocatorTraits::const_reference;
    using iterator = BufferIterator<false>;
    using const_iterator = BufferIterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    Buffer() noexcept(noexcept(Allocator())) : _alloc(), _data(), _size() {}

    explicit Buffer(size_type new_size, const Allocator &alloc = Allocator())
        : _alloc(alloc), _data(_allocate(_alloc, new_size)), _size(new_size)
    {}

    explicit Buffer(const Allocator &alloc) : _alloc(alloc), _data(), _size() {}

    Buffer(const Buffer &other) //
        requires(std::is_trivially_copy_constructible_v<T>)
        : Buffer(other, AllocatorTraits::select_on_container_copy_construction(
                            other._alloc))
    {}

    Buffer(const Buffer &other, const Allocator &alloc) //
        requires(std::is_trivially_copy_constructible_v<T>)
        : _alloc(alloc), _data(_allocate(other._size)), _size(other._size)
    {
        std::ranges::copy_n(other._data, _size, _data);
    }

    Buffer(Buffer &&other) noexcept : _alloc(std::move(other._alloc))
    {
        _move_assign_data(std::move(other));
    }

    Buffer(Buffer &&other, const Allocator &alloc) //
        requires(std::is_trivially_move_constructible_v<T> ||
                 AllocatorTraits::is_always_equal::value) //
        noexcept(AllocatorTraits::is_always_equal::value)
        : Buffer(std::move(other), alloc,
                 AllocatorTraits::is_always_equal::value{})
    {}

    ~Buffer() noexcept
    {
        if (_size) {
            _deallocate(_data, _size);
        }
    }

    Buffer &operator=(const Buffer &other) requires(
        std::is_trivially_copy_constructible_v<T>
            &&std::is_trivially_copy_assignable_v<T>)
    {
        if (this == std::addressof(other)) {
            return *this;
        } else if constexpr (
            AllocatorTraits::propagate_on_container_move_assignment::value) {
            if (!AllocatorTraits::is_always_equal::value &&
                _alloc != other._alloc) {
                clear();
            }

            _alloc = other._alloc;
        }

        _copy_assign_data(other);
        return *this;
    }

    Buffer &operator=(Buffer &&other) noexcept(
        AllocatorTraits::propagate_on_container_move_assignment::value ||
        AllocatorTraits::is_always_equal::value) //
        requires(std::is_trivially_move_constructible_v<T>
                     &&std::is_trivially_move_assignable_v<T>)
    {
        if (this == std::addressof(other)) {
            return *this;
        } else if constexpr (
            AllocatorTraits::propagate_on_container_move_assignment::value) {
            _alloc = std::move(other._alloc);
            _move_assign_data(std::move(other));
        } else if constexpr (AllocatorTraits::is_always_equal::value) {
            _move_assign_data(std::move(other));
        } else if (_alloc == other._alloc) {
            _move_assign_data(std::move(other));
        } else {
            _copy_assign_data(other);
        }

        return *this;
    }

    Allocator get_allocator() const noexcept
    {
        return _alloc;
    }

    reference operator[](size_type n) noexcept
    {
        return _data[n];
    }

    const_reference operator[](size_type n) const noexcept
    {
        return _data[n];
    }

    pointer data() noexcept
    {
        return _data;
    }

    const_pointer data() const noexcept
    {
        return _data;
    }

    reference at(size_type n)
    {
        return _at(*this, n);
    }

    const_reference at(size_type n) const
    {
        return _at(*this, n);
    }

    reference front() noexcept
    {
        return _data[0];
    }

    const_reference front() const noexcept
    {
        return _data[0];
    }

    reference back() noexcept
    {
        return _data[_size - 1];
    }

    const_reference back() const noexcept
    {
        return _data[_size - 1];
    }

    iterator begin() noexcept
    {
        return iterator(_data);
    }

    iterator end() noexcept
    {
        return iterator(_data + _size);
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(_data);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(_data + _size);
    }

    const_iterator cbegin() const noexcept
    {
        return begin();
    }

    const_iterator cend() const noexcept
    {
        return cend();
    }

    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return rbegin();
    }

    const_reverse_iterator crend() const noexcept
    {
        return rend();
    }

    bool empty() const noexcept
    {
        return !_size;
    }

    size_type size() const noexcept
    {
        return _size;
    }

    size_type max_size() const noexcept
    {
        return std::numeric_limits<difference_type>::max();
    }

    void clear() noexcept
    {
        if (_size) {
            _deallocate(_data, _size);
            _size = 0;
            _data = nullptr;
        }
    }

    void resize(size_type new_size);

    template <class... Args>
    iterator emplace(const_iterator pos, Args &&...args)
    {
        return AllocatorTraits::construct(
            _alloc, std::to_address(pos), std::forward<Args>(args)...);
    }

    friend void swap(Buffer &a, Buffer &b) noexcept
        requires(AllocatorTraits::propagate_on_container_swap::value ||
                 AllocatorTraits::is_always_equal::value)
    {
        if constexpr (AllocatorTraits::propagate_on_container_swap::value) {
            std::ranges::swap(a._alloc, b._alloc);
        }

        std::ranges::swap(a._data, b._data);
        std::ranges::swap(a._size, b._size);
    }

private:
    [[no_unique_address]] Allocator _alloc;
    pointer _data;
    size_type _size;

    Buffer(Buffer &&other, const Allocator &alloc, std::true_type) noexcept
        : _alloc(alloc)
    {
        _move_assign_data(std::move(other));
    }

    Buffer(Buffer &&other, const Allocator &alloc, std::false_type) noexcept
        : _alloc(alloc)
    {
        if (_alloc == other._alloc) {
            _move_assign_data(std::move(other));
        } else {
            _size = other._size;
            _data = _allocate(_size);
            std::ranges::copy_n(other._data, _size, _data);
        }
    }

    static auto _at(auto &self, size_type n)
    {
        if (n >= self._size) {
            throw std::out_of_range("htl::Buffer::at(): Index out of range");
        }

        return self[n];
    }

    pointer _allocate(size_type n)
    {
        return AllocatorTraits::allocate(_alloc, n);
    }

    void _deallocate(pointer p, size_type n) noexcept
    {
        AllocatorTraits::deallocate(_alloc, p, n);
    }

    void _move_assign_data(Buffer &&other) noexcept
    {
        _data = other._data;
        _size = other._size;
        other._data = nullptr;
        other._size = 0;
    }

    void _copy_assign_data(const Buffer &other)
    {
        if (!other._size) {
            clear();
        } else if (_size != other._size) {
            pointer new_data = _allocate(other._size);

            if (_size) {
                _deallocate(_data, _size);
            }

            _data = new_data;
            _size = other._size;
        }

        std::ranges::copy_n(other._data, _data, _size);
    }
};

} // namespace htl

#endif

#ifndef HLIB_EXPERIMENTAL_RING_BUFFER_H_
#define HLIB_EXPERIMENTAL_RING_BUFFER_H_

#include <algorithm>
#include <array>
#include <cassert>
#include <concepts>
#include <iterator>
#include <memory>
#include <ranges>
#include <hlib/compare.h>

namespace hlib {

template <class T, std::size_t N>
class RingBuffer {
private:
    template <bool Const>
    class BasicIterator {
    private:
        using Parent = std::conditional_t<Const, const RingBuffer, RingBuffer>;

    public:
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = std::conditional_t<Const, const T, T> *;
        using reference = std::conditional_t<Const, const T, T> &;
        using iterator_category = std::random_access_iterator_tag;
        using iterator_concept = std::random_access_iterator_tag;

        BasicIterator(Parent &parent, difference_type pos)
            : _parent(std::addressof(parent)), _pos(pos)
        {}

        BasicIterator(BasicIterator<!Const> other) noexcept requires(Const)
            : _parent(other._parent), _pos(other._pos)
        {}

        reference operator[](difference_type n) const
        {
            return (*_parent)[_pos + n];
        }

        reference operator*() const
        {
            return (*_parent)[_pos];
        }

        pointer operator->() const
        {
            return std::addressof(**this);
        }

        BasicIterator &operator++()
        {
            ++_pos;
            return *this;
        }

        BasicIterator &operator--()
        {
            --_pos;
            return *this;
        }

        BasicIterator operator++(int)
        {
            BasicIterator copy(*this);
            ++*this;
            return copy
        }

        BasicIterator operator--(int)
        {
            BasicIterator copy(*this);
            --*this;
            return copy
        }

        BasicIterator &operator+=(difference_type n)
        {
            _pos += diff;
            return *this;
        }

        BasicIterator &operator-=(difference_type n)
        {
            _pos -= n;
            return *this;
        }

        friend BasicIterator operator+(
            const BasicIterator &a, difference_type n)
        {
            return BasicIterator(a) += n;
        }

        friend BasicIterator operator+(
            difference_type n, const BasicIterator &a)
        {
            return a + n;
        }

        friend BasicIterator operator-(
            const BasicIterator &a, difference_type n)
        {
            return BasicIterator(a) -= n;
        }

        friend difference_type operator-(
            const BasicIterator &a, const BasicIterator &b)
        {
            return a._pos - b._pos;
        }

        friend bool operator==(const BasicIterator &, const BasicIterator &) =
            default;

        friend std::strong_ordering operator<=>(
            const BasicIterator &a, const BasicIterator &b)
        {
            return a._pos <=> b._pos;
        }

    private:
        Parent *_parent;
        difference_type _pos;
    };

public:
    static_assert(N, "RingBuffer capacity must be greater than zero");
    static_assert(std::same_as<std::remove_cvref_t<T>, T>,
                  "RingBuffer value type may not be cvref qualified");

    using value_type = T;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const value_type *;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using iterator = BasicIterator<false>;
    using const_iterator = BasicIterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // Special Members

    RingBuffer() noexcept : _size(), _start() {}

    explicit RingBuffer(size_type count) //
        noexcept(std::is_nothrow_default_constructible_v<T>)
        : _size(std::max(count, N)), _start()
    {
        std::ranges::uninitialized_value_construct_n(_data, _size);
    }

    RingBuffer(size_type count, const_reference value) //
        noexcept(std::is_nothrow_copy_constructible_v<T>)
        : _size(std::max(count, N)), _start()
    {
        std::ranges::uninitialized_fill_n(_data, _size, value);
    }

    template <std::input_iterator I, std::sentinel_for<I> S>
        requires(std::convertible_to<std::iter_reference_t<I>, T>)
    RingBuffer(I first, S last)
    {
        if (std::sized_sentinel_for<S, I>) {
            _size = last - first;
            _start = 0;

            if (_size > N) {
                std::ranges::advance(first, _size - N);
                _size = N;
            }

            std::ranges::uninitialized_copy(
                std::move(first), std::move(last), _data);
        } else {
            pointer data_pos = _data;
            pointer data_end = _data + N;

            try {
                for (; data_pos != data_end; ++data_pos) {
                    if (first == last) {
                        _size = data_end - data_pos;
                        _start = 0;
                        return;
                    }

                    std::ranges::construct_at(data_pos, *first);
                    ++first;
                }
            } catch (...) {
                std::ranges::destroy(_data, data_pos);
                throw;
            }

            try {
                while (true) {
                    for (data_pos = _data; data_pos != data_end; ++data_pos) {
                        if (first == last) {
                            _start = data_end - data_pos;
                            _size = N;
                            return;
                        }

                        std::ranges::construct_at(data_pos, *first);
                        ++first;
                    }
                }
            } catch (...) {
                std::ranges::destroy(_data, data_pos);
                std::ranges::destroy(data_pos + 1, data_end);
                throw;
            }
        }
    }

    template <std::ranges::input_range R>
        requires(std::convertible_to<std::ranges::range_reference_t<R>, T>)
    explicit RingBuffer(R &&r)
        : RingBuffer(std::ranges::begin(r), std::ranges::end(r))
    {}

    RingBuffer(std::initializer_list<T> il) //
        noexcept(std::is_nothrow_copy_constructible_v<T>)
        : RingBuffer(std::ranges::subrange(il))
    {}

    RingBuffer(const RingBuffer &other) //
        noexcept(std::is_nothrow_copy_constructible_v<T>)
        : _size(other._size), _start()
    {
        std::ranges::uninitialized_copy_n(other, _data);
    }

    RingBuffer(RingBuffer &&other) //
        noexcept(std::is_nothrow_move_constructible_v<T>)
        : _size(other._size), _start()
    {
        std::ranges::uninitialized_move_n(other, _data);
    }

    ~RingBuffer()
    {
        std::ranges::destroy(*this);
    }

    template <std::ranges::input_range R>
        requires(std::constructible_from<std::ranges::range_reference_t<R>, T>)
    RingBuffer &operator=(R &&r)
    {
        assign(std::forward<R>(r));
        return *this;
    }

    RingBuffer &operator=(std::initializer_list<T> il)
    {
        assign(il);
        return *this;
    }

    RingBuffer &operator=(const RingBuffer &other) //
        noexcept(std::is_nothrow_copy_assignable_v<T>)
    {
        assign(other);
        return *this;
    }

    RingBuffer &operator=(RingBuffer &&other) //
        noexcept(std::is_nothrow_move_assignable_v<T>)
    {
        assign(std::move(other));
        return *this;
    }

    constexpr void assign(size_type count, const_reference value)
    {
        count = std::min(count, N);

        if (_size < count) {
            std::ranges::fill(*this, value);

            while (_size < count) {
                _construct_back(value);
            }
        } else {
            while (_size > count) {
                _pop_back();
            }

            std::ranges::fill(*this, value);
        }
    }

    template <std::input_iterator I, std::sentinel_for<I> S>
        requires(std::convertible_to<std::iter_reference_t<I>, T>)
    void assign(I first, S last);

    template <std::ranges::input_range R>
        requires(std::convertible_to<std::ranges::range_reference_t<R>, T>)
    void assign(R &&r)
    {
        assign(std::ranges::begin(first), std::ranges::end(last));
    }

    void assign(std::initializer_list<T> il)
    {
        assign(il.begin(), il.end());
    }

    void assign(const RingBuffer &other)
    {
        assign(other.begin(), other.end());
    }

    void assign(RingBuffer &&other)
    {
        assign(std::move_iterator(other.begin()), //
               std::move_iterator(other.end()));
    }

    bool empty() const noexcept
    {
        return !_size;
    }

    size_type size() const noexcept
    {
        return _size;
    }

    size_type capacity() const noexcept
    {
        return N;
    }

    size_type max_size() const noexcept
    {
        return N;
    }

    reference operator[](size_type n)
    {
        return *_data_at(n);
    }

    const_reference operator[](size_type n) const
    {
        return *_data_at(n);
    }

    reference at(size_type n)
    {
        _at(*this, n);
    }

    const_reference at(size_type n) const
    {
        return _at(*this, n);
    }

    reference front()
    {
        return _front(*this);
    }

    const_reference front() const
    {
        return _front(*this);
    }

    reference back()
    {
        return _back(*this);
    }

    const_reference back() const
    {
        return _back(*this);
    }

    iterator insert(const_iterator pos, const_reference value);

    iterator insert(const_iterator pos, value_type &&value);

    iterator insert(const_iterator pos, size_type count, const_reference value);

    template <std::input_iterator I, std::sentinel_for<I> S>
        requires(std::convertible_to<std::iter_reference_t<I>, T>)
    iterator insert(const_iterator pos, I first, S last);

    template <std::ranges::input_range R>
        requires(std::convertible_to<std::ranges::range_reference_t<R>, T>)
    iterator insert(const_iterator pos, R &&r);

    iterator insert(const_iterator pos, std::initializer_list<T> il);

    template <class... Args>
        requires(std::constructible_from<T, Args...>)
    iterator emplace(const_iterator pos, Args &&...args);

    iterator erase(const_iterator pos);

    iterator erase(const_iterator first, const_iterator last);

    void push_back(const_reference value)
    {
        _push_back(value);
    }

    void push_back(value_type &&value)
    {
        _push_back(std::move(value));
    }

    template <class... Args>
        requires(std::constructible_from<T, Args...>)
    reference emplace_back(Args &&...args)
    noexcept(std::is_nothrow_constructible_v<T, Args...>)
    {
        pointer target = _data_at(_size);

        if (_size == N) {
            std::ranges::destroy_at(target);
        }

        std::ranges::construct_at(target, std::forward<Args>(args)...);
        _increment_start();

        return *target;
    }

    void pop_back()
    {
        std::ranges::destroy_at(_data_at(--_size));
    }

    void push_front(const_reference value)
    {
        _push_front(value);
    }

    void push_front(value_type &&value)
    {
        _push_front(std::move(value));
    }

    template <class... Args>
        requires(std::constructible_from<T, Args...>)
    reference emplace_front(Args &&...args)
    noexcept(std::is_nothrow_constructible_v<T, Args...>)
    {
        _decrement_start();
        pointer target = _data + _start;

        if (_size == N) {
            std::ranges::destroy_at(target);
        }

        std::ranges::construct_at(target, std::forward<Args>(args)...);
        return *target;
    }

    void pop_front()
    {
        std::ranges::destroy_at(_data + _start);
        _increment_start();
    }

    void resize(size_type count) //
        noexcept(std::is_nothrow_default_constructible_v<T>);

    void resize(size_type count, reference value) //
        noexcept(std::is_nothrow_copy_constructible_v<T>);

    void swap(RingBuffer &other) //
        noexcept(std::is_nothrow_swappable_v<T>);

    iterator begin() noexcept
    {
        return iterator(*this, 0);
    }

    iterator end() noexcept
    {
        return iterator(*this, _size);
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(*this, 0);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(*this, _size);
    }

    const_iterator cbegin() const noexcept
    {
        return begin();
    }

    const_iterator cend() const noexcept
    {
        return end();
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
        const_reverse_iterator(end());
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

private:
    pointer _data_at(difference_type n) noexcept
    {
        return const_cast<pointer>(std::as_const(*this)._data_at(n));
    }

    const_pointer _data_at(difference_type n) const noexcept
    {
        return _data + ((n + _start) % N);
    }

    void _increment_start() noexcept
    {
        _start = (_start + 1) % N;
    }

    void _decrement_start() noexcept
    {
        _start = (_start + N - 1) % N;
    }

    template <class U>
    void _push_front(U &&value)
    {
        if (_size < N) {
            _construct_front(std::forward<U>(value));
        } else {
            _assign_front(std::forward<U>(value));
        }
    }

    template <class U>
    void _assign_front(U &&value)
    {
        _decrement_start();
        _data[_start] = std::forward<U>(value);
    }

    template <class U>
    void _construct_front(U &&value)
    {
        _decrement_start();
        std::ranges::construct_at(_data + _start, std::forward<U>(value));
        ++_size;
    }

    template <class U>
    void _push_back(U &&value)
    {
        if (_size < N) {
            _contruct_back(std::forward<U>(value));
        } else {
            _assign_back(std::forward<U>(value));
        }
    }

    template <class U>
    void _assign_back(U &&value)
    {
        *_data_at(_size) = std::forward<U>(value);
        _increment_start();
    }

    template <class U>
    void _construct_back(U &&value)
    {
        std::ranges::construct_at(_data_at(_size++), std::forward<U>(value));
        _increment_start();
    }

    static auto &_at(auto &self, size_type n)
    {
        if (n >= self._size) {
            throw std::out_of_range();
        }

        return self[n];
    }

    static auto &_front(auto &self)
    {
        return *self._data_at(self._start);
    }

    static auto &_back(auto &self)
    {
        return *self._data_at(self._size - 1);
    }

    T _data[N];
    size_type _size;
    size_type _start;
};

template <class T, std::size_t N>
inline void swap(RingBuffer<T, N> &a, RingBuffer<T, N> &b) //
    noexcept(noexcept(a.swap(b)))
{
    a.swap(b);
}

template <class T, std::size_t N>
    requires std::equality_comparable<T>
inline bool operator==(const RingBuffer<T, N> &a, const RingBuffer<T, N> &b)
{
    return std::ranges::equal(a, b);
}

template <class T, std::size_t N>
inline SynthThreeWayResult<T>
operator<=>(const RingBuffer<T, N> &a, const RingBuffer<T, N> &b)
{
    return std::ranges::lexicographical_compare_three_way(
        a, b, synth_three_way);
}

} // namespace hlib

#endif

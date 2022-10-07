#ifndef HTL_INTRUSIVE_LIST_H_
#define HTL_INTRUSIVE_LIST_H_

#include <iterator>
#include <utility>
#include <algorithm>
#include <ranges>
#include <concepts>
#include <compare>
#include <htl/utility.h>
#include <htl/detail/simple_hash.h>
#include <htl/concepts.h>
#include <htl/compare.h>

namespace htl {

template <class>
class IntrusiveList;

template <class T>
class IntrusiveListNode {
public:
    IntrusiveListNode() noexcept : _prev{ this }, _next{ this } {}

    IntrusiveListNode(IntrusiveListNode &prev, IntrusiveListNode &next)
        : _prev{ std::addressof(prev) }, _next{ std::addressof(next) }
    {
        _link();
    }

    IntrusiveListNode(const IntrusiveListNode &) = delete;

    IntrusiveListNode(IntrusiveListNode &&other) noexcept
    {
        if (other._empty()) {
            _set();
        } else {
            _set(*other._prev, *other._next);
            _link();
        }
    }

    IntrusiveListNode &operator=(const IntrusiveListNode &) = delete;

    IntrusiveListNode &operator=(IntrusiveListNode &&other)
    {
        if (std::addressof(other) != this) {
            if (other._empty()) {
                _reset();
            } else {
                _reset(*other._prev, *other._next);
            }
        }
    }

    friend void swap(IntrusiveListNode &a, IntrusiveListNode &b) noexcept
    {
        if (std::addressof(a) == std::addressof(b)) {
            return;
        } else if (a._empty()) {
            if (!b._empty()) {
                a._set(*b._prev, b._next);
                a._link();
                b._set();
            }
        } else if (b._empty()) {
            b._set(*a._prev, a._next);
            b._link();
            a._set();
        } else {
            std::ranges::swap(a._next, b._next);
            std::ranges::swap(a._prev, b._prev);
            a._link();
            b._link();
        }
    }

private:
    friend IntrusiveList<T>;

    IntrusiveListNode *_prev;
    IntrusiveListNode *_next;

    void _reset()
    {
        _unlink();
        _set();
    }

    void _reset(IntrusiveListNode &prev, IntrusiveListNode &next)
    {
        _unlink();
        _set(prev, next);
        _link();
    }

    void _unlink()
    {
        _prev->_next = _next;
        _next->_prev = _prev;
    }

    void _link()
    {
        _prev->_next = this;
        _next->_prev = this;
    }

    void _set()
    {
        _set(*this, *this);
    }

    void _set(IntrusiveListNode &prev, IntrusiveListNode &next)
    {
        _prev = std::addressof(prev);
        _next = std::addressof(next);
    }

    bool _empty() const
    {
        return _next == this;
    }
};

template <class T>
// requires std::derived_from<T, IntrusiveListNode<T>>
class IntrusiveList {
private:
    using Node = IntrusiveListNode<T>;

    template <bool Const>
    class BasicIterator {
    private:
        using Node = std::conditional_t<
            Const, const IntrusiveList::Node, IntrusiveList::Node>;

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = std::conditional_t<Const, const T, T> *;
        using reference = std::conditional_t<Const, const T, T> &;
        using iterator_category = std::bidirectional_iterator_tag;
        using iterator_concept = std::bidirectional_iterator_tag;

        BasicIterator() = default;

        BasicIterator(reference value)
            : BasicIterator(static_cast<Node &>(value))
        {}

        BasicIterator(BasicIterator<!Const> other) requires(Const)
            : _node{ other._node }
        {}

        BasicIterator &operator++()
        {
            _node = _node->_next;
            return *this;
        }

        BasicIterator &operator--()
        {
            _node = _node->_prev;
            return *this;
        }

        BasicIterator operator++(int)
        {
            BasicIterator copy{ *this };
            ++*this;
            return copy;
        }

        BasicIterator operator--(int)
        {
            BasicIterator copy{ this };
            --*this;
            return copy;
        }

        reference operator*() const
        {
            return static_cast<reference>(*_node);
        }

        pointer operator->() const
        {
            return static_cast<pointer>(_node);
        }

        friend bool operator==(BasicIterator a, BasicIterator b) = default;

    private:
        friend IntrusiveList;

        BasicIterator(Node &node) : _node{ std::addressof(node) } {}

        Node *_node;

        IntrusiveList::Node *_get() const noexcept
        {
            return const_cast<IntrusiveList::Node *>(_node);
        }
    };

public:
    using value_type = T;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;
    using iterator = BasicIterator<false>;
    using const_iterator = BasicIterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    IntrusiveList() = default;

    template <std::input_iterator I, std::sentinel_for<I> S>
        requires(std::convertible_to<std::iter_reference_t<I>, reference>)
    IntrusiveList(I first, S last)
    {
        insert(begin(), std::move(first), std::move(last));
    }

    template <std::ranges::input_range R>
        requires(
            std::convertible_to<std::ranges::range_reference_t<R>, reference>)
    explicit IntrusiveList(R &&r)
        : IntrusiveList(std::ranges::begin(r), std::ranges::end(r))
    {}

    IntrusiveList(std::initializer_list<std::reference_wrapper<T>> il)
        : IntrusiveList(std::ranges::subrange(il))
    {}

    IntrusiveList(const IntrusiveList &) = delete;

    IntrusiveList(IntrusiveList &&other) = default;

    IntrusiveList &operator=(const IntrusiveList &) = delete;

    IntrusiveList &operator=(IntrusiveList &&other)
    {
        assign(std::move(other));
        return *this;
    }

    IntrusiveList &operator=(
        std::initializer_list<std::reference_wrapper<T>> il)
    {
        assign(il);
        return *this;
    }

    template <std::input_iterator I, std::sentinel_for<I> S>
        requires(std::convertible_to<std::iter_reference_t<I>, reference>)
    void assign(I first, S last)
    {
        insert(begin(), std::move(first), std::move(last));
    }

    template <std::ranges::input_range R>
        requires std::convertible_to<
            std::ranges::range_reference_t<R>, reference>
    void assign(R &&r)
    {
        assign(std::ranges::begin(r), std::ranges::end(r));
    }

    void assign(std::initializer_list<std::reference_wrapper<T>> il)
    {
        assign(std::ranges::subrange(il));
    }

    void assign(const IntrusiveList &) = delete;

    void assign(IntrusiveList &&other)
    {
        if (other.empty()) {
            _root._reset();
        } else {
            _root._reset(*other._root._prev, *other._root._next);
        }
    }

    iterator insert(const_iterator pos, reference value)
    {
        value.Node::_reset(*pos._get()->_prev, *pos._get());
        return iterator(value);
    }

    template <std::input_iterator I, std::sentinel_for<I> S>
        requires(std::convertible_to<std::iter_reference_t<I>, reference>)
    iterator insert(const_iterator pos, I first, S last)
    {
        Node *next = pos._get(), *start = next->_prev, *prev = start;

        for (; first != last; ++first) {
            reference value = *first;
            Node *node = std::addressof(value);

            node->_prev = prev;
            prev->_next = node;
            prev = node;
        }

        prev->_next = next;
        next->_prev = prev;

        return iterator(*start->_next);
    }

    template <std::ranges::input_range R>
        requires(
            std::convertible_to<std::ranges::range_reference_t<R>, reference>)
    iterator insert(const_iterator pos, R &&r)
    {
        return insert(pos, std::ranges::begin(r), std::ranges::end(r));
    }

    iterator insert(const_iterator pos,
                    std::initializer_list<std::reference_wrapper<T>> il)
    {
        return insert(pos, std::ranges::subrange(il));
    }

    void push_front(reference value)
    {
        insert(begin(), value);
    }

    void push_back(reference value)
    {
        insert(end(), value);
    }

    void pop_front()
    {
        _root._next->_reset();
    }

    void pop_back()
    {
        _root._prev->_reset();
    }

    iterator erase(const_iterator pos)
    {
        Node *node = pos._get(), next = node->_next;
        node->_reset();
        return iterator(next);
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        Node *node = first._get(), next = last._get(), prev = node->_prev;

        if (node != next) {
            for (; node != next; node = node->_next) {
                node->_set();
            }

            prev->_next = next;
            next->_prev = prev;
        }

        return iterator(*node);
    }

    void clear()
    {
        _root.reset();
    }

    bool empty() const noexcept
    {
        return _root._empty();
    }

    reference front()
    {
        return static_cast<reference>(*_root._next);
    }

    reference back()
    {
        return static_cast<reference>(*_root._prev);
    }

    const_reference front() const
    {
        return *_root._next;
    }

    const_reference back() const
    {
        return *_root._prev;
    }

    iterator begin() noexcept
    {
        return iterator(*_root._next);
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(*_root._next);
    }

    const_iterator cbegin() const noexcept
    {
        return const_iterator(*_root._next);
    }

    iterator end() noexcept
    {
        return iterator(_root);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(_root);
    }

    const_iterator cend() const noexcept
    {
        return const_iterator(_root);
    }

    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    friend void swap(IntrusiveList &a, IntrusiveList &b)
    {
        std::ranges::swap(a._root, b._root);
    }

private:
    Node _root;
};

template <class T>
inline SynthThreeWayResult<T>
operator<=>(const IntrusiveList<T> &a, const IntrusiveList<T> &b)
{
    return std::lexicographical_compare_three_way(
        a.begin(), a.end(), b.begin(), b.end(), synth_three_way);
}

template <std::equality_comparable T>
inline bool operator==(const IntrusiveList<T> &a, const IntrusiveList<T> &b)
{
    return std::ranges::equal(a, b);
}

} // namespace htl

#endif

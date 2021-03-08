#pragma once

#include <type_traits>
#include <iterator>
#include <cassert>

namespace entler {

    class IntrusiveListNode {
    public:
        IntrusiveListNode();
        IntrusiveListNode(IntrusiveListNode&& other);
        IntrusiveListNode(const IntrusiveListNode&) = delete;

        ~IntrusiveListNode();

        IntrusiveListNode& operator=(IntrusiveListNode&& rhs);
        IntrusiveListNode& operator=(const IntrusiveListNode&) = delete;

        bool is_linked() const;
        void unlink();

        // prepend this to next
        void link(IntrusiveListNode* next);

        // next = prev = this
        void self_link();

        IntrusiveListNode* next();
        const IntrusiveListNode* next() const;
        IntrusiveListNode* prev();
        const IntrusiveListNode* prev() const;

    private:
        IntrusiveListNode* next_;
        IntrusiveListNode* prev_;
    };

    template <typename T, IntrusiveListNode T::*member_node>
    class IntrusiveListConstIterator {
    public:
        using const_iterator = IntrusiveListConstIterator;

        using value_type = T;
        using pointer = const T*;
        using reference = const T&;
        using iterator_category = std::bidirectional_iterator_tag;

        IntrusiveListConstIterator(const IntrusiveListNode* node = nullptr);

        reference operator*() const;
        pointer operator->() const;

        const_iterator& operator=(const const_iterator&) = default;
        const_iterator& operator++(); // prefix increment
        const_iterator operator++(int); // postfix increment
        const_iterator& operator--(); // prefix decrement
        const_iterator operator--(int); // postfix decrement

        bool operator==(const const_iterator& rhs) const;
        bool operator!=(const const_iterator& rhs) const;

        const IntrusiveListNode* node() const;

    protected:
        IntrusiveListConstIterator(IntrusiveListNode* node);

        IntrusiveListNode* node_;
    };

    // TODO: make iterators member classes of IntrusiveList 
    template <typename T, IntrusiveListNode T::*member_node>
    class IntrusiveListIterator : public IntrusiveListConstIterator<T, member_node> {
    public:
        using iterator = IntrusiveListIterator;

        using value_type = typename std::remove_cv<T>::type;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::bidirectional_iterator_tag;

        IntrusiveListIterator(IntrusiveListNode* node = nullptr);

        reference operator*() const;
        pointer operator->() const;

        iterator& operator=(const iterator&) = default;
        iterator& operator++(); // prefix increment
        iterator operator++(int); // postfix increment
        iterator& operator--(); // prefix decrement
        iterator operator--(int); // postfix decrement

        IntrusiveListNode* node();
    };

    template <typename T, IntrusiveListNode T::*node>
    class IntrusiveList{
    public:
        using const_iterator = IntrusiveListConstIterator<T, node>;
        using iterator = IntrusiveListIterator<T, node>;

        IntrusiveList();
        IntrusiveList(IntrusiveList&& other);
        IntrusiveList(const IntrusiveList&) = delete;

        ~IntrusiveList();

        IntrusiveList& operator=(IntrusiveList&& rhs);
        IntrusiveList& operator=(const IntrusiveList& rhs) = delete;

        iterator begin();
        iterator end();
        const_iterator begin() const;
        const_iterator end() const;

        bool empty() const;

        T& front();
        const T& front() const;
        T& back();
        const T& back() const;

        iterator insert(const_iterator pos, T& value);
        iterator push_back(T& value);
        iterator push_front(T& value);
        iterator erase(const_iterator pos);
        void pop_front();
        void pop_back();
        void clear();

    private:
        IntrusiveListNode root_;
    };

#include "intrusive_list_inline.h"

}

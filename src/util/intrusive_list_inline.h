namespace detail {

    // TODO: inline / templated variable
    template <typename T, IntrusiveListNode T::*node_member>
    inline size_t intrusive_list_node_offset()
    {
        const T* value = nullptr;
        const IntrusiveListNode* node = &(value->*node_member);

        auto value_ptr = reinterpret_cast<const uint8_t*>(value);
        auto node_ptr = reinterpret_cast<const uint8_t*>(node);
        assert(value_ptr <= node_ptr && "list node offset should be positive");
        return node_ptr - value_ptr;
    }

    template <typename T, IntrusiveListNode T::*node_member>
    inline IntrusiveListNode *to_intrusive_list_node(T* value)
    {
        auto value_ptr = reinterpret_cast<uint8_t*>(value);
        auto node_ptr = value_ptr + intrusive_list_node_offset<T, node_member>();
        return reinterpret_cast<IntrusiveListNode*>(node_ptr);
    }

    template <typename T, IntrusiveListNode T::*node_member>
    inline const IntrusiveListNode *to_intrusive_list_node(const T* value)
    {
        auto value_ptr = reinterpret_cast<const uint8_t*>(value);
        auto node_ptr = value_ptr + intrusive_list_node_offset<T, node_member>();
        return reinterpret_cast<const IntrusiveListNode*>(node_ptr);
    }

    template <typename T, IntrusiveListNode T::*node_member>
    inline T* from_intrusive_list_node(IntrusiveListNode* node)
    {
        auto node_ptr = reinterpret_cast<uint8_t*>(node);
        auto value_ptr = node_ptr - intrusive_list_node_offset<T, node_member>();
        return reinterpret_cast<T*>(value_ptr);
    }

    template <typename T, IntrusiveListNode T::*node_member>
    inline const T* from_intrusive_list_node(const IntrusiveListNode* node)
    {
        auto node_ptr = reinterpret_cast<const uint8_t*>(node);
        auto value_ptr = node_ptr - intrusive_list_node_offset<T, node_member>();
        return reinterpret_cast<const T*>(value_ptr);
    }

}

inline IntrusiveListNode::IntrusiveListNode()
        : next_{nullptr}
        , prev_{nullptr}
{
}

inline IntrusiveListNode::IntrusiveListNode(IntrusiveListNode&& other)
        : next_{other.next_}
        , prev_{other.prev_}
{
    other.prev_ = nullptr;
    other.next_ = nullptr;
}

inline IntrusiveListNode::~IntrusiveListNode()
{
    unlink();
}

inline IntrusiveListNode& IntrusiveListNode::operator=(IntrusiveListNode&& rhs)
{
    if(this != &rhs) {
        unlink();

        next_ = rhs.next_;
        prev_ = rhs.prev_;

        rhs.next_ = nullptr;
        rhs.prev_ = nullptr;
    }

    return *this;
}

inline bool IntrusiveListNode::is_linked() const
{
    return next_ != nullptr;
}

inline void IntrusiveListNode::unlink()
{
    if(!is_linked()) {
        return;
    }

    auto next = next_;
    auto prev = prev_;

    next_->prev_ = prev;
    prev_->next_ = next;

    next_ = nullptr;
    prev_ = nullptr;
}

inline void IntrusiveListNode::link(IntrusiveListNode* next)
{
    unlink();

    auto prev = next->prev_;

    next_ = next;
    prev_ = prev;

    next->prev_ = this;
    prev->next_ = this;
}

inline void IntrusiveListNode::self_link()
{
    unlink();

    next_ = this;
    prev_ = this;
}

inline IntrusiveListNode* IntrusiveListNode::next()
{
    return next_;
}

inline const IntrusiveListNode* IntrusiveListNode::next() const
{
    return next_;
}

inline IntrusiveListNode* IntrusiveListNode::prev()
{
    return prev_;
}

inline const IntrusiveListNode* IntrusiveListNode::prev() const
{
    return prev_;
}

template <typename T, IntrusiveListNode T::*member_node>
IntrusiveListConstIterator<T, member_node>::IntrusiveListConstIterator(const IntrusiveListNode* node)
        : node_{const_cast<IntrusiveListNode*>(node)}
{}

template <typename T, IntrusiveListNode T::*member_node>
IntrusiveListConstIterator<T, member_node>::IntrusiveListConstIterator(IntrusiveListNode* node)
        : node_{node}
{}

template <typename T, IntrusiveListNode T::*member_node>
auto IntrusiveListConstIterator<T, member_node>::operator*() const -> reference
{
    return *detail::from_intrusive_list_node<T, member_node>(node_);
}

template <typename T, IntrusiveListNode T::*member_node>
auto IntrusiveListConstIterator<T, member_node>::operator->() const -> pointer
{
    return detail::from_intrusive_list_node<T, member_node>(node_);
}

template <typename T, IntrusiveListNode T::*member_node>
auto IntrusiveListConstIterator<T, member_node>::operator++() -> const_iterator&
{
    node_ = node_->next();
    return *this;
}

template <typename T, IntrusiveListNode T::*member_node>
auto IntrusiveListConstIterator<T, member_node>::operator++(int) -> const_iterator
{
    auto result = *this;
    node_ = node_->next();
    return result;
}

template <typename T, IntrusiveListNode T::*member_node>
auto IntrusiveListConstIterator<T, member_node>::operator--() -> const_iterator&
{
    node_ = node_->prev();
    return *this;
}

template <typename T, IntrusiveListNode T::*member_node>
auto IntrusiveListConstIterator<T, member_node>::operator--(int) -> const_iterator
{
    auto result = *this;
    node_ = node_->prev();
    return result;
}

template <typename T, IntrusiveListNode T::*member_node>
bool IntrusiveListConstIterator<T, member_node>::operator==(const const_iterator& rhs) const
{
    return node_ == rhs.node_;
}

template <typename T, IntrusiveListNode T::*member_node>
bool IntrusiveListConstIterator<T, member_node>::operator!=(const const_iterator& rhs) const
{
    return node_ != rhs.node_;
}

template <typename T, IntrusiveListNode T::*member_node>
const IntrusiveListNode* IntrusiveListConstIterator<T, member_node>::node() const
{
    return node_;
}

template <typename T, IntrusiveListNode T::*member_node>
IntrusiveListIterator<T, member_node>::IntrusiveListIterator(IntrusiveListNode* node)
        : IntrusiveListConstIterator<T, member_node>(node)
{}

template <typename T, IntrusiveListNode T::*member_node>
auto IntrusiveListIterator<T, member_node>::operator*() const -> reference
{
    return *detail::from_intrusive_list_node<T, member_node>(this->node_);
}

template <typename T, IntrusiveListNode T::*member_node>
auto IntrusiveListIterator<T, member_node>::operator->() const -> pointer
{
    return detail::from_intrusive_list_node<T, member_node>(this->node_);
}

template <typename T, IntrusiveListNode T::*member_node>
auto IntrusiveListIterator<T, member_node>::operator++() -> iterator&
{
    this->node_ = this->node_->next();
    return *this;
}

template <typename T, IntrusiveListNode T::*member_node>
auto IntrusiveListIterator<T, member_node>::operator++(int) -> iterator
{
    auto result = *this;
    this->node_ = this->node_->next();
    return result;
}

template <typename T, IntrusiveListNode T::*member_node>
auto IntrusiveListIterator<T, member_node>::operator--() -> iterator&
{
    this->node_ = this->node_->prev();
    return *this;
}

template <typename T, IntrusiveListNode T::*member_node>
auto IntrusiveListIterator<T, member_node>::operator--(int) -> iterator
{
    auto result = *this;
    this->node_ = this->node_->prev();
    return result;
}

template <typename T, IntrusiveListNode T::*member_node>
IntrusiveListNode* IntrusiveListIterator<T, member_node>::node()
{
    return this->node_;
}

template <typename T, IntrusiveListNode T::*member_node>
IntrusiveList<T, member_node>::IntrusiveList()
{
    root_.self_link();
}

template <typename T, IntrusiveListNode T::*member_node>
IntrusiveList<T, member_node>::IntrusiveList(IntrusiveList&& other)
{
    // insert ourselves before the rhs root
    root_.link(&other.root_);

    // unlink rhs root and make it part of an empty list
    other.root_.self_link();
}

template <typename T, IntrusiveListNode T::*member_node>
IntrusiveList<T, member_node>::~IntrusiveList()
{
    clear();
}

template <typename T, IntrusiveListNode T::*member_node>
IntrusiveList<T, member_node>& IntrusiveList<T, member_node>::operator=(IntrusiveList&& rhs)
{
    if(this != &rhs) {
        clear();

        // insert ourselves before the rhs root
        root_.link(&rhs.root_);

        // unlink rhs root and make it part of an empty list
        rhs.root_.self_link();
    }

    return *this;
}

template <typename T, IntrusiveListNode T::*member_node>
auto IntrusiveList<T, member_node>::begin() -> iterator
{
    return iterator{root_.next()};
}

template <typename T, IntrusiveListNode T::*member_node>
auto IntrusiveList<T, member_node>::end() -> iterator
{
    return iterator{&root_};
}

template <typename T, IntrusiveListNode T::*member_node>
auto IntrusiveList<T, member_node>::begin() const -> const_iterator
{
    return const_iterator{root_.next()};
}

template <typename T, IntrusiveListNode T::*member_node>
auto IntrusiveList<T, member_node>::end() const -> const_iterator
{
    return const_iterator{&root_};
}

template <typename T, IntrusiveListNode T::*member_node>
bool IntrusiveList<T, member_node>::empty() const
{
    return begin() == end();
}

template <typename T, IntrusiveListNode T::*member_node>
T& IntrusiveList<T, member_node>::front()
{
    assert(!empty());
    return *begin();
}

template <typename T, IntrusiveListNode T::*member_node>
const T& IntrusiveList<T, member_node>::front() const
{
    assert(!empty());
    return *begin();
}

template <typename T, IntrusiveListNode T::*member_node>
T& IntrusiveList<T, member_node>::back()
{
    assert(!empty());
    return *(--end());
}

template <typename T, IntrusiveListNode T::*member_node>
const T& IntrusiveList<T, member_node>::back() const
{
    assert(!empty());
    return *(--end());
}

template <typename T, IntrusiveListNode T::*member_node>
auto IntrusiveList<T, member_node>::insert(const_iterator pos, T& value) -> iterator
{
    auto node = detail::to_intrusive_list_node<T, member_node>(&value);
    node->link(const_cast<IntrusiveListNode*>(pos.node()));
    return iterator{node};
}

template <typename T, IntrusiveListNode T::*member_node>
auto IntrusiveList<T, member_node>::push_back(T& value) -> iterator
{
    return insert(end(), value);
}

template <typename T, IntrusiveListNode T::*member_node>
auto IntrusiveList<T, member_node>::push_front(T& value) -> iterator
{
    return insert(begin(), value);
}

template <typename T, IntrusiveListNode T::*member_node>
auto IntrusiveList<T, member_node>::erase(const_iterator pos) -> iterator
{
    auto node = const_cast<IntrusiveListNode*>(pos.node());
    if(node != &root_) {
        iterator next{node->next()};
        node->unlink();
        return next;
    }

    return end();
}

template <typename T, IntrusiveListNode T::*member_node>
void IntrusiveList<T, member_node>::pop_front()
{
    assert(!empty());
    erase(begin());
}

template <typename T, IntrusiveListNode T::*member_node>
void IntrusiveList<T, member_node>::pop_back()
{
    assert(!empty());
    erase(--end());
}

template <typename T, IntrusiveListNode T::*member_node>
void IntrusiveList<T, member_node>::clear()
{
    for(auto it = begin(); it != end(); ) {
        it = erase(it);
    }
}

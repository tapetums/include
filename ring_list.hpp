#pragma once

//---------------------------------------------------------------------------//
//
// ring_list.hpp
//  双方向循環リスト
//   Copyright (C) 2014-2017 tapetums
//
//---------------------------------------------------------------------------//

#include <functional>
#include <initializer_list>

//---------------------------------------------------------------------------//

namespace tapetums {

//---------------------------------------------------------------------------//
// Class
//---------------------------------------------------------------------------//

template<typename T> class ring_list
{
protected: // inner structure
    struct list_node
    {
        T value;
        bool is_head;
        list_node* prev;
        list_node* next;

        list_node() : is_head(true), prev(this), next(this) { }
        list_node(const T& value) : is_head(false), value(value) { }
        list_node(T&& value) : is_head(false), value(std::move(value)) { }
    };

public: // inner iterators
    class iterator_base
    {
        friend class ring_list;

    protected:
        list_node* m_node;

    public:
        iterator_base()
        {
            m_node = nullptr;
        }

        iterator_base(list_node* node)
        {
            m_node = node;
        }

        iterator_base(const iterator_base& lhs)
        {
            m_node = lhs.m_node;
        }

        iterator_base(iterator_base&& rhs) noexcept
        {
            m_node = rhs.m_node;
            rhs.m_node = nullptr;
        }

    public:
        bool operator==(const iterator_base& it) const
        {
            return (m_node == it.m_node);
        }

        bool operator!=(const iterator_base& it) const
        {
            return (m_node != it.m_node);
        }
    };

    class const_iterator : public iterator_base
    {
    private:
        using super = iterator_base;

    public:
        const_iterator()                              : super()     { }
        const_iterator(list_node* node)               : super(node) { }
        const_iterator(const const_iterator& lhs)     : super(lhs)  { }
        const_iterator(const_iterator&& rhs) noexcept : super(rhs)  { }

    public:
        const_iterator& operator=(const const_iterator& lhs)
        {
            m_node = lhs.m_node;
            return *this;
        }

        const_iterator& operator=(const_iterator&& rhs) noexcept
        {
            m_node = rhs.m_node;
            rhs.m_node = nullptr;

            return *this;
        }

        const T& operator*() const
        {
            return m_node->value;
        }

        const T* operator->() const
        {
            return &m_node->value;
        }

        const_iterator& operator++()
        {
            m_node = m_node->next;
            return *this;
        }

        const_iterator& operator--()
        {
            m_node = m_node->prev;
            return *this;
        }
    };

    class iterator : public const_iterator
    {
    private:
        using super = const_iterator;

    public:
        iterator()                        : super()     { }
        iterator(list_node* node)         : super(node) { }
        iterator(const iterator& lhs)     : super(lhs)  { }
        iterator(iterator&& rhs) noexcept : super(rhs)  { }

    public:
        iterator& operator=(const iterator& lhs)
        {
            m_node = lhs.m_node;
            return *this;
        }

        iterator& operator=(iterator&& rhs) noexcept
        {
            m_node = rhs.m_node;
            rhs.m_node = nullptr;

            return *this;
        }

        T& operator*()
        {
            return m_node->value;
        }

        T* operator->()
        {
            return &m_node->value;
        }

        iterator& operator++()
        {
            m_node = m_node->next;
            return *this;
        }

        iterator& operator--()
        {
            m_node = m_node->prev;
            return *this;
        }
    };

    class const_reverse_iterator : public iterator_base
    {
    private:
        using super = iterator_base;

    public:
        const_reverse_iterator()                                      : super()     { }
        const_reverse_iterator(list_node* node)                       : super(node) { }
        const_reverse_iterator(const const_reverse_iterator& lhs)     : super(lhs)  { }
        const_reverse_iterator(const_reverse_iterator&& rhs) noexcept : super(rhs)  { }

    public:
        const_reverse_iterator& operator=(const const_reverse_iterator& lhs)
        {
            m_node = lhs.m_node;
            return *this;
        }

        const_reverse_iterator& operator=(const_reverse_iterator&& rhs) noexcept
        {
            m_node = rhs.m_node;
            rhs.m_node = nullptr;

            return *this;
        }

        const T& operator*() const
        {
            return m_node->value;
        }

        const T* operator->() const
        {
            return &m_node->value;
        }

        const_reverse_iterator& operator++()
        {
            m_node = m_node->prev;
            return *this;
        }

        const_reverse_iterator& operator--()
        {
            m_node = m_node->next;
            return *this;
        }
    };

    class reverse_iterator : public const_reverse_iterator
    {
    private:
        using super = const_reverse_iterator;

    public:
        reverse_iterator()                                : super()     { }
        reverse_iterator(list_node* node)                 : super(node) { }
        reverse_iterator(const reverse_iterator& lhs)     : super(lhs)  { }
        reverse_iterator(reverse_iterator&& rhs) noexcept : super(rhs)  { }

    public:
        reverse_iterator& operator=(const reverse_iterator& lhs)
        {
            m_node = lhs.m_node;
            return *this;
        }

        reverse_iterator& operator=(reverse_iterator&& rhs) noexcept
        {
            m_node = rhs.m_node;
            rhs.m_node = nullptr;

            return *this;
        }

        T& operator*()
        {
            return m_node->value;
        }

        T* operator->()
        {
            return &m_node->value;
        }

        reverse_iterator& operator++()
        {
            m_node = m_node->prev;
            return *this;
        }

        reverse_iterator& operator--()
        {
            m_node = m_node->next;
            return *this;
        }
    };

    class const_ring_iterator : public const_iterator
    {
    private:
        using super = const_iterator;

    public:
        const_ring_iterator()                                   : super()     { }
        const_ring_iterator(list_node* node)                    : super(node) { }
        const_ring_iterator(const const_ring_iterator& lhs)     : super(lhs)  { }
        const_ring_iterator(const_ring_iterator&& rhs) noexcept : super(rhs)  { }

    public:
        const_ring_iterator& operator=(const const_ring_iterator& lhs)
        {
            m_node = lhs.m_node;
            return *this;
        }

        const_ring_iterator& operator=(const_ring_iterator&& rhs) noexcept
        {
            m_node = rhs.m_node;
            rhs.m_node = nullptr;

            return *this;
        }

        const_ring_iterator& operator++()
        {
            m_node = m_node->next;
            if ( m_node->ishead )
            {
                m_node = m_node->next;
            }
            return *this;
        }

        const_ring_iterator& operator--()
        {
            m_node = m_node->prev;
            if ( m_node->ishead )
            {
                m_node = m_node->prev;
            }
            return *this;
        }
    };

    class ring_iterator : public const_ring_iterator
    {
    private:
        using super = const_ring_iterator;

    public:
        ring_iterator()                             : super()     { }
        ring_iterator(list_node* node)              : super(node) { }
        ring_iterator(const ring_iterator& lhs)     : super(lhs)  { }
        ring_iterator(ring_iterator&& rhs) noexcept : super(rhs)  { }

    public:
        ring_iterator& operator=(const ring_iterator& lhs)
        {
            m_node = lhs.m_node;
            return *this;
        }

        ring_iterator& operator=(ring_iterator&& rhs) noexcept
        {
            m_node = rhs.m_node;
            rhs.m_node = nullptr;

            return *this;
        }

        T& operator*()
        {
            return m_node->value;
        }

        T* operator->()
        {
            return &m_node->value;
        }

        ring_iterator& operator++()
        {
            m_node = m_node->next;
            if ( m_node->is_head )
            {
                m_node = m_node->next;
            }
            return *this;
        }

        ring_iterator& operator--()
        {
            m_node = m_node->prev;
            if ( m_node->is_head )
            {
                m_node = m_node->prev;
            }
            return *this;
        }
    };

    class const_reverse_ring_iterator : public const_reverse_iterator
    {
    private:
        using super = iterator_base;

    public:
        const_reverse_ring_iterator()                                           : super()     { }
        const_reverse_ring_iterator(list_node* node)                            : super(node) { }
        const_reverse_ring_iterator(const const_reverse_ring_iterator& lhs)     : super(lhs)  { }
        const_reverse_ring_iterator(const_reverse_ring_iterator&& rhs) noexcept : super(rhs)  { }

    public:
        const_reverse_ring_iterator& operator=(const const_reverse_ring_iterator& lhs)
        {
            m_node = lhs.m_node;
            return *this;
        }

        const_reverse_ring_iterator& operator=(const_reverse_ring_iterator&& rhs) noexcept
        {
            m_node = rhs.m_node;
            rhs.m_node = nullptr;

            return *this;
        }

        const T& operator*() const
        {
            return m_node->value;
        }

        const T* operator->() const
        {
            return &m_node->value;
        }

        const_reverse_ring_iterator& operator++()
        {
            m_node = m_node->prev;
            if ( m_node->ishead )
            {
                m_node = m_node->prev;
            }
            return *this;
        }

        const_reverse_ring_iterator& operator--()
        {
            m_node = m_node->next;
            if ( m_node->ishead )
            {
                m_node = m_node->next;
            }
            return *this;
        }
    };

    class reverse_ring_iterator : public const_reverse_ring_iterator
    {
    private:
        using super = const_reverse_ring_iterator;

    public:
        reverse_ring_iterator()                                     : super()     { }
        reverse_ring_iterator(list_node* node)                      : super(node) { }
        reverse_ring_iterator(const reverse_ring_iterator& lhs)     : super(lhs)  { }
        reverse_ring_iterator(reverse_ring_iterator&& rhs) noexcept : super(rhs)  { }

    public:
        reverse_ring_iterator& operator=(const reverse_ring_iterator& lhs)
        {
            m_node = lhs.m_node;
            return *this;
        }

        reverse_ring_iterator& operator=(reverse_ring_iterator&& rhs) noexcept
        {
            m_node = rhs.m_node;
            rhs.m_node = nullptr;

            return *this;
        }

        T& operator*()
        {
            return m_node->value;
        }

        T* operator->()
        {
            return &m_node->value;
        }

        reverse_ring_iterator& operator++()
        {
            m_node = m_node->prev;
            if ( m_node->ishead )
            {
                m_node = m_node->prev;
            }
            return *this;
        }

        reverse_ring_iterator& operator--()
        {
            m_node = m_node->next;
            if ( m_node->ishead )
            {
                m_node = m_node->next;
            }
            return *this;
        }
    };

public: // ctor / dtor
    ring_list()
    {
        m_size = 0;
        m_head = new list_node();
    }

    explicit ring_list(size_t count) : ring_list(count, T())
    {
    }

    ring_list(size_t count, const T& value) : ring_list()
    {
        while ( count-- )
        {
            push_back(value);
        }
    }

    template<class InputIterator>
    ring_list(InputIterator first, InputIterator last) : ring_list()
    {
        for ( auto it = first; it != last; ++it )
        {
            push_back(*it);
        }
    }

    ring_list(const ring_list& lhs) : ring_list()
    {
        operator=(lhs);
    }

    ring_list(ring_list&& rhs) noexcept
    {
        operator=(rhs);
    }

    ring_list(std::initializer_list<T> il) : ring_list()
    {
        for ( const auto& value : il )
        {
            push_back(value);
        }
    }

    ~ring_list()
    {
        clear();

        if ( m_head )
        {
            delete m_head;
            m_head = nullptr;
        }
    }

public: // operators
    ring_list& operator=(const ring_list& lhs)
    {
        if ( this == &lhs )
        {
            return *this;
        }

        clear();

        for ( const auto& value : lhs )
        {
            push_back(value);
        }

        return *this;
    }

    ring_list& operator=(ring_list&& rhs) noexcept
    {
        if ( this == &rhs )
        {
            return *this;
        }

        clear();

        m_size = rhs.m_size;
        m_head = rhs.m_head;

        rhs.m_size = 0;
        rhs.m_head = nullptr;

        return *this;
    }

    ring_list& operator=(std::initializer_list<T> il)
    {
        clear();

        for ( const auto& value : il )
        {
            push_back(value);
        }

        return *this;
    }

public: // properties
    bool empty() const noexcept
    {
        return m_size == 0;
    };

    size_t size() const noexcept
    {
        return m_size;
    };

    T& front()
    {
        return m_head->next->value;
    };

    T& back()
    {
        return m_head->prev->value;
    };

    const T& front() const
    {
        return m_head->m_nex->value;
    };

    const T& back() const
    {
        return m_head->prev->value;
    };

public: // methods
    template<class InputIterator>
    void assign(InputIterator first, InputIterator last)
    {
        auto it = first;
        for ( auto node = m_head->next; node != m_head; node = node->next )
        {
            if ( it == last )
            {
                break;
            }
            node->value = *it;
            ++it;
        }
    }

    void assign(size_t count, const T& value)
    {
        for ( auto node = m_head->next; node != m_head; node = node->next )
        {
            if ( count == 0 )
            {
                break;
            }
            node->value = value;
            --count;
        }
    }

    void assign(std::initializer_list<T> il)
    {
        auto it = il.begin();
        const auto end = il.end();
        for ( auto node = m_head->next; node != m_head; node = node->next )
        {
            if ( it == end )
            {
                break;
            }
            node->value = *it;
            ++it;
        }
    }

    template<class... Args>
    void emplace_back(Args&&... args)
    {
        const auto node = new list_node(std::move(T(args...)));
        node->prev = m_head->prev;
        node->next = m_head;

        m_head->prev->next = node;
        m_head->prev = node;

        ++m_size;
    }

    template<class... Args>
    void emplace_front(Args&&... args)
    {
        const auto node = new list_node(std::move(T(args...)));
        node->next = m_head->next;
        node->prev = m_head;

        m_head->next->prev = node;
        m_head->next = node;

        ++m_size;
    }

    void push_back(const T& value)
    {
        const auto node = new list_node(value);
        node->prev = m_head->prev;
        node->next = m_head;

        m_head->prev->next = node;
        m_head->prev = node;

        ++m_size;
    }

    void push_back(T&& value)
    {
        const auto node = new list_node(std::move(value));
        node->prev = m_head->prev;
        node->next = m_head;

        m_head->prev->next = node;
        m_head->prev = node;

        ++m_size;
    }

    void push_front(const T& value)
    {
        const auto node = new list_node(value);
        node->next = m_head->next;
        node->prev = m_head;

        m_head->next->prev = node;
        m_head->next = node;

        ++m_size;
    }

    void push_front(T&& value)
    {
        const auto node = new list_node(std::move(value));
        node->next = m_head->next;
        node->prev = m_head;

        m_head->next->prev = node;
        m_head->next = node;

        ++m_size;
    }

    void pop_back()
    {
        if ( m_size )
        {
            const auto node = m_head->prev;
            node->prev->next = node->next;
            node->next->prev = node->prev;
            delete node;

            --m_size;
        }
    }

    void pop_front()
    {
        if ( m_size )
        {
            const auto node = m_head->next;
            node->prev->next = node->next;
            node->next->prev = node->prev;
            delete node;

            --m_size;
        }
    }

    template<class... Args>
    iterator emplace(const_iterator pos, Args&&... args)
    {
        const auto dst = pos.m_node;

        const auto node = new list_node(args...);
        node->prev = dst->prev;
        node->next = dst;

        dst->prev->next = node;
        dst->prev = node;

        ++m_size;

        return iterator(node);
    }

    iterator insert(const_iterator pos, const T& value)
    {
        const auto dst = pos.m_node;

        const auto node = new list_node(value);
        node->prev = dst->prev;
        node->next = dst;

        dst->prev->next = node;
        dst->prev = node;

        ++m_size;

        return iterator(node);
    }

    iterator insert(const_iterator pos, size_t count, const T& value)
    {
        auto node = pos.m_node->prev;

        const auto dst = pos.m_node;
        while ( count-- )
        {
            const auto n = new list_node(value);
            n->prev = dst->prev;
            n->next = dst;

            dst->prev->next = n;
            dst->prev = n;
        }

        m_size += count;
        return iterator(node->next);
    }

    iterator insert(const_iterator pos, T&& value)
    {
        const auto dst = pos.m_node;

        const auto node = new list_node(std::move(value));
        node->prev = dst->prev;
        node->next = dst;

        dst->prev->next = node;
        dst->prev = node;

        ++m_size;

        return iterator(node);
    }

    iterator erase(const_iterator pos)
    {
        const auto node = pos.m_node;
        if ( node->is_head )
        {
            return iterator(m_head);
        }

        auto next = node->next;

        node->prev->next = node->next;
        node->next->prev = node->prev;
        delete node;

        --m_size;

        return iterator(next);
    }

    void resize(size_t count)
    {
        if ( m_size < count )
        {
            while ( m_size != count )
            {
                push_back(T());
            }
        }
        else if ( m_size > count )
        {
            auto node = m_head->prev;
            while ( m_size != count )
            {
                const auto prev = node->prev;

                node->prev->next = node->next;
                node->next->prev = node->prev;
                delete node;
                
                node = prev;

                --m_size;
            }
        }
    }

    void resize(size_t count, const T& value)
    {
        if ( m_size < count )
        {
            while ( m_size != count )
            {
                push_back(T(value));
            }
        }
        else if ( m_size > count )
        {
            auto node = m_head->prev;
            while ( m_size != count )
            {
                const auto prev = node->prev;

                node->prev->next = node->next;
                node->next->prev = node->prev;
                delete node;

                node = prev;

                --m_size;
            }
        }
    }

    void swap(ring_list& other) noexcept
    {
        if ( this == &other )
        {
            return;
        }

        const auto size = m_size;
        const auto head = m_head;

        m_size = other.m_size;
        m_head = other.m_head;

        other.m_size = size;
        other.m_head = head;
    }

    void clear() noexcept
    {
        if ( m_size == 0 )
        {
            return;
        }

        auto node = m_head->next;
        while ( !node->is_head )
        {
            const auto next = node->next;
            delete node;
            node = next;
        }

        m_head->prev = m_head->next = m_head;
        m_size = 0;
    }

    void remove(const T& value)
    {
        auto node = m_head->next;
        while ( !node->is_head )
        {
            const auto next = node->next;
            if( node->value == value )
            {
                node->prev->next = node->next;
                node->next->prev = node->prev;
                delete node;
                
                --m_size;
            }
            node = next;
        }
    }

    void remove_if(const std::function<bool (const T&)>& pred)
    {
        auto node = m_head->next;
        while ( !node->is_head )
        {
            const auto next = node->next;
            if( pred(node->value) )
            {
                node->prev->next = node->next;
                node->next->prev = node->prev;
                delete node;
                
                --m_size;
            }
            node = next;
        }
    }

public: // iterators
    iterator begin() noexcept
    {
        return iterator(m_head->next);
    }

    iterator end() noexcept
    {
        return iterator(m_head);
    }

    const_iterator begin() const noexcept
    {
        return const_iterator(m_head->next);
    }

    const_iterator end() const noexcept
    {
        return const_iterator(m_head);
    }

    const_iterator cbegin() const noexcept
    {
        return const_iterator(m_head->next);
    }

    const_iterator cend() const noexcept
    {
        return const_iterator(m_head);
    }
    
    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(m_head->prev);
    }

    reverse_iterator rend() noexcept
    {
        return reverse_iterator(m_head);
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(m_head->prev);
    }

    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(m_head);
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(m_head->prev);
    }

    const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(m_head);
    }

    ring_iterator ring_begin() noexcept
    {
        return ring_iterator(m_head->next);
    }

    ring_iterator ring_end() noexcept
    {
        return ring_iterator(m_head);
    }

    const_ring_iterator ring_begin() const noexcept
    {
        return const_ring_iterator(m_head->next);
    }

    const_ring_iterator ring_end() const noexcept
    {
        return const_ring_iterator(m_head);
    }

    const_ring_iterator ring_cbegin() const noexcept
    {
        return const_ring_iterator(m_head->next);
    }

    const_ring_iterator ring_cend() const noexcept
    {
        return const_ring_iterator(m_head);
    }
    
    reverse_ring_iterator ring_rbegin() noexcept
    {
        return reverse_ring_iterator(m_head->prev);
    }

    reverse_ring_iterator ring_rend() noexcept
    {
        return reverse_ring_iterator(m_head);
    }

    const_reverse_ring_iterator ring_rbegin() const noexcept
    {
        return const_reverse_ring_iterator(m_head->prev);
    }

    const_reverse_ring_iterator ring_rend() const noexcept
    {
        return const_reverse_ring_iterator(m_head);
    }

    const_reverse_ring_iterator ring_crbegin() const noexcept
    {
        return const_reverse_ring_iterator(m_head->prev);
    }

    const_reverse_ring_iterator ring_crend() const noexcept
    {
        return const_reverse_ring_iterator(m_head);
    }

protected: // members
    size_t     m_size;
    list_node* m_head;

public:
    static void unittest();
};

//---------------------------------------------------------------------------//

} //namespace tapetums

//---------------------------------------------------------------------------//
// Unit Test
//---------------------------------------------------------------------------//

#if defined(TEST)

#include <string>

#endif

//---------------------------------------------------------------------------//

namespace tapetums {

//---------------------------------------------------------------------------//

#if defined(TEST)

template<typename T>
inline void ring_list<T>::unittest()
{
    size_t idx;
    ring_list<std::string> ring;

    printf("<push_back(%%zu) * 10 times>\n");
    char buf[64];
    for ( size_t i = 0; i < 10; ++i )
    {
        sprintf_s(buf, 64, "push_back(%zu)", i);
        ring.push_back(buf);
    }

    idx = 0;
    for ( auto& str : ring )
    {
        printf("%02zu: %s\n", idx, str.c_str());
        ++idx;
    }
    printf("\n");

    printf("<operations>\n");
    printf("  push_front(\"push_front\")\n");
    ring.push_front("push_front");
    ring.emplace_front("emplace_front");
    printf("  emplace_front(\"emplace_front\")\n");
    ring.emplace_back("emplace_back");
    printf("  emplace_back(\"emplace_back\")\n");
    ring.remove("push_back(7)");
    printf("  push_back(\"push_back(7)\")\n");
    auto node = ring.insert(++(++(++ring.cbegin())), "insert");
    printf("  insert(\"insert\")\n");
    auto next = ring.erase(++node);
    printf("  erase(++(++(++(++ring.cbegin())))\n");
    ring.emplace(++next, "emplace");
    printf("  emplace(\"emplace\")\n");
    idx = 0;
    for ( auto& str : ring )
    {
        printf("%02zu: %s\n", idx, str.c_str());
        ++idx;
    }
    printf("\n");

    printf("<reverse iterators>\n");
    idx = 0;
    const auto z = ring.size();
    const auto rend = ring.crend();
    for ( auto it = ring.rbegin(); it != rend; ++it )
    {
        printf("%02zu: %s\n", z - idx - 1, it->c_str());
        ++idx;
    }
    printf("\n");

    printf("<remove_if(\"(\")>\n");
    ring.remove_if([](const std::string& value)
    {
        return std::string::npos != value.find("(");
    });
    idx = 0;
    for ( auto& str : ring )
    {
        printf("%02zu: %s\n", idx, str.c_str());
        ++idx;
    }
    printf("\n");

    printf("<back() == --ring.end() ?>\n");
    auto it = ring.end();
    --it;
    if ( *it == ring.back() )
    {
        printf("back: %s\n", it->c_str());
    }
    printf("\n");

    printf("<front() == ++ring.end() ?>\n");
    it = ring.end();
    ++it;
    if ( *it == ring.front() )
    {
        printf("front: %s\n", it->c_str());
    }
    printf("\n");

    printf("<pop_front()>\n");
    ring.pop_front();
    printf("front: %s\n", ring.begin()->c_str());
    printf("\n");

    printf("<loop * 20 times>\n");
    printf("ring.size() = %zu\n", ring.size());
    auto ring_it = ring.ring_begin();
    for ( auto i = 0; i < 20; ++i )
    {
        printf("%02u: %s\n", i, ring_it->c_str());
        ++ring_it;
    }
    printf("\n");

    printf("<end()>\n");
    printf("end: %s\n", ring.end()->c_str());
    printf("\n");

    printf("<operations>\n");
    printf("  pop_front()\n");
    ring.pop_front();
    printf("  resize(3)\n");
    ring.resize(3);
    printf("  pop_back()\n");
    ring.pop_back();
    idx = 0;
    for ( auto& str : ring )
    {
        printf("%02zu: %s\n", idx, str.c_str());
        ++idx;
    }
    printf("\n");

    printf("<reverse iterators>\n");
    auto rit = ring.rbegin();
    const auto zz = ring.size();
    for ( size_t i = 0; i < zz; ++i, ++rit )
    {
        printf("%02zu: %s\n", zz - i - 1, rit->c_str());
    }
    printf("\n");

    printf("<clear()>\n");
    ring.clear();
    for ( auto& str : ring )
    {
        printf("%s\n", str.c_str());
    }
    printf("ring.size() = %zu\n", ring.size());
    printf("\n");

    printf("<resize(8, \"poo\")>\n");
    ring.resize(8, "poo");
    idx = 0;
    for ( auto& str : ring )
    {
        printf("%02zu: %s\n", idx, str.c_str());
        ++idx;
    }
    printf("\n");

    printf("<assign(\"1\" ~ \"3\")>\n");
    ring.assign({ "1", "2", "3", });
    idx = 0;
    for ( auto& str : ring )
    {
        printf("%02zu: %s\n", idx, str.c_str());
        ++idx;
    }
    printf("\n");

    printf("<assign(\"1\" ~ \"0\")>\n");
    ring.assign({ "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", });
    idx = 0;
    for ( auto& str : ring )
    {
        printf("%02zu: %s\n", idx, str.c_str());
        ++idx;
    }
    printf("\n");

    printf("<make another ring with initializer_list{\"A\" ~ \"E\"}>\n");
    ring_list<std::string> another{ "A", "B", "C", "D", "E", };
    idx = 0;
    for ( auto& str : another )
    {
        printf("%02zu: %s\n", idx, str.c_str());
        ++idx;
    }
    printf("\n");

    printf("<insert(\"qwerty\") * 4 times to the front fo another>\n");
    another.insert(another.begin(), 4, "qwerty");
    idx = 0;
    for ( auto& str : another )
    {
        printf("%02zu: %s\n", idx, str.c_str());
        ++idx;
    }
    printf("\n");

    printf("<assign({\"z\", \"x\"}) to the front of another>\n");
    another.assign({ "z", "x" });
    idx = 0;
    for ( auto& str : another )
    {
        printf("%02zu: %s\n", idx, str.c_str());
        ++idx;
    }
    printf("\n");

    printf("<swap() original and another>\n");
    ring.swap(another);
    printf("  original ring:\n");
    idx = 0;
    for ( auto& str : ring )
    {
        printf("%02zu: %s\n", idx, str.c_str());
        ++idx;
    }
    printf("  another ring:\n");
    idx = 0;
    for ( auto& str : another )
    {
        printf("%02zu: %s\n", idx, str.c_str());
        ++idx;
    }

    return;
}

#else

template<typename T>
inline void ring_list<T>::unittest() { }

#endif

//---------------------------------------------------------------------------//

} //namespace tapetums

//---------------------------------------------------------------------------//

// ring_list.hpp
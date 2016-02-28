#pragma once

//---------------------------------------------------------------------------//
//
// Dom.hpp
//  DOM データ格納クラス
//   Copyright (C) 2007-2014 tapetums
//
//---------------------------------------------------------------------------//

template <typename T>
class Dom
{
public:
    class iterator
    {
    private:
        Dom* m_dom = nullptr;

    private:
        iterator() = delete;

    public:
        iterator(Dom* dom)
        {
            m_dom = dom;
        }

        iterator(const iterator& lhs)
        {
            m_dom = lhs.m_dom;
        }

        iterator(iterator&& rhs)
        {
            m_dom = rhs.m_dom;
            rhs.m_dom = nullptr;
        }

    public:
        iterator& __stdcall operator =(const iterator& lhs) const
        {
            m_dom = lhs.m_dom;
            return *this;
        }

        iterator& __stdcall operator =(iterator&& rhs)
        {
            m_dom = rhs.m_dom;
            rhs.m_dom = nullptr;

            return *this;
        }

        bool __stdcall operator ==(const iterator& it) const
        {
            return (m_dom == it.m_dom);
        }

        bool __stdcall operator !=(const iterator& it) const
        {
            return (m_dom != it.m_dom);
        }

        Dom* __stdcall operator *() const
        {
            return m_dom;
        }

        Dom* __stdcall operator ->() const
        {
            return m_dom;
        }

        iterator& __stdcall operator ++()
        {
            m_dom = m_dom->m_next;
            return *this;
        }

        iterator& __stdcall operator --()
        {
            m_dom = m_dom->m_prev;
            return *this;
        }
    };

public:
    T data;

protected:
    Dom* m_head;
    Dom* m_next;
    Dom* m_prev;
    Dom* m_parent;
    Dom* m_child;

public:
    Dom()
    {
        init();
    }

    ~Dom()
    {
        clear();
    }

private:
    Dom(Dom* head, const T& data)
    {
        m_head   = head;
        m_next   = head;
        m_prev   = head->m_prev;
        m_parent = nullptr;
        m_child  = nullptr;

        this->data = data;
    }

    void __stdcall init()
    {
        m_head   = this;
        m_next   = this;
        m_prev   = this;
        m_parent = nullptr;
        m_child  = nullptr;
    }

public:
    Dom* __stdcall append(const T& data)
    {
        if ( this == m_next )
        {
            m_head = new Dom;
            m_head->m_next = this;
            m_head->m_prev = this;
            m_next = m_head;
            m_prev = m_head;
        }

        const auto dom = new Dom(m_head, data);
        m_head->m_prev->m_next = dom;
        m_head->m_prev = dom;

        return dom;
    }

    Dom* __stdcall adopt(const T& data)
    {
        if ( nullptr == m_child )
        {
            m_child = new Dom;
            m_child->m_parent = this;
        }

        const auto dom = new Dom(m_child, data);
        m_child->m_prev->m_next = dom;
        m_child->m_prev = dom;

        return dom;
    }

    void __stdcall clear()
    {
        if ( m_child )
        {
            m_child->clear();
            delete m_child;
            m_child = nullptr;
        }

        auto dom = m_next;
        while ( dom != this )
        {
            const auto next = dom->m_next;

            dom->m_next = dom;
            delete dom;

            dom = next;
        }

        init();
    }

    iterator __stdcall remove()
    {
        if ( this == m_head )
        {
            return std::move(iterator(this));
        }

        iterator it(m_next);

        m_next->m_prev = m_prev;
        m_prev->m_next = m_next;

        m_head = this;
        m_next = this;
        m_prev = this;

        return std::move(it);
    }

public:
    iterator __stdcall begin() const
    {
        return iterator(m_head->m_next);
    }

    iterator __stdcall end() const
    {
        return iterator(m_head);
    }

    Dom* __stdcall parent() const
    {
        return m_head->m_parent;
    }

    Dom* __stdcall child() const
    {
        return m_child;
    }
};

//---------------------------------------------------------------------------//

// Dom.hpp
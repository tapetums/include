#pragma once

//---------------------------------------------------------------------------//
//
// result11.hpp
//  2種類の結果を返す型 (簡易版) (C++11)
//   Copyright (C) 2016-2017 tapetums
//
// GCC:   above 7.1
// clang: above 3.4
// VC++:  above 2015
//
//---------------------------------------------------------------------------//

#include <utility>

//---------------------------------------------------------------------------//
// Forward Declarations
//---------------------------------------------------------------------------//

namespace tapetums
{
    struct none_t { } static constexpr none { };

    template <typename V, typename E> class result;
    template <typename T> using optional = result<T, void>;

    template <typename V, typename E> result<V, E> make_value(const V& value);
    template <typename V, typename E> result<V, E> make_value(V&& value);
    template <typename V, typename E> result<V, E> make_value();
    template <typename V, typename E> result<V, E> make_error(const E& error);
    template <typename V, typename E> result<V, E> make_error(E&& error);
    template <typename V, typename E> result<V, E> make_error();
}

//---------------------------------------------------------------------------//
// Classes
//---------------------------------------------------------------------------//

template <typename V, typename E>
class tapetums::result final
{
private:
    bool has_value;

    union
    {
        V m_value;
        E m_error;
    };

public:
    result() = delete;

    result(const result&)             = delete;
    result& operator =(const result&) = delete;

    result(result&& rhs)             noexcept { swap(std::move(rhs)); }
    result& operator =(result&& rhs) noexcept { swap(std::move(rhs)); return *this; }

    result(const V& value) : has_value(true), m_value(value)            { }
    result(V&& value)      : has_value(true), m_value(std::move(value)) { }

    result(const E& error) : has_value(false), m_error(error)            { }
    result(E&& error)      : has_value(false), m_error(std::move(error)) { }

    result(none_t, const E& error ) : has_value(false), m_error(error)            { }
    result(none_t, E&& error)       : has_value(false), m_error(std::move(error)) { }

    template <typename... Args>
    result(Args&&... args) : has_value(true), m_value(std::forward<Args>(args)...) { }

    template <typename... Args>
    result(none_t, Args&&... args) : has_value(false), m_error(std::forward<Args>(args)...) { }

    ~result()
    {
        if ( has_value ) { m_value.~V(); }
        else             { m_error.~E(); }
    }

public:
    void swap(result&& rhs) noexcept { if ( this != &rhs ) { std::swap(*this, rhs); } }

public:
    bool valid     () const noexcept { return has_value; }
    bool operator !() const noexcept { return ! valid(); }
    explicit operator bool() const noexcept { return valid(); }

    const V* operator ->() const noexcept { return std::addressof(m_value); }
    V*       operator ->()       noexcept { return std::addressof(m_value); }

    const V& operator *() const &  noexcept { return m_value; }
    V&       operator *()       &  noexcept { return m_value; }
    V&&      operator *()       && noexcept { return std::move(m_value); }

public:
    const V& value() const & noexcept { return m_value; }
    const E& error() const & noexcept { return m_error; }

    const V&& value() const && noexcept { return m_value; }
    const E&& error() const && noexcept { return m_error; }

    V& value() & noexcept { return m_value; }
    E& error() & noexcept { return m_error; }

    V&& value() && noexcept { return m_value; }
    E&& error() && noexcept { return m_error; }

    template <typename... Args>
    const V value_or(Args&&... args) const & { return *this ? m_value : V(std::forward<Args>(args)...); }

    template <typename... Args>
    V       value_or(Args&&... args) &&      { return *this ? m_value : V(std::forward<Args>(args)...); }
};

//---------------------------------------------------------------------------//
// Template Specializations
//---------------------------------------------------------------------------//

template <typename T>
class tapetums::result<T, T> final
{
private:
    bool has_value;

    union
    {
        T m_value;
        T m_error;
    };

public:
    result() = delete;

    result(const result&)             = delete;
    result& operator =(const result&) = delete;

    result(result&& rhs)             noexcept { swap(std::move(rhs)); }
    result& operator =(result&& rhs) noexcept { swap(std::move(rhs)); return *this; }

    result(const T& value) : has_value(true), m_value(value)            { }
    result(T&& value)      : has_value(true), m_value(std::move(value)) { }

    result(none_t, const T& error) : has_value(false), m_error(error)            { }
    result(none_t, T&& error)      : has_value(false), m_error(std::move(error)) { }

    template <typename... Args>
    result(Args&&... args) : has_value(true), m_value(std::forward<Args>(args)...) { }

    template <typename... Args>
    result(none_t, Args&&... args) : has_value(false), m_error(std::forward<Args>(args)...) { }

    ~result() { m_value.~T(); }

public:
    void swap(result&& rhs) noexcept { if ( this != &rhs ) { std::swap(*this, rhs); } }

public:
    bool valid     () const noexcept { return has_value; }
    bool operator !() const noexcept { return ! valid(); }
    explicit operator bool() const noexcept { return valid(); }

    const T* operator ->() const noexcept { return std::addressof(m_value); }
    T*       operator ->()       noexcept { return std::addressof(m_value); }

    const T& operator *() const &  noexcept { return m_value; }
    T&       operator *()       &  noexcept { return m_value; }
    T&&      operator *()       && noexcept { return std::move(m_value); }

public:
    const T& value() const & noexcept { return m_value; }
    const T& error() const & noexcept { return m_error; }

    const T&& value() const && noexcept { return m_value; }
    const T&& error() const && noexcept { return m_error; }

    T& value() & noexcept { return m_value; }
    T& error() & noexcept { return m_error; }

    T&& value() && noexcept { return m_value; }
    T&& error() && noexcept { return m_error; }

    template <typename... Args>
    const T value_or(Args&&... args) const & { return *this ? m_value : T(std::forward<Args>(args)...); }

    template <typename... Args>
    T       value_or(Args&&... args) &&      { return *this ? m_value : T(std::forward<Args>(args)...); }
};

//---------------------------------------------------------------------------//

template <typename V>
class tapetums::result<V, void> final
{
private:
    bool has_value;

    union
    {
        unsigned char dummy;
        V m_value;
    };

public:
    result() noexcept : has_value(false), dummy() { }

    result(const result&)             = delete;
    result& operator =(const result&) = delete;

    result(result&& rhs)             noexcept { swap(std::move(rhs)); }
    result& operator =(result&& rhs) noexcept { swap(std::move(rhs)); return *this; }

    result(const V& value) : has_value(true), m_value(value)            { }
    result(V&& value)      : has_value(true), m_value(std::move(value)) { }

    result(none_t) noexcept : has_value(false), dummy() { }

    template <typename... Args>
    result(Args&&... args) : has_value(true), m_value(std::forward<Args>(args)...) { }

    ~result() { if ( has_value ) { m_value.~V(); } }

public:
    void swap(result&& rhs) noexcept { if ( this != &rhs ) { std::swap(*this, rhs); } }

public:
    bool valid     () const noexcept { return has_value; }
    bool operator !() const noexcept { return ! valid(); }
    explicit operator bool() const noexcept { return valid(); }

    const V* operator ->() const noexcept { return std::addressof(m_value); }
    V*       operator ->()       noexcept { return std::addressof(m_value); }

    const V& operator *() const &  noexcept { return m_value; }
    V&       operator *()       &  noexcept { return m_value; }
    V&&      operator *()       && noexcept { return std::move(m_value); }

public:
    const V&  value() const &  noexcept { return m_value; }
    const V&& value() const && noexcept { return m_value; }

    V&  value() &  noexcept { return m_value; }
    V&& value() && noexcept { return m_value; }

    template <typename... Args>
    const V value_or(Args&&... args) const & { return *this ? m_value : V(std::forward<Args>(args)...); }

    template <typename... Args>
    V       value_or(Args&&... args) &&      { return *this ? m_value : V(std::forward<Args>(args)...); }
};

//---------------------------------------------------------------------------//

template <typename E>
class tapetums::result<void, E> final
{
private:
    bool has_value;

    union
    {
        unsigned char dummy;
        E m_error;
    };

public:
    result() noexcept : has_value(true), dummy() { }

    result(const result&)             = delete;
    result& operator =(const result&) = delete;

    result(result&& rhs)             noexcept { swap(std::move(rhs)); }
    result& operator =(result&& rhs) noexcept { swap(std::move(rhs)); return *this; }

    result(const E& error) : has_value(false), m_error(error)            { }
    result(E&& error)      : has_value(false), m_error(std::move(error)) { }

    result(none_t, const E& error) : has_value(false), m_error(error)            { }
    result(none_t, E&& error)      : has_value(false), m_error(std::move(error)) { }

    template <typename... Args>
    result(none_t, Args&&... args) : has_value(false), m_error(std::forward<Args>(args)...) { }

    ~result() { if ( ! has_value ) { m_error.~E(); } }

public:
    void swap(result&& rhs) noexcept { if ( this != &rhs ) { std::swap(*this, rhs); } }

public:
    bool valid     () const noexcept { return has_value; }
    bool operator !() const noexcept { return ! valid(); }
    explicit operator bool() const noexcept { return valid(); }

public:
    const E&  error() const &  noexcept { return m_error; }
    const E&& error() const && noexcept { return m_error; }

    E&  error() &  noexcept { return m_error; }
    E&& error() && noexcept { return m_error; }
};

//---------------------------------------------------------------------------//

template <typename V, typename E>
class tapetums::result<V*, E> final
{
private:
    bool has_value;

    union
    {
        V* m_value;
        E  m_error;
    };

public:
    result() = delete;

    result(const result&)             = delete;
    result& operator =(const result&) = delete;

    result(result&& rhs)             noexcept { swap(std::move(rhs)); }
    result& operator =(result&& rhs) noexcept { swap(std::move(rhs)); return *this; }

    result(V* value) noexcept : has_value(true), m_value(value) { }

    result(const E& error) : has_value(false), m_error(error)            { }
    result(E&& error)      : has_value(false), m_error(std::move(error)) { }

    result(none_t, const E& error) : has_value(false), m_error(error)            { }
    result(none_t, E&& error)      : has_value(false), m_error(std::move(error)) { }

    template <typename... Args>
    result(none_t, Args&&... args) : has_value(false), m_error(std::forward<Args>(args)...) { }

    ~result() { if ( ! has_value ) { m_error.~E(); } }

public:
    void swap(result&& rhs) noexcept { if ( this != &rhs ) { std::swap(*this, rhs); } }

public:
    bool valid     () const noexcept { return has_value; }
    bool operator !() const noexcept { return ! valid(); }
    explicit operator bool() const noexcept { return valid(); }

    const V* operator ->() const noexcept { return m_value; }
    V*       operator ->()       noexcept { return m_value; }

    const V* operator *() const noexcept { return m_value; }
    V*       operator *()       noexcept { return m_value; }

public:
    const V* value() const & noexcept { return m_value; }
    const E& error() const & noexcept { return m_error; }

    const E&& error() const && noexcept { return m_error; }

    V* value() & noexcept { return m_value; }
    E& error() & noexcept { return m_error; }

    E&& error() && noexcept { return m_error; }

    const V* value_or(V* default_value) const & noexcept { return *this ? m_value : default_value; }
    V*       value_or(V* default_value) &&      noexcept { return *this ? m_value : default_value; }
};

//---------------------------------------------------------------------------//

template <typename V, typename E>
class tapetums::result<V, E*> final
{
private:
    bool has_value;

    union
    {
        V  m_value;
        E* m_error;
    };

public:
    result() = delete;

    result(const result&)             = delete;
    result& operator =(const result&) = delete;

    result(result&& rhs)             noexcept { swap(std::move(rhs)); }
    result& operator =(result&& rhs) noexcept { swap(std::move(rhs)); return *this; }

    result(const V& value) : has_value(true), m_value(value)            { }
    result(V&& value)      : has_value(true), m_value(std::move(value)) { }

    result(E* error) noexcept : has_value(false), m_error(error) { }

    result(none_t, E* error) noexcept : has_value(false), m_error(error) { }

    template <typename... Args>
    result(Args&&... args) : has_value(true), m_value(std::forward<Args>(args)...) { }

    ~result() { if ( has_value ) { m_value.~V(); } }

public:
    void swap(result&& rhs) noexcept { if ( this != &rhs ) { std::swap(*this, rhs); } }

public:
    bool valid     () const noexcept { return has_value; }
    bool operator !() const noexcept { return ! valid(); }
    explicit operator bool() const noexcept { return valid(); }

    const V* operator ->() const noexcept { return std::addressof(m_value); }
    V*       operator ->()       noexcept { return std::addressof(m_value); }

    const V& operator *() const &  noexcept { return m_value; }
    V&       operator *()       &  noexcept { return m_value; }
    V&&      operator *()       && noexcept { return std::move(m_value); }

public:
    const V& value() const & noexcept { return m_value; }
    const E* error() const & noexcept { return m_error; }

    const V&& value() const && noexcept { return m_value; }

    V& value() & noexcept { return m_value; }
    E* error() & noexcept { return m_error; }

    V&& value() && noexcept { return m_value; }

    template <typename... Args>
    const V value_or(Args&&... args) const & { return *this ? m_value : V(std::forward<Args>(args)...); }

    template <typename... Args>
    V       value_or(Args&&... args) &&      { return *this ? m_value : V(std::forward<Args>(args)...); }
};

//---------------------------------------------------------------------------//

template <typename V, typename E>
class tapetums::result<V*, E*> final
{
private:
    bool has_value;

    union
    {
        V* m_value;
        E* m_error;
    };

public:
    result() = delete;

    result(const result&)             = default;
    result& operator =(const result&) = default;

    result(result&& rhs)             noexcept = default;
    result& operator =(result&& rhs) noexcept = default;

    result(V* value) noexcept : has_value(true), m_value(value) { }

    result(E* error) noexcept : has_value(false), m_error(error) { }

    result(none_t, E* error) noexcept : has_value(false), m_error(error) { }

    ~result() = default;

public:
    void swap(result&& rhs) noexcept { if ( this != &rhs ) { std::swap(*this, rhs); } }

public:
    bool valid     () const noexcept { return has_value; }
    bool operator !() const noexcept { return ! valid(); }
    explicit operator bool() const noexcept { return valid(); }

    const V* operator ->() const noexcept { return m_value; }
    V*       operator ->()       noexcept { return m_value; }

    const V* operator *() const noexcept { return m_value; }
    V*       operator *()       noexcept { return m_value; }

public:
    const V* value() const & noexcept { return m_value; }
    const E* error() const & noexcept { return m_error; }

    V* value() & noexcept { return m_value; }
    E* error() & noexcept { return m_error; }

    const V* value_or(V* default_value) const & noexcept { return *this ? m_value : default_value; }
    V*       value_or(V* default_value) &&      noexcept { return *this ? m_value : default_value; }
};

//---------------------------------------------------------------------------//

template <typename T>
class tapetums::result<T*, T*> final
{
private:
    bool has_value;

    union
    {
        T* m_value;
        T* m_error;
    };

public:
    result() = delete;

    result(const result&)             = default;
    result& operator =(const result&) = default;

    result(result&& rhs)             noexcept = default;
    result& operator =(result&& rhs) noexcept = default;

    result(T* value) noexcept : has_value(true), m_value(value) { }

    result(none_t, T* error) noexcept : has_value(false), m_error(error) { }

    ~result() = default;

public:
    void swap(result&& rhs) noexcept { if ( this != &rhs ) { std::swap(*this, rhs); } }

public:
    bool valid     () const noexcept { return has_value; }
    bool operator !() const noexcept { return ! valid(); }
    explicit operator bool() const noexcept { return valid(); }

    const T* operator ->() const noexcept { return m_value; }
    T*       operator ->()       noexcept { return m_value; }

    const T* operator *() const noexcept { return m_value; }
    T*       operator *()       noexcept { return m_value; }

public:
    const T* value() const & noexcept { return m_value; }
    const T* error() const & noexcept { return m_error; }

    T* value() & noexcept { return m_value; }
    T* error() & noexcept { return m_error; }

    const T* value_or(T* default_value) const & noexcept { return *this ? m_value : default_value; }
    T*       value_or(T* default_value) &&      noexcept { return *this ? m_value : default_value; }
};

//---------------------------------------------------------------------------//

template <typename V>
class tapetums::result<V*, void> final
{
private:
    bool has_value;

    union
    {
        unsigned char dummy;
        V* m_value;
    };

public:
    result() noexcept : has_value(false), dummy() { }

    result(const result&)             = default;
    result& operator =(const result&) = default;

    result(result&& rhs)             noexcept = default;
    result& operator =(result&& rhs) noexcept = default;

    result(V* value) noexcept : has_value(true), m_value(value) { }

    result(none_t) noexcept : has_value(false), dummy() { }

    ~result() = default;

public:
    void swap(result&& rhs) noexcept { if ( this != &rhs ) { std::swap(*this, rhs); } }

public:
    bool valid     () const noexcept { return has_value; }
    bool operator !() const noexcept { return ! valid(); }
    explicit operator bool() const noexcept { return valid(); }

    const V* operator ->() const noexcept { return m_value; }
    V*       operator ->()       noexcept { return m_value; }

    const V* operator *() const noexcept { return m_value; }
    V*       operator *()       noexcept { return m_value; }

public:
    const V* value() const & noexcept { return m_value; }
    V*       value()       & noexcept { return m_value; }

    const V* value_or(V* default_value) const & noexcept { return *this ? m_value : default_value; }
    V*       value_or(V* default_value) &&      noexcept { return *this ? m_value : default_value; }
};

//---------------------------------------------------------------------------//

template <typename E>
class tapetums::result<void, E*> final
{
private:
    bool has_value;

    union
    {
        unsigned char dummy;
        E* m_error;
    };

public:
    result() noexcept : has_value(true), dummy() { }

    result(const result&)             = default;
    result& operator =(const result&) = default;

    result(result&& rhs)             noexcept = default;
    result& operator =(result&& rhs) noexcept = default;

    result(E* error) noexcept : has_value(false), m_error(error) { }

    result(none_t, E* error) noexcept : has_value(false), m_error(error) { }

    ~result() = default;

public:
    void swap(result&& rhs) noexcept { if ( this != &rhs ) { std::swap(*this, rhs); } }

public:
    bool valid     () const noexcept { return has_value; }
    bool operator !() const noexcept { return ! valid(); }
    explicit operator bool() const noexcept { return valid(); }

public:
    const E* error() const & noexcept { return m_error; }
    E*       error()       & noexcept { return m_error; }
};

//---------------------------------------------------------------------------//

template <>
class tapetums::result<void, void> final
{
private:
    bool has_value;

public:
    result() noexcept : has_value(true) { }

    result(const result&)             = default;
    result& operator =(const result&) = default;

    result(result&& rhs)             noexcept = default;
    result& operator =(result&& rhs) noexcept = default;

    result(none_t) noexcept : has_value(false) { }

    ~result() = default;

public:
    void swap(result&& rhs) noexcept { if ( this != &rhs ) { std::swap(*this, rhs); } }

public:
    bool valid     () const noexcept { return has_value; }
    bool operator !() const noexcept { return ! valid(); }
    explicit operator bool() const noexcept { return valid(); }
};

//---------------------------------------------------------------------------//
// Utility Functions
//---------------------------------------------------------------------------//

template <typename V, typename E>
tapetums::result<V, E> tapetums::make_value(const V& value)
{
    return result<V, E>(value);
}

//---------------------------------------------------------------------------//

template <typename V, typename E>
tapetums::result<V, E> tapetums::make_value(V&& value)
{
    return result<V, E>(std::move(value));
}

//---------------------------------------------------------------------------//

template <typename V, typename E>
tapetums::result<V, E> tapetums::make_value()
{
    return result<V, E>();
}

//---------------------------------------------------------------------------//

template <typename V, typename E>
tapetums::result<V, E> tapetums::make_error(const E& error)
{
    return result<V, E>(none, error);
}

//---------------------------------------------------------------------------//

template <typename V, typename E>
tapetums::result<V, E> tapetums::make_error(E&& error)
{
    return result<V, E>(none, std::move(error));
}

//---------------------------------------------------------------------------//

template <typename V, typename E>
tapetums::result<V, E> tapetums::make_error()
{
    return result<V, E>(none);
}

//---------------------------------------------------------------------------//

// result11.hpp
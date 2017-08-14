#pragma once

//---------------------------------------------------------------------------//
//
// result14.hpp
//  2種類の結果を返す型 (constexpr版) (C++14)
//   Copyright (C) 2016-2017 tapetums
//
// GCC:   above 7.2
// clang: above 3.6
// VC++:  above 2017
//
//---------------------------------------------------------------------------//

#include <utility>

//---------------------------------------------------------------------------//
// Forward Declarations
//---------------------------------------------------------------------------//

namespace tapetums
{
    namespace detail
    {
        namespace result
        {

            template <typename V, typename E> union trivial_storage;
            template <typename V, typename E> union no_trivial_storage;

            template <typename V, typename E> struct trivial_result_base;
            template <typename V, typename E> struct no_trivial_result_base;
        }
    }

    struct value_t { } static constexpr v { };
    struct error_t { } static constexpr e { }, none { };

    template <typename V, typename E> class result;

    template <typename T> using optional = result<T, void>;
}

//---------------------------------------------------------------------------//
// Storages
//---------------------------------------------------------------------------//

template <typename V, typename E>
union tapetums::detail::result::trivial_storage
{
private:
    unsigned char dummy;

public:
    V value;
    E error;

public:
    constexpr trivial_storage() noexcept : dummy() { }
    constexpr trivial_storage(value_t) noexcept(std::is_nothrow_constructible<V>::value) : value() { }
    constexpr trivial_storage(error_t) noexcept(std::is_nothrow_constructible<E>::value) : error() { }
    ~trivial_storage() noexcept = default;

    template <typename ... Args>
    explicit constexpr trivial_storage(value_t, Args&&... args) noexcept(std::is_nothrow_constructible<V, Args&&...>::value) : value(std::forward<Args>(args)...) { }

    template <typename ... Args>
    explicit constexpr trivial_storage(error_t, Args&&... args) noexcept(std::is_nothrow_constructible<E, Args&&...>::value) : error(std::forward<Args>(args)...) { }
};

//---------------------------------------------------------------------------//

template <typename T>
union tapetums::detail::result::trivial_storage<T, T>
{
private:
    unsigned char dummy;

public:
    T value;
    T error;

public:
    constexpr trivial_storage() noexcept : dummy() { }
    constexpr trivial_storage(value_t) noexcept(std::is_nothrow_constructible<T>::value) : value() { }
    constexpr trivial_storage(error_t) noexcept(std::is_nothrow_constructible<T>::value) : error() { }
    ~trivial_storage() noexcept = default;

    template <typename ... Args>
    explicit constexpr trivial_storage(value_t, Args&&... args) noexcept(std::is_nothrow_constructible<T, Args&&...>::value) : value(std::forward<Args>(args)...) { }

    template <typename ... Args>
    explicit constexpr trivial_storage(error_t, Args&&... args) noexcept(std::is_nothrow_constructible<T, Args&&...>::value) : error(std::forward<Args>(args)...) { }
};

//---------------------------------------------------------------------------//

template <typename V>
union tapetums::detail::result::trivial_storage<V, void>
{
private:
    unsigned char dummy;

public:
    V value;

public:
    constexpr trivial_storage() noexcept : dummy() { }
    constexpr trivial_storage(value_t) noexcept(std::is_nothrow_constructible<V>::value) : value() { }
    constexpr trivial_storage(error_t) noexcept : dummy() { }
    ~trivial_storage() noexcept = default;

    template <typename ... Args>
    explicit constexpr trivial_storage(value_t, Args&&... args) noexcept(std::is_nothrow_constructible<V, Args&&...>::value) : value(std::forward<Args>(args)...) { }
};

//---------------------------------------------------------------------------//

template <typename E>
union tapetums::detail::result::trivial_storage<void, E>
{
private:
    unsigned char dummy;

public:
    E error;

public:
    constexpr trivial_storage() noexcept : dummy() { }
    constexpr trivial_storage(value_t) noexcept : dummy() { }
    constexpr trivial_storage(error_t) noexcept(std::is_nothrow_constructible<E>::value) : error() { }
    ~trivial_storage() noexcept = default;

    template <typename ... Args>
    explicit constexpr trivial_storage(error_t, Args&&... args) noexcept(std::is_nothrow_constructible<E, Args&&...>::value) : error(std::forward<Args>(args)...) { }
};

//---------------------------------------------------------------------------//

template <>
union tapetums::detail::result::trivial_storage<void, void>
{
public:
    constexpr trivial_storage() noexcept = default;
    constexpr trivial_storage(value_t) noexcept { }
    constexpr trivial_storage(error_t) noexcept { }
    ~trivial_storage() noexcept = default;
};

//***************************************************************************//

template <typename V, typename E>
union tapetums::detail::result::no_trivial_storage
{
private:
    unsigned char dummy;

public:
    V value;
    E error;

public:
    constexpr no_trivial_storage() noexcept : dummy() { }
    constexpr no_trivial_storage(value_t) noexcept : dummy() { }
    constexpr no_trivial_storage(error_t) noexcept : dummy() { }
    ~no_trivial_storage() noexcept { }

    template <typename ... Args>
    explicit constexpr no_trivial_storage(value_t, Args&&... args) noexcept(std::is_nothrow_constructible<V, Args&&...>::value) : value(std::forward<Args>(args)...) { }

    template <typename ... Args>
    explicit constexpr no_trivial_storage(error_t, Args&&... args) noexcept(std::is_nothrow_constructible<E, Args&&...>::value) : error(std::forward<Args>(args)...) { }
};

//---------------------------------------------------------------------------//

template <typename T>
union tapetums::detail::result::no_trivial_storage<T, T>
{
private:
    unsigned char dummy;

public:
    T value;
    T error;

public:
    constexpr no_trivial_storage() noexcept : dummy() { }
    constexpr no_trivial_storage(value_t) noexcept : dummy() { }
    constexpr no_trivial_storage(error_t) noexcept : dummy() { }
    ~no_trivial_storage() noexcept { }

    template <typename ... Args>
    explicit constexpr no_trivial_storage(value_t, Args&&... args) noexcept(std::is_nothrow_constructible<T, Args&&...>::value) : value(std::forward<Args>(args)...) { }

    template <typename ... Args>
    explicit constexpr no_trivial_storage(error_t, Args&&... args) noexcept(std::is_nothrow_constructible<T, Args&&...>::value) : error(std::forward<Args>(args)...) { }
};

//---------------------------------------------------------------------------//

template <typename V>
union tapetums::detail::result::no_trivial_storage<V, void>
{
private:
    unsigned char dummy;

public:
    V value;

public:
    constexpr no_trivial_storage() noexcept : dummy() { }
    constexpr no_trivial_storage(value_t) noexcept : dummy() { }
    constexpr no_trivial_storage(error_t) noexcept : dummy() { }
    ~no_trivial_storage() noexcept { }

    template <typename ... Args>
    explicit constexpr no_trivial_storage(value_t, Args&&... args) noexcept(std::is_nothrow_constructible<V, Args&&...>::value) : value(std::forward<Args>(args)...) { }
};

//---------------------------------------------------------------------------//

template <typename E>
union tapetums::detail::result::no_trivial_storage<void, E>
{
private:
    unsigned char dummy;

public:
    E error;

public:
    constexpr no_trivial_storage() noexcept : dummy() { }
    constexpr no_trivial_storage(value_t) noexcept : dummy() { }
    constexpr no_trivial_storage(error_t) noexcept : dummy() { }
    ~no_trivial_storage() noexcept { }

    template <typename ... Args>
    explicit constexpr no_trivial_storage(error_t, Args&&... args) noexcept(std::is_nothrow_constructible<E, Args&&...>::value) : error(std::forward<Args>(args)...) { }
};

//---------------------------------------------------------------------------//

template <>
union tapetums::detail::result::no_trivial_storage<void, void>
{
public:
    constexpr no_trivial_storage() noexcept = default;
    constexpr no_trivial_storage(value_t) noexcept { }
    constexpr no_trivial_storage(error_t) noexcept { }
    ~no_trivial_storage() noexcept = default;
};

//---------------------------------------------------------------------------//
// Bases
//---------------------------------------------------------------------------//

template<typename V, typename E>
struct tapetums::detail::result::trivial_result_base
{
    bool has_value;
    trivial_storage<V, E> storage;

    constexpr trivial_result_base() noexcept(std::is_nothrow_constructible<V>::value) : has_value(true), storage(v) { }

    template<typename... Args>
    explicit constexpr trivial_result_base(value_t, Args&&... args) noexcept(std::is_nothrow_constructible<V, Args&&...>::value) : has_value(true),  storage(v, std::forward<Args>(args)...) { }

    template<typename... Args>
    explicit constexpr trivial_result_base(error_t, Args&&... args) noexcept(std::is_nothrow_constructible<E, Args&&...>::value) : has_value(false), storage(e, std::forward<Args>(args)...) { }

    trivial_result_base(const trivial_result_base&) = delete;

    constexpr trivial_result_base(trivial_result_base&& rhs) noexcept(std::is_nothrow_move_constructible<V>::value && std::is_nothrow_move_constructible<E>::value) : has_value(rhs.has_value), storage()
    {
        if ( rhs.has_value ) { ::new(value_ptr()) V(rhs.storage.value); }
        else                 { ::new(error_ptr()) E(rhs.storage.error); }
    }

    ~trivial_result_base() noexcept = default;

    constexpr const V* value_ptr() const noexcept { return std::addressof(storage.value); }
    constexpr V*       value_ptr() noexcept       { return std::addressof(storage.value); }
    constexpr const E* error_ptr() const noexcept { return std::addressof(storage.error); }
    constexpr E*       error_ptr() noexcept       { return std::addressof(storage.error); }
};

//---------------------------------------------------------------------------//

template<typename T>
struct tapetums::detail::result::trivial_result_base<T, T>
{
    bool has_value;
    trivial_storage<T, T> storage;

    constexpr trivial_result_base() noexcept(std::is_nothrow_constructible<T>::value) : has_value(true), storage(v) { }

    template<typename... Args>
    explicit constexpr trivial_result_base(value_t, Args&&... args) noexcept(std::is_nothrow_constructible<T, Args&&...>::value) : has_value(true),  storage(v, std::forward<Args>(args)...) { }

    template<typename... Args>
    explicit constexpr trivial_result_base(error_t, Args&&... args) noexcept(std::is_nothrow_constructible<T, Args&&...>::value) : has_value(false), storage(e, std::forward<Args>(args)...) { }

    trivial_result_base(const trivial_result_base&) = delete;

    constexpr trivial_result_base(trivial_result_base&& rhs) noexcept(std::is_nothrow_move_constructible<T>::value) : has_value(rhs.has_value), storage()
    {
        if ( rhs.has_value ) { ::new(value_ptr()) T(rhs.storage.value); }
        else                 { ::new(error_ptr()) T(rhs.storage.error); }
    }

    ~trivial_result_base() noexcept = default;

    constexpr const T* value_ptr() const noexcept { return std::addressof(storage.value); }
    constexpr T*       value_ptr() noexcept       { return std::addressof(storage.value); }
    constexpr const T* error_ptr() const noexcept { return std::addressof(storage.error); }
    constexpr T*       error_ptr() noexcept       { return std::addressof(storage.error); }
};

//---------------------------------------------------------------------------//

template<typename V>
struct tapetums::detail::result::trivial_result_base<V, void>
{
    bool has_value;
    trivial_storage<V, void> storage;

    constexpr trivial_result_base() noexcept = delete;

    template<typename... Args>
    explicit constexpr trivial_result_base(value_t, Args&&... args) noexcept(std::is_nothrow_constructible<V, Args&&...>::value) : has_value(true), storage(v, std::forward<Args>(args)...) { }

    constexpr trivial_result_base(error_t) noexcept : has_value(false), storage(e) { }

    trivial_result_base(const trivial_result_base&) = delete;

    constexpr trivial_result_base(trivial_result_base&& rhs) noexcept(std::is_nothrow_move_constructible<V>::value) : has_value(rhs.has_value), storage()
    {
        if ( rhs.has_value ) { ::new(value_ptr()) V(rhs.storage.value); }
    }

    ~trivial_result_base() noexcept = default;

    constexpr const V* value_ptr() const noexcept { return std::addressof(storage.value); }
    constexpr V*       value_ptr() noexcept       { return std::addressof(storage.value); }
};

//---------------------------------------------------------------------------//

template<typename E>
struct tapetums::detail::result::trivial_result_base<void, E>
{
    bool has_value;
    trivial_storage<void, E> storage;

    constexpr trivial_result_base() noexcept = delete;

    constexpr trivial_result_base(value_t) noexcept : has_value(true), storage(v) { }

    template<typename... Args>
    explicit constexpr trivial_result_base(error_t, Args&&... args) noexcept(std::is_nothrow_constructible<E, Args&&...>::value) : has_value(false), storage(e, std::forward<Args>(args)...) { }

    trivial_result_base(const trivial_result_base&) = delete;

    constexpr trivial_result_base(trivial_result_base&& rhs) noexcept(std::is_nothrow_move_constructible<E>::value) : has_value(rhs.has_value), storage()
    {
        if ( ! rhs.has_value ) { ::new(error_ptr()) E(rhs.storage.error); }
    }

    ~trivial_result_base() noexcept = default;

    constexpr const E* error_ptr() const noexcept { return std::addressof(storage.error); }
    constexpr E*       error_ptr() noexcept       { return std::addressof(storage.error); }
};

//---------------------------------------------------------------------------//

template<>
struct tapetums::detail::result::trivial_result_base<void, void>
{
    bool has_value;
    trivial_storage<void, void> storage;

    constexpr trivial_result_base() noexcept = delete;

    constexpr trivial_result_base(value_t) noexcept : has_value(true),  storage(v) { }

    constexpr trivial_result_base(error_t) noexcept : has_value(false), storage(e) { }

    trivial_result_base(const trivial_result_base&) = delete;

    constexpr trivial_result_base(trivial_result_base&& rhs) noexcept : has_value(rhs.has_value), storage() { }

    ~trivial_result_base() noexcept = default;
};

//***************************************************************************//

template<typename V, typename E>
struct tapetums::detail::result::no_trivial_result_base
{
    bool has_value;
    no_trivial_storage<V, E> storage;

    constexpr no_trivial_result_base() noexcept(std::is_nothrow_constructible<V>::value) : has_value(true), storage(v) { }

    template<typename... Args>
    explicit constexpr no_trivial_result_base(value_t, Args&&... args) noexcept(std::is_nothrow_constructible<V, Args&&...>::value) : has_value(true),  storage(v, std::forward<Args>(args)...) { }

    template<typename... Args>
    explicit constexpr no_trivial_result_base(error_t, Args&&... args) noexcept(std::is_nothrow_constructible<E, Args&&...>::value) : has_value(false), storage(e, std::forward<Args>(args)...) { }

    no_trivial_result_base(const no_trivial_result_base&) = delete;

    constexpr no_trivial_result_base(no_trivial_result_base&& rhs) noexcept(std::is_nothrow_move_constructible<V>::value && std::is_nothrow_move_constructible<E>::value) : has_value(rhs.has_value), storage()
    {
        if ( rhs.has_value ) { ::new(value_ptr()) V(rhs.storage.value); }
        else                 { ::new(error_ptr()) E(rhs.storage.error); }
    }

    ~no_trivial_result_base() noexcept(std::is_nothrow_destructible<V>::value && std::is_nothrow_destructible<E>::value)
    {
        if ( has_value ) { storage.value.~V(); }
        else             { storage.error.~E(); }
    }

    constexpr const V* value_ptr() const noexcept { return std::addressof(storage.value); }
    constexpr V*       value_ptr() noexcept       { return std::addressof(storage.value); }
    constexpr const E* error_ptr() const noexcept { return std::addressof(storage.error); }
    constexpr E*       error_ptr() noexcept       { return std::addressof(storage.error); }
};

//---------------------------------------------------------------------------//

template<typename T>
struct tapetums::detail::result::no_trivial_result_base<T, T>
{
    bool has_value;
    no_trivial_storage<T, T> storage;

    constexpr no_trivial_result_base() noexcept(std::is_nothrow_constructible<T>::value) : has_value(true), storage(v) { }

    template<typename... Args>
    explicit constexpr no_trivial_result_base(value_t, Args&&... args) noexcept(std::is_nothrow_constructible<T, Args&&...>::value) : has_value(true),  storage(v, std::forward<Args>(args)...) { }

    template<typename... Args>
    explicit constexpr no_trivial_result_base(error_t, Args&&... args) noexcept(std::is_nothrow_constructible<T, Args&&...>::value) : has_value(false), storage(e, std::forward<Args>(args)...) { }

    no_trivial_result_base(const no_trivial_result_base&) = delete;

    constexpr no_trivial_result_base(no_trivial_result_base&& rhs) noexcept(std::is_nothrow_move_constructible<T>::value) : has_value(rhs.has_value), storage()
    {
        if ( rhs.has_value ) { ::new(value_ptr()) T(rhs.storage.value); }
        else                 { ::new(error_ptr()) T(rhs.storage.error); }
    }

    ~no_trivial_result_base() noexcept(std::is_nothrow_destructible<T>::value)
    {
        if ( has_value ) { storage.value.~T(); }
        else             { storage.error.~T(); }
    }

    constexpr const T* value_ptr() const noexcept { return std::addressof(storage.value); }
    constexpr T*       value_ptr() noexcept       { return std::addressof(storage.value); }
    constexpr const T* error_ptr() const noexcept { return std::addressof(storage.error); }
    constexpr T*       error_ptr() noexcept       { return std::addressof(storage.error); }
};

//---------------------------------------------------------------------------//

template<typename V>
struct tapetums::detail::result::no_trivial_result_base<V, void>
{
    bool has_value;
    no_trivial_storage<V, void> storage;

    constexpr no_trivial_result_base() noexcept = delete;

    template<typename... Args>
    explicit constexpr no_trivial_result_base(value_t, Args&&... args) noexcept(std::is_nothrow_constructible<V, Args&&...>::value) : has_value(true), storage(v, std::forward<Args>(args)...) { }

    constexpr no_trivial_result_base(error_t) noexcept : has_value(false), storage(e) { }

    no_trivial_result_base(const no_trivial_result_base&) = delete;

    constexpr no_trivial_result_base(no_trivial_result_base&& rhs) noexcept(std::is_nothrow_move_constructible<V>::value) : has_value(rhs.has_value), storage()
    {
        if ( rhs.has_value ) { ::new(value_ptr()) V(rhs.storage.value); }
    }

    ~no_trivial_result_base() noexcept(std::is_nothrow_destructible<V>::value)
    {
        if ( has_value ) { storage.value.~V(); }
    }

    constexpr const V* value_ptr() const noexcept { return std::addressof(storage.value); }
    constexpr V*       value_ptr() noexcept       { return std::addressof(storage.value); }
};

//---------------------------------------------------------------------------//

template<typename E>
struct tapetums::detail::result::no_trivial_result_base<void, E>
{
    bool has_value;
    no_trivial_storage<void, E> storage;

    constexpr no_trivial_result_base() noexcept = delete;

    constexpr no_trivial_result_base(value_t) noexcept : has_value(true),  storage(v) { }

    template<typename... Args>
    explicit constexpr no_trivial_result_base(error_t, Args&&... args) noexcept(std::is_nothrow_constructible<E, Args&&...>::value) : has_value(false), storage(e, std::forward<Args>(args)...) { }

    no_trivial_result_base(const no_trivial_result_base&) = delete;

    constexpr no_trivial_result_base(no_trivial_result_base&& rhs) noexcept(std::is_nothrow_move_constructible<E>::value) : has_value(rhs.has_value), storage()
    {
        if ( ! rhs.has_value ) { ::new(error_ptr()) E(rhs.storage.error); }
    }

    ~no_trivial_result_base() noexcept(std::is_nothrow_destructible<E>::value)
    {
        if ( ! has_value ) { storage.error.~E(); }
    }

    constexpr const E* error_ptr() const noexcept { return std::addressof(storage.error); }
    constexpr E*       error_ptr() noexcept       { return std::addressof(storage.error); }
};

//---------------------------------------------------------------------------//

template<>
struct tapetums::detail::result::no_trivial_result_base<void, void>
{
    bool has_value;
    no_trivial_storage<void, void> storage;

    constexpr no_trivial_result_base() noexcept = delete;

    constexpr no_trivial_result_base(value_t) noexcept : has_value(true),  storage(v) { }

    constexpr no_trivial_result_base(error_t) noexcept : has_value(false), storage(e) { }

    no_trivial_result_base(const no_trivial_result_base&) = delete;

    constexpr no_trivial_result_base(no_trivial_result_base&& rhs) noexcept : has_value(rhs.has_value), storage() { }

    ~no_trivial_result_base() noexcept = default;
};

//---------------------------------------------------------------------------//
// Condition
//---------------------------------------------------------------------------//

namespace tapetums
{
    namespace detail
    {
        namespace result
        {
            template<typename V, typename E>
            using result_base = std::conditional_t
            <
                (std::is_void<V>::value || std::is_trivially_destructible<V>::value) && (std::is_void<E>::value || std::is_trivially_destructible<E>::value),
                tapetums::detail::result::trivial_result_base<V, E>,
                tapetums::detail::result::no_trivial_result_base<V, E>
            >;
        }
    }
}

//---------------------------------------------------------------------------//
// Classes
//---------------------------------------------------------------------------//

template<typename V, typename E>
class tapetums::result final : private tapetums::detail::result::result_base<V, E>
{
    using base = tapetums::detail::result::result_base<V, E>;

public:
    constexpr result()               noexcept(std::is_nothrow_default_constructible<V>::value) : base() { }
    constexpr result(const V& value) noexcept(std::is_nothrow_copy_constructible<V>::value)    : base(v, value) { }
    constexpr result(V&& value)      noexcept(std::is_nothrow_move_constructible<V>::value)    : base(v, std::move(value)) { }
    constexpr result(const E& error) noexcept(std::is_nothrow_copy_constructible<E>::value)    : base(e, error) { }
    constexpr result(E&& error)      noexcept(std::is_nothrow_move_constructible<E>::value)    : base(e, std::move(error)) { }
    constexpr result(result&& rhs)   noexcept(std::is_nothrow_move_constructible<V>::value && std::is_nothrow_move_constructible<E>::value) : base(static_cast<base&&>(rhs)) { }
    constexpr result(const result&) = delete;
    ~result() noexcept = default;

    template <typename... Args>
    constexpr result(Args... args) noexcept(std::is_nothrow_default_constructible<V>::value) : base(v, std::forward<Args>(args)...) { }

    template <typename... Args>
    constexpr result(value_t, Args... args) noexcept(std::is_nothrow_default_constructible<V>::value) : base(v, std::forward<Args>(args)...) { }

    template <typename... Args>
    constexpr result(error_t, Args... args) noexcept(std::is_nothrow_default_constructible<E>::value) : base(e, std::forward<Args>(args)...) { }

public:
    constexpr result& operator =(const result&) = delete;
    constexpr result& operator =(result&& rhs) noexcept(std::is_nothrow_move_constructible<V>::value && std::is_nothrow_move_constructible<E>::value) { swap(std::move(rhs)); return *this; }

    constexpr bool valid     () const noexcept { return this->has_value; }
    constexpr bool operator !() const noexcept { return ! valid(); }
    explicit constexpr operator bool() const noexcept { return valid(); }

private:
    constexpr void swap(result&& rhs) { std::swap(*this, rhs); }

public:
    constexpr const V* operator ->() const noexcept { return this->value_ptr(); }
    constexpr V*       operator ->()       noexcept { return this->value_ptr(); }

    constexpr const V& operator *() const & noexcept { return this->storage.value; }
    constexpr V&       operator *() &       noexcept { return this->storage.value; }
    constexpr V&&      operator *() &&      noexcept { return std::move(this->storage.value); }

    constexpr const V& value() const & noexcept { return this->storage.value; }
    constexpr V&       value() &       noexcept { return this->storage.value; }
    constexpr V&&      value() &&      noexcept { return std::move(this->storage.value); }

    constexpr const E& error() const & noexcept { return this->storage.error; }
    constexpr E&       error() &       noexcept { return this->storage.error; }
    constexpr E&&      error() &&      noexcept { return std::move(this->storage.error); }
};

//---------------------------------------------------------------------------//

template<typename T>
class tapetums::result<T, T> final : private tapetums::detail::result::result_base<T, T>
{
    using base = tapetums::detail::result::result_base<T, T>;

public:
    constexpr result()               noexcept(std::is_nothrow_default_constructible<T>::value) : base() { }
    constexpr result(const T& value) noexcept(std::is_nothrow_copy_constructible<T>::value)    : base(v, value) { }
    constexpr result(T&& value)      noexcept(std::is_nothrow_move_constructible<T>::value)    : base(v, std::move(value)) { }
    constexpr result(result&& rhs)   noexcept(std::is_nothrow_move_constructible<T>::value)    : base(static_cast<base&&>(rhs)) { }
    constexpr result(const result&) = delete;
    ~result() noexcept = default;

    template <typename... Args>
    constexpr result(Args... args) noexcept(std::is_nothrow_default_constructible<T>::value) : base(v, std::forward<Args>(args)...) { }

    template <typename... Args>
    constexpr result(value_t, Args... args) noexcept(std::is_nothrow_default_constructible<T>::value) : base(v, std::forward<Args>(args)...) { }

    template <typename... Args>
    constexpr result(error_t, Args... args) noexcept(std::is_nothrow_default_constructible<T>::value) : base(e, std::forward<Args>(args)...) { }

public:
    constexpr result& operator =(const result&) = delete;
    constexpr result& operator =(result&& rhs) noexcept(std::is_nothrow_move_constructible<T>::value) { swap(std::move(rhs)); return *this; }

    constexpr bool valid     () const noexcept { return this->has_value; }
    constexpr bool operator !() const noexcept { return ! valid(); }
    explicit constexpr operator bool() const noexcept { return valid(); }

private:
    constexpr void swap(result&& rhs) { std::swap(*this, rhs); }

public:
    constexpr const T* operator ->() const noexcept { return this->value_ptr(); }
    constexpr T*       operator ->()       noexcept { return this->value_ptr(); }

    constexpr const T& operator *() const & noexcept { return this->storage.value; }
    constexpr T&       operator *() &       noexcept { return this->storage.value; }
    constexpr T&&      operator *() &&      noexcept { return std::move(this->storage.value); }

    constexpr const T& value() const & noexcept { return this->storage.value; }
    constexpr T&       value() &       noexcept { return this->storage.value; }
    constexpr T&&      value() &&      noexcept { return std::move(this->storage.value); }

    constexpr const T& error() const & noexcept { return this->storage.error; }
    constexpr T&       error() &       noexcept { return this->storage.error; }
    constexpr T&&      error() &&      noexcept { return std::move(this->storage.error); }
};

//---------------------------------------------------------------------------//

template<typename V>
class tapetums::result<V, void> final : private tapetums::detail::result::result_base<V, void>
{
    using base = tapetums::detail::result::result_base<V, void>;

public:
    constexpr result()               noexcept : base(e) { }
    constexpr result(const V& value) noexcept(std::is_nothrow_copy_constructible<V>::value) : base(v, value) { }
    constexpr result(V&& value)      noexcept(std::is_nothrow_move_constructible<V>::value) : base(v, std::move(value)) { }
    constexpr result(result&& rhs)   noexcept(std::is_nothrow_move_constructible<V>::value) : base(static_cast<base&&>(rhs)) { }
    constexpr result(const result&) = delete;
    ~result() noexcept = default;

    template <typename... Args>
    constexpr result(Args... args) noexcept(std::is_nothrow_default_constructible<V>::value) : base(v, std::forward<Args>(args)...) { }

    template <typename... Args>
    constexpr result(value_t, Args... args) noexcept(std::is_nothrow_default_constructible<V>::value) : base(v, std::forward<Args>(args)...) { }

    constexpr result(error_t) noexcept : base(e) { }

public:
    constexpr result& operator =(const result&) = delete;
    constexpr result& operator =(result&& rhs) noexcept(std::is_nothrow_move_constructible<V>::value) { swap(std::move(rhs)); return *this; }

    constexpr bool valid     () const noexcept { return this->has_value; }
    constexpr bool operator !() const noexcept { return ! valid(); }
    explicit constexpr operator bool() const noexcept { return valid(); }

private:
    constexpr void swap(result&& rhs) { std::swap(*this, rhs); }

public:
    constexpr const V* operator ->() const noexcept { return this->value_ptr(); }
    constexpr V*       operator ->()       noexcept { return this->value_ptr(); }

    constexpr const V& operator *() const & noexcept { return this->storage.value; }
    constexpr V&       operator *() &       noexcept { return this->storage.value; }
    constexpr V&&      operator *() &&      noexcept { return std::move(this->storage.value); }

    constexpr const V& value() const & noexcept { return this->storage.value; }
    constexpr V&       value() &       noexcept { return this->storage.value; }
    constexpr V&&      value() &&      noexcept { return std::move(this->storage.value); }
};

//---------------------------------------------------------------------------//

template<typename E>
class tapetums::result<void, E> final : private tapetums::detail::result::result_base<void, E>
{
    using base = tapetums::detail::result::result_base<void, E>;

public:
    constexpr result()               noexcept : base(v) { }
    constexpr result(const E& error) noexcept(std::is_nothrow_copy_constructible<E>::value) : base(e, error) { }
    constexpr result(E&& error)      noexcept(std::is_nothrow_move_constructible<E>::value) : base(e, std::move(error)) { }
    constexpr result(result&& rhs)   noexcept(std::is_nothrow_move_constructible<E>::value) : base(static_cast<base&&>(rhs)) { }
    constexpr result(const result&) = delete;
    ~result() noexcept = default;

    template <typename... Args>
    constexpr result(Args... args) noexcept(std::is_nothrow_default_constructible<E>::value) : base(e, std::forward<Args>(args)...) { }

    constexpr result(value_t) noexcept : base(v) { }

    template <typename... Args>
    constexpr result(error_t, Args... args) noexcept(std::is_nothrow_default_constructible<E>::value) : base(e, std::forward<Args>(args)...) { }

public:
    constexpr result& operator =(const result&) = delete;
    constexpr result& operator =(result&& rhs) noexcept(std::is_nothrow_move_constructible<E>::value) { swap(std::move(rhs)); return *this; }

    constexpr bool valid     () const noexcept { return this->has_value; }
    constexpr bool operator !() const noexcept { return ! valid(); }
    explicit constexpr operator bool() const noexcept { return valid(); }

private:
    constexpr void swap(result&& rhs) { std::swap(*this, rhs); }

public:
    constexpr const E& error() const & noexcept { return this->storage.error; }
    constexpr E&       error() &       noexcept { return this->storage.error; }
    constexpr E&&      error() &&      noexcept { return std::move(this->storage.error); }
};

//---------------------------------------------------------------------------//

template<typename V, typename E>
class tapetums::result<V*, E> final : private tapetums::detail::result::result_base<V*, E>
{
    using base = tapetums::detail::result::result_base<V*, E>;

public:
    constexpr result()               noexcept : base() { }
    constexpr result(V* value)       noexcept : base(v, value) { }
    constexpr result(const E& error) noexcept(std::is_nothrow_copy_constructible<E>::value) : base(e, error) { }
    constexpr result(E&& error)      noexcept(std::is_nothrow_move_constructible<E>::value) : base(e, std::move(error)) { }
    constexpr result(result&& rhs)   noexcept(std::is_nothrow_move_constructible<E>::value) : base(static_cast<base&&>(rhs)) { }
    constexpr result(const result&) = delete;
    ~result() noexcept = default;

    template <typename... Args>
    constexpr result(Args... args) noexcept(std::is_nothrow_default_constructible<E>::value) : base(e, std::forward<Args>(args)...) { }

    constexpr result(value_t, V* value) noexcept : base(v, value) { }

    template <typename... Args>
    constexpr result(error_t, Args... args) noexcept(std::is_nothrow_default_constructible<E>::value) : base(e, std::forward<Args>(args)...) { }

public:
    constexpr result& operator =(const result&) = delete;
    constexpr result& operator =(result&& rhs) noexcept(std::is_nothrow_move_constructible<E>::value) { swap(std::move(rhs)); return *this; }

    constexpr bool valid     () const noexcept { return this->has_value; }
    constexpr bool operator !() const noexcept { return ! valid(); }
    explicit constexpr operator bool() const noexcept { return valid(); }

private:
    constexpr void swap(result&& rhs) { std::swap(*this, rhs); }

public:
    constexpr const V* operator ->() const noexcept { return this->storage.value; }
    constexpr V*       operator ->()       noexcept { return this->storage.value; }

    constexpr const V* operator *() const noexcept { return this->storage.value; }
    constexpr V*       operator *()       noexcept { return this->storage.value; }

    constexpr const V* value() const noexcept { return this->storage.value; }
    constexpr V*       value()       noexcept { return this->storage.value; }

    constexpr const E& error() const & noexcept { return this->storage.error; }
    constexpr E&       error() &       noexcept { return this->storage.error; }
    constexpr E&&      error() &&      noexcept { return std::move(this->storage.error); }
};

//---------------------------------------------------------------------------//

template<typename V, typename E>
class tapetums::result<V, E*> final : private tapetums::detail::result::result_base<V, E*>
{
    using base = tapetums::detail::result::result_base<V, E*>;

public:
    constexpr result()               noexcept(std::is_nothrow_default_constructible<V>::value) : base() { }
    constexpr result(const V& value) noexcept(std::is_nothrow_copy_constructible<V>::value)    : base(v, value) { }
    constexpr result(V&& value)      noexcept(std::is_nothrow_move_constructible<V>::value)    : base(v, std::move(value)) { }
    constexpr result(E* error)       noexcept                                                  : base(e, error) { }
    constexpr result(result&& rhs)   noexcept(std::is_nothrow_move_constructible<V>::value)    : base(static_cast<base&&>(rhs)) { }
    constexpr result(const result&) = delete;
    ~result() noexcept = default;

    constexpr result(value_t) noexcept(std::is_nothrow_copy_constructible<V>::value) : base(v) { }
    constexpr result(error_t) noexcept                                               : base(e) { }

    template <typename... Args>
    constexpr result(Args... args) noexcept(std::is_nothrow_default_constructible<V>::value) : base(v, std::forward<Args>(args)...) { }

    template <typename... Args>
    constexpr result(value_t, Args... args) noexcept(std::is_nothrow_default_constructible<V>::value) : base(v, std::forward<Args>(args)...) { }

    constexpr result(error_t, E* error) noexcept : base(e, error) { }

public:
    constexpr result& operator =(const result&) = delete;
    constexpr result& operator =(result&& rhs) noexcept(std::is_nothrow_move_constructible<V>::value && std::is_nothrow_move_constructible<E>::value) { swap(std::move(rhs)); return *this; }

    constexpr bool valid     () const noexcept { return this->has_value; }
    constexpr bool operator !() const noexcept { return ! valid(); }
    explicit constexpr operator bool() const noexcept { return valid(); }

private:
    constexpr void swap(result&& rhs) { std::swap(*this, rhs); }

public:
    constexpr const V* operator ->() const noexcept { return this->value_ptr(); }
    constexpr V*       operator ->()       noexcept { return this->value_ptr(); }

    constexpr const V& operator *() const & noexcept { return this->storage.value; }
    constexpr V&       operator *() &       noexcept { return this->storage.value; }
    constexpr V&&      operator *() &&      noexcept { return std::move(this->storage.value); }

    constexpr const V& value() const & noexcept { return this->storage.value; }
    constexpr V&       value() &       noexcept { return this->storage.value; }
    constexpr V&&      value() &&      noexcept { return std::move(this->storage.value); }

    constexpr const E* error() const noexcept { return this->storage.error; }
    constexpr E*       error()       noexcept { return this->storage.error; }
};

//---------------------------------------------------------------------------//

template<typename V, typename E>
class tapetums::result<V*, E*> final : private tapetums::detail::result::result_base<V*, E*>
{
    using base = tapetums::detail::result::result_base<V*, E*>;

public:
    constexpr result()             noexcept : base() { }
    constexpr result(V* value)     noexcept : base(v, value) { }
    constexpr result(E* error)     noexcept : base(e, error) { }
    constexpr result(result&& rhs) noexcept : base(static_cast<base&&>(rhs)) { }
    constexpr result(const result&) = delete;
    ~result() noexcept = default;

    constexpr result(value_t, V* value) noexcept : base(v, value) { }

    constexpr result(error_t, E* error) noexcept : base(e, error) { }

public:
    constexpr result& operator =(const result&) = delete;
    constexpr result& operator =(result&& rhs) noexcept { swap(std::move(rhs)); return *this; }

    constexpr bool valid     () const noexcept { return this->has_value; }
    constexpr bool operator !() const noexcept { return ! valid(); }
    explicit constexpr operator bool() const noexcept { return valid(); }

private:
    constexpr void swap(result&& rhs) { std::swap(*this, rhs); }

public:
    constexpr const V* operator ->() const noexcept { return this->storage.value; }
    constexpr V*       operator ->()       noexcept { return this->storage.value; }

    constexpr const V* operator *() const noexcept { return this->storage.value; }
    constexpr V*       operator *()       noexcept { return this->storage.value; }

    constexpr const V* value() const noexcept { return this->storage.value; }
    constexpr V*       value()       noexcept { return this->storage.value; }

    constexpr const E* error() const noexcept { return this->storage.error; }
    constexpr E*       error()       noexcept { return this->storage.error; }
};

//---------------------------------------------------------------------------//

template<typename T>
class tapetums::result<T*, T*> final : private tapetums::detail::result::result_base<T*, T*>
{
    using base = tapetums::detail::result::result_base<T*, T*>;

public:
    constexpr result()             noexcept : base() { }
    constexpr result(T* value)     noexcept : base(v, value) { }
    constexpr result(result&& rhs) noexcept : base(static_cast<base&&>(rhs)) { }
    constexpr result(const result&) = delete;
    ~result() noexcept = default;

    constexpr result(value_t, T* value) noexcept : base(v, value) { }

    constexpr result(error_t, T* error) noexcept : base(e, error) { }

public:
    constexpr result& operator =(const result&) = delete;
    constexpr result& operator =(result&& rhs) noexcept { swap(std::move(rhs)); return *this; }

    constexpr bool valid     () const noexcept { return this->has_value; }
    constexpr bool operator !() const noexcept { return ! valid(); }
    explicit constexpr operator bool() const noexcept { return valid(); }

private:
    constexpr void swap(result&& rhs) { std::swap(*this, rhs); }

public:
    constexpr const T* operator ->() const noexcept { return this->storage.value; }
    constexpr T*       operator ->()       noexcept { return this->storage.value; }

    constexpr const T* operator *() const noexcept { return this->storage.value; }
    constexpr T*       operator *()       noexcept { return this->storage.value; }

    constexpr const T* value() const noexcept { return this->storage.value; }
    constexpr T*       value()       noexcept { return this->storage.value; }

    constexpr const T* error() const noexcept { return this->storage.error; }
    constexpr T*       error()       noexcept { return this->storage.error; }
};

//---------------------------------------------------------------------------//

template<typename V>
class tapetums::result<V*, void> final : private tapetums::detail::result::result_base<V*, void>
{
    using base = tapetums::detail::result::result_base<V*, void>;

public:
    constexpr result()             noexcept : base(e) { }
    constexpr result(V* value)     noexcept : base(v, value) { }
    constexpr result(result&& rhs) noexcept: base(static_cast<base&&>(rhs)) { }
    constexpr result(const result&) = delete;
    ~result() noexcept = default;

    constexpr result(value_t, V* value) noexcept : base(v, value) { }

    constexpr result(error_t) noexcept : base(e) { }

public:
    constexpr result& operator =(const result&) = delete;
    constexpr result& operator =(result&& rhs) noexcept { swap(std::move(rhs)); return *this; }

    constexpr bool valid     () const noexcept { return this->has_value; }
    constexpr bool operator !() const noexcept { return ! valid(); }
    explicit constexpr operator bool() const noexcept { return valid(); }

private:
    constexpr void swap(result&& rhs) { std::swap(*this, rhs); }

public:
    constexpr const V* operator ->() const noexcept { return this->storage.value; }
    constexpr V*       operator ->()       noexcept { return this->storage.value; }

    constexpr const V* operator *() const noexcept { return this->storage.value; }
    constexpr V*       operator *()       noexcept { return this->storage.value; }

    constexpr const V* value() const noexcept { return this->storage.value; }
    constexpr V*       value()       noexcept { return this->storage.value; }
};

//---------------------------------------------------------------------------//

template<typename E>
class tapetums::result<void, E*> final : private tapetums::detail::result::result_base<void, E*>
{
    using base = tapetums::detail::result::result_base<void, E*>;

public:
    constexpr result()             noexcept : base(v) { }
    constexpr result(E* error)     noexcept : base(e, error) { }
    constexpr result(result&& rhs) noexcept : base(static_cast<base&&>(rhs)) { }
    constexpr result(const result&) = delete;
    ~result() noexcept = default;

    constexpr result(value_t) noexcept : base(v) { }

    constexpr result(error_t, E* error) noexcept : base(e, error) { }

public:
    constexpr result& operator =(const result&) = delete;
    constexpr result& operator =(result&& rhs) noexcept { swap(std::move(rhs)); return *this; }

    constexpr bool valid     () const noexcept { return this->has_value; }
    constexpr bool operator !() const noexcept { return ! valid(); }
    explicit constexpr operator bool() const noexcept { return valid(); }

private:
    constexpr void swap(result&& rhs) { std::swap(*this, rhs); }

public:
    constexpr const E* error() const noexcept { return this->storage.error; }
    constexpr E*       error()       noexcept { return this->storage.error; }
};

//---------------------------------------------------------------------------//

template<>
class tapetums::result<void, void> final : private tapetums::detail::result::result_base<void, void>
{
    using base = tapetums::detail::result::result_base<void, void>;

public:
    constexpr result()             noexcept : base(v) { }
    constexpr result(result&& rhs) noexcept : base(static_cast<base&&>(rhs)) { }
    constexpr result(const result&) = delete;
    ~result() noexcept = default;

    constexpr result(value_t) noexcept : base(v) { }
    constexpr result(error_t) noexcept : base(e) { }

public:
    constexpr result& operator =(const result&) = delete;
    constexpr result& operator =(result&& rhs) noexcept { swap(std::move(rhs)); return *this; }

    constexpr bool valid     () const noexcept { return this->has_value; }
    constexpr bool operator !() const noexcept { return ! valid(); }
    explicit constexpr operator bool() const noexcept { return valid(); }

private:
    constexpr void swap(result&& rhs) { this->has_value = rhs.has_value; }
};

//---------------------------------------------------------------------------//

// result14.hpp
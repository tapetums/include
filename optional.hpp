#pragma once

//---------------------------------------------------------------------------//
//
// optional.hpp
// 「値を持たないかもしれない」状態を表現するオブジェクト (in C++14)
//   Copyright (C) 2015-2016 tapetums
//    2015.11.03 created by tapetums
//
//---------------------------------------------------------------------------//

#include <cassert>

#include <utility>

//---------------------------------------------------------------------------//

namespace tapetums {

//---------------------------------------------------------------------------//
// nullopt_t
//---------------------------------------------------------------------------//

struct nullopt_t
{
    constexpr explicit nullopt_t(int) noexcept { }
};

//---------------------------------------------------------------------------//

constexpr nullopt_t nullopt { 0 };

//---------------------------------------------------------------------------//
// optional
//---------------------------------------------------------------------------//

template<typename T>
class optional
{
    static_assert( !std::is_same<typename std::decay<T>::type, nullopt_t>::value, "bad T" );

private:
    bool engaged_;
    T    value_;

public:
    constexpr optional() noexcept : engaged_{ false }, value_{ } { }
    constexpr optional(const optional&)     = default;
    constexpr optional(optional&&) noexcept = default;
    ~optional()                             = default;

    constexpr explicit optional(const T& v) : engaged_{ true }, value_{ v } { }
    constexpr explicit optional(T&& v) noexcept : engaged_{ true }, value_{ std::move(v) } { }
    constexpr explicit optional(nullopt_t) noexcept : optional() { }

    optional& operator =(const T& lhs)
    {
        if ( lhs.initialized() )
        {
            value_   = lhs.value_;
            engaged_ = true;
        }
        else
        {
            engaged_ = false;
        }
        return *this;
    }

    optional& operator =(T&& rhs) noexcept
    {
        if ( rhs.initialized() )
        {
            value_ = std::move(value);
            engaged_ = true;
        }
        else
        {
            engaged_ = false;
        }
        return *this;
    }

    optional& operator =(nullopt_t) noexcept
    {
        clear();
        return *this;
    }

public:
    constexpr void clear() noexcept
    {
        if ( initialized() ) { engaged_ = false; }
    }

public:
    constexpr bool initialized() const noexcept { return engaged_; }

    constexpr explicit operator bool() const noexcept { return engaged_; }

    constexpr T const* operator ->() const
    {
        assert(initialized());
        return std::addressof(value_);
    }

    constexpr T* operator ->()
    {
        assert(initialized());
        return std::addressof(value_);
    }

    constexpr T const& operator *() const
    {
        assert(initialized());
        return value_;
    }

    constexpr T& operator *()
    {
        assert(initialized());
        return value_;
    }

public:
    constexpr T const& value() const
    {
        assert(initialized());
        return value_;
    }

    constexpr T& value()
    {
        assert(initialized());
        return value_;
    }
};

//---------------------------------------------------------------------------//

template<typename T>
class optional<T&>
{
    static_assert( !std::is_same<T, nullopt_t>::value, "bad T" );

private:
    T* ref;

public:
    constexpr optional() noexcept : ref { nullptr } { }
    constexpr optional(const optional& lhs) noexcept : ref { lhs.ref } { }
    constexpr optional(optional&& rhs) noexcept : ref { rhs.ref } { rhs.ref = nullptr; }
    ~optional() = default;

    optional(T&&) = delete;
    constexpr explicit optional(T& v) noexcept : ref { std::addressof(v) } { }
    constexpr explicit optional(nullopt_t) noexcept : ref { nullptr } { }

    optional& operator =(T& v) noexcept
    {
        ref = std::addressof(v);
        return *this;
    }

    optional& operator =(nullopt_t) noexcept
    {
        ref = nullptr;
        return *this;
    }

public:
    constexpr explicit operator bool() const noexcept { return ref != nullptr; }

    constexpr T const* operator ->() const
    {
        assert(ref);
        return ref;
    }

    constexpr T const& operator *() const
    {
        assert(ref);
        return *ref;
    }

    constexpr T* operator ->()
    {
        assert(ref);
        return ref;
    }

    constexpr T& operator *()
    {
        assert(ref);
        return *ref;
    }

public:
    constexpr T const& value() const
    {
        assert(ref);
        return *ref;
    }

    constexpr T& value()
    {
        assert(ref);
        return *ref;
    }
};

//---------------------------------------------------------------------------//

template<typename T>
class optional<T&&>
{
    static_assert(sizeof(T) == 0, "optional<T&&> is disallowed");
};

//---------------------------------------------------------------------------//
// operators
//---------------------------------------------------------------------------//

template<typename T>
constexpr bool operator ==(const optional<T>& x, const optional<T>& y) noexcept
{
    return bool(x) != bool(y) ? false : bool(x) == false ? true : *x == *y;
}

//---------------------------------------------------------------------------//

template<typename T>
constexpr bool operator !=(const optional<T>& x, const optional<T>& y) noexcept
{
    return !(x == y);
}

//---------------------------------------------------------------------------//

template<typename T>
constexpr bool operator ==(const optional<T>& x, nullopt_t) noexcept
{
    return !bool(x);
}

//---------------------------------------------------------------------------//

template<typename T>
constexpr bool operator !=(const optional<T>& x, nullopt_t) noexcept
{
    return bool(x);
}

//---------------------------------------------------------------------------//

template<typename T>
constexpr bool operator ==(nullopt_t, const optional<T>& x) noexcept
{
    return !bool(x);
}

//---------------------------------------------------------------------------//

template<typename T>
constexpr bool operator !=(nullopt_t, const optional<T>& x) noexcept
{
    return bool(x);
}

//---------------------------------------------------------------------------//

} // namespace tapetums

//---------------------------------------------------------------------------//

// optional.hpp
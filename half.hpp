#pragma once

//---------------------------------------------------------------------------//
//
// halp.hpp
//  ヘッダファイルだけで使える半精度浮動小数点数 (C++14)
//  Portable implementation of IEEE 754 half-precision floating-point format
//   Copyright (C) tapetums 2015-2017
//
//---------------------------------------------------------------------------//
//
// Copyright (c) 2006, Industrial Light & Magic, a division of Lucasfilm
// Entertainment Company Ltd.  Portions contributed and copyright held by
// others as indicated.  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above
//      copyright notice, this list of conditions and the following
//      disclaimer.
//
//    * Redistributions in binary form must reproduce the above
//      copyright notice, this list of conditions and the following
//      disclaimer in the documentation and/or other materials provided with
//      the distribution.
//
//    * Neither the name of Industrial Light & Magic nor the names of
//      any other contributors to this software may be used to endorse or
//      promote products derived from this software without specific prior
//      written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
// IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//---------------------------------------------------------------------------//
// Primary authors:
//     Florian Kainz <kainz@ilm.com>
//     Rod Bogart <rgb@ilm.com>
//
// Modification for portable implementation:
//     tapetums <tapetums@live.jp>
//---------------------------------------------------------------------------//

#ifndef _HALF_H_
#define _HALF_H_

#include <cstdint>
#include <cmath>

#include <iostream>
#include <limits>

//---------------------------------------------------------------------------//
// Forward Declarations
//---------------------------------------------------------------------------//

namespace IEEE754
{
    class half;

    using float16_t = half;
    using float32_t = float;
    using float64_t = double;

    inline half operator +(half a, half b);
    inline half operator +(float32_t a, half b);
    inline half operator +(half a, float32_t b);
    inline half operator -(half a, half b);
    inline half operator -(float32_t a, half b);
    inline half operator -(half a, float32_t b);
    inline half operator *(half a, half b);
    inline half operator *(float32_t a, half b);
    inline half operator *(half a, float32_t b);
    inline half operator /(half a, half b);
    inline half operator /(float32_t a, half b);
    inline half operator /(half a, float32_t b);

    inline bool operator ==(half a, half b);
    inline bool operator ==(float32_t a, half b);
    inline bool operator ==(half a, float32_t b);
    inline bool operator !=(half a, half b);
    inline bool operator !=(float32_t a, half b);
    inline bool operator !=(half a, float32_t b);
    inline bool operator <=(half a, half b);
    inline bool operator <=(float32_t a, half b);
    inline bool operator <=(half a, float32_t b);
    inline bool operator >=(half a, half b);
    inline bool operator >=(float32_t a, half b);
    inline bool operator >=(half a, float32_t b);
    inline bool operator < (half a, half b);
    inline bool operator < (float32_t a, half b);
    inline bool operator < (half a, float32_t b);
    inline bool operator > (half a, half b);
    inline bool operator > (float32_t a, half b);
    inline bool operator > (half a, float32_t b);

    template<typename C>
    std::basic_ostream<C>& operator <<(std::basic_ostream<C>& stream, half  lhs);
    template<typename C>
    std::basic_istream<C>& operator >>(std::basic_istream<C>& stream, half& lhs);
}

//---------------------------------------------------------------------------//
// Utility Functions
//---------------------------------------------------------------------------//

namespace 
{
    inline constexpr bool is_zero(IEEE754::float32_t f) noexcept
    {
        return f == +0.0 || f == -0.0;
    }

    inline constexpr bool is_pos_inf(IEEE754::float32_t f) noexcept
    {
        return f == +std::numeric_limits<IEEE754::float32_t>::infinity();
    }

    inline constexpr bool is_neg_inf(IEEE754::float32_t f) noexcept
    {
        return f == -std::numeric_limits<IEEE754::float32_t>::infinity();
    }
}

//---------------------------------------------------------------------------//
// Class
//---------------------------------------------------------------------------//

class IEEE754::half
{
public: // limits
    static constexpr float32_t NRM_MIN = 6.10351562e-05; // Smallest positive normalized half
    static constexpr float32_t MIN     = 5.96046448e-08; // Smallest positive half
    static constexpr float32_t MAX     = 65504.0;        // Largest  positive half
    static constexpr float32_t EPSILON = 0.00097656;     // Smallest positive e for which

    static constexpr auto MANT_DIG   =  11; // Number of digits in mantissa
                                            // (significand + hidden leading 1)
    static constexpr auto DIG        =   2; // Number of base 10 digits that
                                            // can be represented without change
    static constexpr auto RADIX      =   2; // Base of the exponent
    static constexpr auto MIN_EXP    = -13; // Minimum negative integer such that
                                            // HALF::RADIX raised to the power of
                                            // one less than that integer is a
                                            // normalized half
    static constexpr auto MAX_EXP    =  16; // Maximum positive integer such that
                                            // HALF::RADIX raised to the power of
                                            // one less than that integer is a
                                            // normalized half
    static constexpr auto MIN_10_EXP =  -4; // Minimum positive integer such
                                            // that 10 raised to that power is
                                            // a normalized half
    static constexpr auto MAX_10_EXP =   4; // Maximum positive integer such
                                            // that 10 raised to that power is
                                            // a normalized half

private: // types
    union uif
    {
        int32_t   i;
        float32_t f;

        constexpr uif() noexcept : i(0) {}
        constexpr explicit uif(int32_t   i) noexcept : i(i) {}
        constexpr explicit uif(float32_t f) noexcept : f(f) {}
    };

private: // members
    uint16_t data;

public: // ctor / dtor
    constexpr half() noexcept : data(0) {}

    constexpr half(const half&)            noexcept = default;
    constexpr half& operator=(const half&) noexcept = default;

    constexpr half(half&&)            noexcept = default;
    constexpr half& operator=(half&&) noexcept = default;

    explicit half(float32_t f) noexcept { operator =(f); }

    ~half() = default;

public: // operators
    constexpr half& operator +() noexcept       { return *this; }
    constexpr half  operator -() const noexcept { half h; h.data = data ^ 0x8000; return h; }

    constexpr operator float32_t() const noexcept;

    half& operator =(float32_t) noexcept;

    constexpr half& operator +=(half);
    constexpr half& operator +=(float32_t);

    constexpr half& operator -=(half);
    constexpr half& operator -=(float32_t);

    constexpr half& operator *=(half);
    constexpr half& operator *=(float32_t);

    constexpr half& operator /=(half);
    constexpr half& operator /=(float32_t);

public: // methods
    constexpr half round(uint8_t digits) const noexcept;

public: // properties
    constexpr bool is_finite()       const noexcept;
    constexpr bool is_normalized()   const noexcept;
    constexpr bool is_denormalized() const noexcept;
    constexpr bool is_zero()         const noexcept;
    constexpr bool is_negative()     const noexcept;
    constexpr bool is_infinity()     const noexcept;
    constexpr bool is_pos_inf()      const noexcept;
    constexpr bool is_neg_inf()      const noexcept;
    constexpr bool is_NaN()          const noexcept;

public: // constant objects
    static constexpr half infinity() noexcept { half h; h.data = 0x7C00; return h; }
    static constexpr half pos_inf()  noexcept { return +half::infinity(); }
    static constexpr half neg_inf()  noexcept { return -half::infinity(); }
    static constexpr half qNaN()     noexcept { half h; h.data = 0x7FFF; return h; }
    static constexpr half sNaN()     noexcept { half h; h.data = 0x7DFF; return h; }
    static constexpr half NaN()      noexcept { return half::qNaN(); }

public: // accessors
    constexpr uint16_t bits()       const  noexcept { return data; }
    constexpr half&    bits(uint16_t bits) noexcept { data = bits; return *this; }

private: // internal methods
    static uint16_t  convert(int32_t) noexcept;
    static float32_t overflow()       noexcept;
};

//---------------------------------------------------------------------------//
//
// Implementation --
//
// Representation of a float:
//
//    We assume that a float, f, is an IEEE 754 single-precision
//    floating point number, whose bits are arranged as follows:
//
//        31 (msb)
//        | 
//        | 30     23
//        | |      | 
//        | |      | 22                    0 (lsb)
//        | |      | |                     |
//        X XXXXXXXX XXXXXXXXXXXXXXXXXXXXXXX
//
//        s e        m
//
//    S is the sign-bit, e is the exponent and m is the significand.
//
//    If e is between 1 and 254, f is a normalized number:
//
//                s    e-127
//        f = (-1)  * 2      * 1.m
//
//    If e is 0, and m is not zero, f is a denormalized number:
//
//                s    -126
//        f = (-1)  * 2      * 0.m
//
//    If e and m are both zero, f is zero:
//
//        f = 0.0
//
//    If e is 255, f is an "infinity" or "not a number" (NAN),
//    depending on whether m is zero or not.
//
//    Examples:
//
//        0 00000000 00000000000000000000000 = 0.0
//        0 01111110 00000000000000000000000 = 0.5
//        0 01111111 00000000000000000000000 = 1.0
//        0 10000000 00000000000000000000000 = 2.0
//        0 10000000 10000000000000000000000 = 3.0
//        1 10000101 11110000010000000000000 = -124.0625
//        0 11111111 00000000000000000000000 = +infinity
//        1 11111111 00000000000000000000000 = -infinity
//        0 11111111 10000000000000000000000 = NAN
//        1 11111111 11111111111111111111111 = NAN
//
// Representation of a half:
//
//    Here is the bit-layout for a half number, h:
//
//        15 (msb)
//        | 
//        | 14  10
//        | |   |
//        | |   | 9        0 (lsb)
//        | |   | |        |
//        X XXXXX XXXXXXXXXX
//
//        s e     m
//
//    S is the sign-bit, e is the exponent and m is the significand.
//
//    If e is between 1 and 30, h is a normalized number:
//
//                s    e-15
//        h = (-1)  * 2     * 1.m
//
//    If e is 0, and m is not zero, h is a denormalized number:
//
//                S    -14
//        h = (-1)  * 2     * 0.m
//
//    If e and m are both zero, h is zero:
//
//        h = 0.0
//
//    If e is 31, h is an "infinity" or "not a number" (NAN),
//    depending on whether m is zero or not.
//
//    Examples:
//
//        0 00000 0000000000 = 0.0
//        0 01110 0000000000 = 0.5
//        0 01111 0000000000 = 1.0
//        0 10000 0000000000 = 2.0
//        0 10000 1000000000 = 3.0
//        1 10101 1111000001 = -124.0625
//        0 11111 0000000000 = +infinity
//        1 11111 0000000000 = -infinity
//        0 11111 1000000000 = NAN
//        1 11111 1111111111 = NAN
//
// Conversion:
//
//    Converting from a float to a half requires some non-trivial bit
//    manipulations.  In some cases, this makes conversion relatively
//    slow, but the most common case is accelerated via table lookups.
//
//    Converting back from a half to a float is easier because we don't
//    have to do any rounding.  In addition, there are only 65536
//    different half numbers; we can convert each of those numbers once
//    and store the results in a table.  Later, all conversions can be
//    done using only simple table lookups.
//
//  <NOTE>
//    tapetums <tapetums@live.jp> removed table lookup features.
//    This change caused a drop of the speed
//    in exchange for improvement of the portability. 
//
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
// Operators
//---------------------------------------------------------------------------//

inline constexpr IEEE754::half::operator float32_t() const noexcept
{
    if ( data == 0x0000 )
    {
        return 0.0;
    }
    else if ( data == 0x8000 )
    {
        return -0.0;
    }

    const auto s =  (data << 16) & 0x8000'0000;
    const auto e = ((data >> 10) & 0x0000'001F) + (127 - 15);
    const auto m =   data        & 0x0000'03FF;

    uif tmp;
    tmp.i = s | (e << 23) | (m << (23 - 10));
    return tmp.f;
}

//---------------------------------------------------------------------------//

inline IEEE754::half& IEEE754::half::operator =(float32_t f) noexcept
{
    uif tmp { f };

    if ( ::is_zero(f) )
    {
        // Common special case - zero.
        // Preserve the zero's sign bit.
        data = (tmp.i >> 16);
    }
    else
    {
        const auto s =  (tmp.i >> 16) & 0x0000'8000;
        const auto e = ((tmp.i >> 23) & 0x0000'00FF) - (127 - 15);
        const auto m =   tmp.i        & 0x007F'FFFF;

        if ( 0 < e && e < 31 )
        {
            // Simple case - round the significand, m, to 10
            // bits and combine it with the sign and exponent.
            data = s | (e << 10) | (m >> (23 - 10));
        }
        else
        {
            // Difficult case - call a function.
            data = convert(tmp.i); // too small
        }
    }

    return *this;
}

//---------------------------------------------------------------------------//

inline constexpr IEEE754::half& IEEE754::half::operator +=(half h)
{
    //std::cout << "[op+=()]" << '\n';
    if ( is_NaN() )
    {
        // Return NaN.
    }
    else if ( h.is_NaN() )
    {
        data = h.data; // Propagate NaN.
    }
    else if ( is_pos_inf() && h.is_neg_inf() )
    {
        data = NaN().bits(); // +∞ + -∞ : undefined
    }
    else if ( is_neg_inf() && h.is_pos_inf() )
    {
        data = NaN().bits(); // -∞ + +∞ : undefined
    }
    else
    {
        operator=(static_cast<float32_t>(*this) + static_cast<float32_t>(h));
    }
    return *this;
}

//---------------------------------------------------------------------------//

inline constexpr IEEE754::half& IEEE754::half::operator +=(float32_t f)
{
    //std::cout << "[op+=()]" << '\n';
    if ( is_NaN() )
    {
        // Return NaN.
    }
    else if ( ::isnan(f) )
    {
        data = NaN().bits(); // Propagate NaN.
    }
    else if ( is_pos_inf() && ::is_neg_inf(f) )
    {
        data = NaN().bits(); // +∞ + -∞ : undefined
    }
    else if ( is_neg_inf() && ::is_pos_inf(f) )
    {
        data = NaN().bits(); // -∞ + +∞ : undefined
    }
    else
    {
        operator=(static_cast<float32_t>(*this) + f);
    }
    return *this;
}

//---------------------------------------------------------------------------//

inline constexpr IEEE754::half& IEEE754::half::operator -=(half h)
{
    //std::cout << "[op-=()]" << '\n';
    if ( is_NaN() )
    {
        // Return NaN.
    }
    else if ( h.is_NaN() )
    {
        data = h.data; // Propagate NaN.
    }
    else if ( is_pos_inf() && h.is_pos_inf() )
    {
        data = NaN().bits(); // +∞ - +∞ : undefined
    }
    else if ( is_neg_inf() && h.is_neg_inf() )
    {
        data = NaN().bits(); // -∞ - -∞ : undefined
    }
    else
    {
        operator=(static_cast<float32_t>(*this) - static_cast<float32_t>(h));
    }
    return *this;
}

//---------------------------------------------------------------------------//

inline constexpr IEEE754::half& IEEE754::half::operator -=(float32_t f)
{
    //std::cout << "[op-=()]" << '\n';
    if ( is_NaN() )
    {
        // Return NaN.
    }
    else if ( ::isnan(f) )
    {
        data = NaN().bits(); // Propagate NaN.
    }
    else if ( is_pos_inf() && ::is_pos_inf(f) )
    {
        data = NaN().bits(); // +∞ - +∞ : undefined
    }
    else if ( is_neg_inf() && ::is_neg_inf(f) )
    {
        data = NaN().bits(); // -∞ - -∞ : undefined
    }
    else
    {
        operator=(static_cast<float32_t>(*this) - f);
    }
    return *this;
}

//---------------------------------------------------------------------------//

inline constexpr IEEE754::half& IEEE754::half::operator *=(half h)
{
    //std::cout << "[op*=()]" << '\n';
    if ( is_NaN() )
    {
        // Return NaN.
    }
    else if ( h.is_NaN() )
    {
        data = h.data; // Propagate NaN.
    }
    else if ( is_infinity() && h.is_zero() )
    {
        data = NaN().bits(); // ±∞ * ±0 : undefined
    }
    else if ( is_zero() && h.is_infinity() )
    {
        data = NaN().bits(); // ±0 * ±∞ : undefined
    }
    else
    {
        operator=(static_cast<float32_t>(*this) * static_cast<float32_t>(h));
    }
    return *this;
}

//---------------------------------------------------------------------------//

inline constexpr IEEE754::half& IEEE754::half::operator *=(float32_t f)
{
    //std::cout << "[op*=()]" << '\n';
    if ( is_NaN() )
    {
        // Return NaN.
    }
    else if ( ::isnan(f) )
    {
        data = NaN().bits(); // Propagate NaN.
    }
    else if ( is_infinity() && ::is_zero(f) )
    {
        data = NaN().bits(); // ±∞ * ±0 : undefined
    }
    else if ( is_zero() && ::isinf(f) )
    {
        data = NaN().bits(); // ±0 * ±∞ : undefined
    }
    else
    {
        operator=(static_cast<float32_t>(*this) * f);
    }
    return *this;
}

//---------------------------------------------------------------------------//

inline constexpr IEEE754::half& IEEE754::half::operator /=(half h)
{
    //std::cout << "[op/=()]" << '\n';
    if ( is_NaN() )
    {
        // Return NaN.
    }
    else if ( h.is_NaN() )
    {
        data = h.data; // Propagate NaN.
    }
    else if ( is_zero() && h.is_zero() )
    {
        data = NaN().bits(); // ±0 ÷ ±0 : undefined
    }
    else if ( is_infinity() && h.is_infinity() )
    {
        data = NaN().bits(); // ±∞ ÷ ±∞ : undefined
    }
    else
    {
        operator=(static_cast<float32_t>(*this) / static_cast<float32_t>(h));
    }
    return *this;
}

//---------------------------------------------------------------------------//

inline constexpr IEEE754::half& IEEE754::half::operator /=(float32_t f)
{
    //std::cout << "[op/=()]" << '\n';
    if ( is_NaN() )
    {
        // Return NaN.
    }
    else if ( ::isnan(f) )
    {
        data = NaN().bits(); // Propagate NaN.
    }
    else if ( is_zero() && ::is_zero(f) )
    {
        data = NaN().bits(); // ±0 ÷ ±0 : undefined
    }
    else if ( is_infinity() && ::isinf(f) )
    {
        data = NaN().bits(); // ±∞ ÷ ±∞ : undefined
    }
    else
    {
        operator=(static_cast<float32_t>(*this) / f);
    }
    return *this;
}

//---------------------------------------------------------------------------//
// Methods
//---------------------------------------------------------------------------//

//---------------------------------------------------------
//  Round to n-bit precision (n should be between 0 and 10).
//  After rounding, the significand's 10-n least significant
//  bits will be zero.
//---------------------------------------------------------
inline constexpr IEEE754::half IEEE754::half::round(uint8_t n) const noexcept
{
    //std::cout << "[round()]" << '\n';

    // Parameter check.
    if ( n >= 10 ) { return *this; }

    // Disassemble h into the sign, s,
    // and the combined exponent and significand, e.
    auto s = data & 0x8000;
    auto e = data & 0x7FFF;

    // Round the exponent and significand to the nearest value
    // where ones occur only in the (10-n) most significant bits.
    // Note that the exponent adjusts automatically if rounding
    // up causes the significand to overflow.
    e >>= 9 - n;
    e  += e & 1;
    e <<= 9 - n;

    // Check for exponent overflow.
    if ( e >= 0x7C00 )
    {
        // Overflow occurred -- truncate instead of rounding.
        e = data;
        e >>= 10 - n;
        e <<= 10 - n;
    }

    // Put the original sign bit back.
    half h;
    h.data = s | e;
    return h;
}

//---------------------------------------------------------------------------//
// Properties
//---------------------------------------------------------------------------//

inline constexpr bool IEEE754::half::is_finite() const noexcept
{
    const auto e = (data >> 10) & 0x001F;
    return e < 31;
}

//---------------------------------------------------------------------------//

inline constexpr bool IEEE754::half::is_normalized() const noexcept
{
    const auto e = (data >> 10) & 0x001F;
    return e > 0 && e < 31;
}

//---------------------------------------------------------------------------//

inline constexpr bool IEEE754::half::is_denormalized() const noexcept
{
    const auto e = (data >> 10) & 0x001F;
    const auto m =  data & 0x03FF;
    return e == 0 && m != 0;
}

//---------------------------------------------------------------------------//

inline constexpr bool IEEE754::half::is_zero() const noexcept
{
    return (data & 0x7FFF) == 0;
}

//---------------------------------------------------------------------------//

inline constexpr bool IEEE754::half::is_negative() const noexcept
{
    return (data & 0x8000) != 0;
}

//---------------------------------------------------------------------------//

inline constexpr bool IEEE754::half::is_infinity() const noexcept
{
    const auto e = (data >> 10) & 0x001F;
    const auto m =  data & 0x03FF;
    return e == 31 && m == 0;
}

//---------------------------------------------------------------------------//

inline constexpr bool IEEE754::half::is_pos_inf() const noexcept
{
    return data == pos_inf().bits();
}

//---------------------------------------------------------------------------//

inline constexpr bool IEEE754::half::is_neg_inf() const noexcept
{
    return data == neg_inf().bits();
}

//---------------------------------------------------------------------------//

inline constexpr bool IEEE754::half::is_NaN() const noexcept
{
    const auto e = (data >> 10) & 0x001F;
    const auto m =  data & 0x03FF;
    return e == 31 && m != 0;
}

//---------------------------------------------------------------------------//
// Internal Methods
//---------------------------------------------------------------------------//

inline uint16_t IEEE754::half::convert(int32_t i) noexcept
{
    //std::cout << "[convert()]" << '\n';

    auto s =  (i >> 16) & 0x0000'8000;
    auto e = ((i >> 23) & 0x0000'00FF) - (127 - 15);
    auto m =   i        & 0x007F'FFFF;

    // Now reassemble s, e and m into a half:
    if ( e <= 0 )
    {
        if ( e < -10 )
        {
            // E is less than -10.  The absolute value of f is
            // less than HALF::MIN (f may be a small normalized
            // float, a denormalized float or a zero).

            // We convert f to a half zero with the same sign as f.
            return s;
        }

        // E is between -10 and 0.  F is a normalized float
        // whose magnitude is less than HALF::NRM_MIN.

        // We convert f to a denormalized half.

        // Add an explicit leading 1 to the significand.
        m = m | 0x00800000;

        // Round to m to the nearest (10+e)-bit value (with e between
        // -10 and 0); in case of a tie, round to the nearest even value.

        // Rounding may cause the significand to overflow and make
        // our number normalized.  Because of the way a half's bits
        // are laid out, we don't have to treat this case separately;
        // the code below will handle it correctly.

        const auto t = 14 - e;
        const auto a = (1 << (t - 1)) - 1;
        const auto b = (m >> t) & 1;

        m = (m + a + b) >> t;

        // Assemble the half from s, e (zero) and m.
        return s | m;
    }
    else if ( e == 0xFF - (127 - 15) )
    {
        if ( m == 0 )
        {
            // F is an infinity; convert f to a half
            // infinity with the same sign as f.

            return s | 0x7C00;
        }
        else
        {
            // F is a NAN; we produce a half NAN that preserves
            // the sign bit and the 10 leftmost bits of the
            // significand of f, with one exception: If the 10
            // leftmost bits are all zero, the NAN would turn 
            // into an infinity, so we have to set at least one
            // bit in the significand.

            m >>= 13;
            return s | 0x7C00 | m | (m == 0);
        }
    }
    else
    {
        // E is greater than zero.  F is a normalized float.
        // We try to convert f to a normalized half.

        // Round to m to the nearest 10-bit value.  In case of
        // a tie, round to the nearest even value.
        m = m + 0x00000FFF + ((m >> 13) & 1);
        if (m & 0x00800000)
        {
            m =  0; // overflow in significand,
            e += 1; // adjust exponent
        }

        // Handle exponent overflow
        if ( e > 30 )
        {
            overflow();        // Cause a hardware floating point overflow;
            return s | 0x7C00; // if this returns, the half becomes an
        }                      // infinity with the same sign as f.

        // Assemble the half from s, e and m.
        return s | (e << 10) | (m >> (23 - 10));
    }
}

//---------------------------------------------------------------------------//

inline IEEE754::float32_t IEEE754::half::overflow() noexcept
{
    volatile float32_t f { 1e10 };

    for ( auto i = 0; i < 10; ++i )
    {
        f *= f; // this will overflow before the for­loop terminates
    }

    return f;
}

//---------------------------------------------------------------------------//
// Global Operators
//---------------------------------------------------------------------------//

inline IEEE754::half IEEE754::operator +(half a, half b)
{
    //std::cout << "[op+()]" << '\n';
    if ( a.is_NaN() || b.is_NaN() )
    {
        return half::NaN();
    }
    else if ( a.is_pos_inf() && b.is_neg_inf() )
    {
        return half::NaN(); // +∞ + -∞ : undefined
    }
    else if ( a.is_neg_inf() && b.is_pos_inf() )
    {
        return half::NaN(); // -∞ + +∞ : undefined
    }
    else
    {
        return half { static_cast<float32_t>(a) + static_cast<float32_t>(b) };
    }
}

//---------------------------------------------------------------------------//

inline IEEE754::half IEEE754::operator +(float32_t a, half b)
{
    //std::cout << "[op+()]" << '\n';
    if ( ::isnan(a) || b.is_NaN() )
    {
        return half::NaN();
    }
    else if ( is_pos_inf(a) && b.is_neg_inf() )
    {
        return half::NaN(); // +∞ + -∞ : undefined
    }
    else if ( is_neg_inf(a) && b.is_pos_inf() )
    {
        return half::NaN(); // -∞ + +∞ : undefined
    }
    else
    {
        return half { a + static_cast<float32_t>(b) };
    }
}

//---------------------------------------------------------------------------//

inline IEEE754::half IEEE754::operator +(half a, float32_t b)
{
    //std::cout << "[op+()]" << '\n';
    if ( a.is_NaN() || ::isnan(b) )
    {
        return half::NaN();
    }
    else if ( a.is_pos_inf() && is_neg_inf(b) )
    {
        return half::NaN(); // +∞ + -∞ : undefined
    }
    else if ( a.is_neg_inf() && is_pos_inf(b) )
    {
        return half::NaN(); // -∞ + +∞ : undefined
    }
    else
    {
        return half { static_cast<float32_t>(a) + b };
    }
}

//---------------------------------------------------------------------------//

inline IEEE754::half IEEE754::operator -(half a, half b)
{
    //std::cout << "[op-()]" << '\n';
    if ( a.is_NaN() || b.is_NaN() )
    {
        return half::NaN();
    }
    else if ( a.is_pos_inf() && b.is_pos_inf() )
    {
        return half::NaN(); // +∞ - +∞ : undefined
    }
    else if ( a.is_neg_inf() && b.is_neg_inf() )
    {
        return half::NaN(); // -∞ - -∞ : undefined
    }
    else
    {
        return half { static_cast<float32_t>(a) - static_cast<float32_t>(b) };
    }
}

//---------------------------------------------------------------------------//

inline IEEE754::half IEEE754::operator -(float32_t a, half b)
{
    //std::cout << "[op-()]" << '\n';
    if ( ::isnan(a) || b.is_NaN() )
    {
        return half::NaN();
    }
    else if ( is_pos_inf(a) && b.is_pos_inf() )
    {
        return half::NaN(); // +∞ - +∞ : undefined
    }
    else if ( is_neg_inf(a) && b.is_neg_inf() )
    {
        return half::NaN(); // -∞ - -∞ : undefined
    }
    else
    {
        return half { a - static_cast<float32_t>(b) };
    }
}

//---------------------------------------------------------------------------//

inline IEEE754::half IEEE754::operator -(half a, float32_t b)
{
    //std::cout << "[op-()]" << '\n';
    if ( a.is_NaN() || ::isnan(b) )
    {
        return half::NaN();
    }
    else if ( a.is_pos_inf() && is_pos_inf(b) )
    {
        return half::NaN(); // +∞ - +∞ : undefined
    }
    else if ( a.is_neg_inf() && is_neg_inf(b) )
    {
        return half::NaN(); // -∞ - -∞ : undefined
    }
    else
    {
        return half { static_cast<float32_t>(a) - b };
    }
}

//---------------------------------------------------------------------------//

inline IEEE754::half IEEE754::operator *(half a, half b)
{
    //std::cout << "[op*()]" << '\n';
    if ( a.is_NaN() || b.is_NaN() )
    {
        return half::NaN();
    }
    else if ( a.is_infinity() && b.is_zero() )
    {
        return half::NaN(); // ±∞ × ±0 : undefined
    }
    else if ( a.is_zero() && b.is_infinity() )
    {
        return half::NaN(); // ±0 × ±∞ : undefined
    }
    else
    {
        return half { static_cast<float32_t>(a) * static_cast<float32_t>(b) };
    }
}

//---------------------------------------------------------------------------//

inline IEEE754::half IEEE754::operator *(float32_t a, half b)
{
    //std::cout << "[op*()]" << '\n';
    if ( ::isnan(a) || b.is_NaN() )
    {
        return half::NaN();
    }
    else if ( ::isinf(a) && b.is_zero() )
    {
        return half::NaN(); // ±∞ × ±0 : undefined
    }
    else if ( ::is_zero(a) && b.is_infinity() )
    {
        return half::NaN(); // ±0 × ±∞ : undefined
    }
    else
    {
        return half { a * static_cast<float32_t>(b) };
    }
}

//---------------------------------------------------------------------------//

inline IEEE754::half IEEE754::operator *(half a, float32_t b)
{
    //std::cout << "[op*()]" << '\n';
    if ( a.is_NaN() || ::isnan(b) )
    {
        return half::NaN();
    }
    else if ( a.is_infinity() && ::is_zero(b) )
    {
        return half::NaN(); // ±∞ × ±0 : undefined
    }
    else if ( a.is_zero() && ::isinf(b) )
    {
        return half::NaN(); // ±0 × ±∞ : undefined
    }
    else
    {
        return half { static_cast<float32_t>(a) * b };
    }
}

//---------------------------------------------------------------------------//

inline IEEE754::half IEEE754::operator /(half a, half b)
{
    //std::cout << "[op/()]" << '\n';
    if ( a.is_NaN() || b.is_NaN() )
    {
        return half::NaN();
    }
    else if ( a.is_zero() && b.is_zero() )
    {
        return half::NaN(); // ±0 ÷ ±0 : undefined
    }
    else if ( a.is_infinity() && b.is_infinity() )
    {
        return half::NaN(); // ±∞ ÷ ±∞ : undefined
    }
    else
    {
        return half { static_cast<float32_t>(a) / static_cast<float32_t>(b) };
    }
}

//---------------------------------------------------------------------------//

inline IEEE754::half IEEE754::operator /(float32_t a, half b)
{
    //std::cout << "[op/()]" << '\n';
    if ( ::isnan(a) || b.is_NaN() )
    {
        return half::NaN();
    }
    else if ( ::is_zero(a) && b.is_zero() )
    {
        return half::NaN(); // ±0 ÷ ±0 : undefined
    }
    else if ( ::isinf(a) && b.is_infinity() )
    {
        return half::NaN(); // ±∞ ÷ ±∞ : undefined
    }
    else
    {
        return half { a / static_cast<float32_t>(b) };
    }
}

//---------------------------------------------------------------------------//

inline IEEE754::half IEEE754::operator /(half a, float32_t b)
{
    //std::cout << "[op/()]" << '\n';
    if ( a.is_NaN() || ::isnan(b) )
    {
        return half::NaN();
    }
    else if ( a.is_zero() && ::is_zero(b) )
    {
        return half::NaN(); // ±0 ÷ ±0 : undefined
    }
    else if ( a.is_infinity() && ::isinf(b) )
    {
        return half::NaN(); // ±∞ ÷ ±∞ : undefined
    }
    else
    {
        return half { static_cast<float32_t>(a) / b };
    }
}

//---------------------------------------------------------------------------//

inline bool IEEE754::operator ==(half a, half b)
{
    //std::cout << "[op==()]" << '\n';
    if ( a.is_NaN() || b.is_NaN() )
    {
        return false;
    }
    else
    {
        return a.bits() == b.bits();
    }
}

//---------------------------------------------------------------------------//

inline bool IEEE754::operator ==(float32_t a, half b)
{
    //std::cout << "[op==()]" << '\n';
    if ( ::isnan(a) || b.is_NaN() )
    {
        return false;
    }
    else
    {
        return a == static_cast<float32_t>(b);
    }
}

//---------------------------------------------------------------------------//

inline bool IEEE754::operator ==(half a, float32_t b)
{
    //std::cout << "[op==()]" << '\n';
    if ( a.is_NaN() || ::isnan(b) )
    {
        return false;
    }
    else
    {
        return static_cast<float32_t>(a) == b;
    }
}

//---------------------------------------------------------------------------//

inline bool IEEE754::operator !=(half a, half b)
{
    //std::cout << "[op!=()]" << '\n';
    return ! operator ==(a, b);
}

//---------------------------------------------------------------------------//

inline bool IEEE754::operator !=(float32_t a, half b)
{
    //std::cout << "[op!=()]" << '\n';
    return ! operator ==(a, b);
}

//---------------------------------------------------------------------------//

inline bool IEEE754::operator !=(half a, float32_t b)
{
    //std::cout << "[op!=()]" << '\n';
    return ! operator==(a, b);
}

//---------------------------------------------------------------------------//

inline bool IEEE754::operator <(half a, half b)
{
    //std::cout << "[op<()]" << '\n';
    if ( a.is_NaN() || b.is_NaN() )
    {
        return false;
    }
    else
    {
        return static_cast<float32_t>(a) < static_cast<float32_t>(b);
    }
}

//---------------------------------------------------------------------------//

inline bool IEEE754::operator <(float32_t a, half b)
{
    //std::cout << "[op<()]" << '\n';
    if ( ::isnan(a) || b.is_NaN() )
    {
        return false;
    }
    else
    {
        return a < static_cast<float32_t>(b);
    }
}

//---------------------------------------------------------------------------//

inline bool IEEE754::operator <(half a, float32_t b)
{
    //std::cout << "[op<()]" << '\n';
    if ( a.is_NaN() || ::isnan(b) )
    {
        return false;
    }
    else
    {
        return static_cast<float32_t>(a) < b;
    }
}

//---------------------------------------------------------------------------//

inline bool IEEE754::operator >=(half a, half b)
{
    //std::cout << "[op>=()]" << '\n';
    return ! operator <(a, b);
}

//---------------------------------------------------------------------------//

inline bool IEEE754::operator >=(float32_t a, half b)
{
    //std::cout << "[op>=()]" << '\n';
    return ! operator<(a, b);
}

//---------------------------------------------------------------------------//

inline bool IEEE754::operator >=(half a, float32_t b)
{
    //std::cout << "[op>=()]" << '\n';
    return ! operator <(a, b);
}

//---------------------------------------------------------------------------//

inline bool IEEE754::operator >(half a, half b)
{
    //std::cout << "[op>()]" << '\n';
    if ( a.is_NaN() || b.is_NaN() )
    {
        return false;
    }
    else
    {
        return static_cast<float32_t>(a) > static_cast<float32_t>(b);
    }
}

//---------------------------------------------------------------------------//

inline bool IEEE754::operator >(float32_t a, half b)
{
    //std::cout << "[op>()]" << '\n';
    if ( ::isnan(a) || b.is_NaN() )
    {
        return false;
    }
    else
    {
        return a > static_cast<float32_t>(b);
    }
}

//---------------------------------------------------------------------------//

inline bool IEEE754::operator >(half a, float32_t b)
{
    //std::cout << "[op>()]" << '\n';
    if ( a.is_NaN() || ::isnan(b) )
    {
        return false;
    }
    else
    {
        return static_cast<float32_t>(a) > b;
    }
}

//---------------------------------------------------------------------------//

inline bool IEEE754::operator <=(half a, half b)
{
    //std::cout << "[op<=()]" << '\n';
    return ! operator >(a, b);
}

//---------------------------------------------------------------------------//

inline bool IEEE754::operator <=(float32_t a, half b)
{
    //std::cout << "[op<=()]" << '\n';
    return ! operator >(a, b);
}

//---------------------------------------------------------------------------//

inline bool IEEE754::operator <=(half a, float32_t b)
{
    //std::cout << "[op>=()]" << '\n';
    return ! operator >(a, b);
}

//---------------------------------------------------------------------------//
// Stream I/O
//---------------------------------------------------------------------------//

template<typename C>
std::basic_ostream<C>& IEEE754::operator <<
(
    std::basic_ostream<C>& stream, half lhs
)
{
    switch ( lhs.bits() )
    {
        case half::pos_inf().bits():
        {
            stream << "∞";
            break;
        }
        case half::neg_inf().bits():
        {
            stream << "-∞";
            break;
        }
        case half::qNaN().bits():
        {
            stream << "qNaN";
            break;
        }
        case half::sNaN().bits():
        {
            stream << "sNaN";
            break;
        }
        default:
        {
            stream << static_cast<float32_t>(lhs);
            break;
        }
    }
    return stream;
}

//---------------------------------------------------------------------------//

template<typename C>
std::basic_istream<C>& IEEE754::operator >>
(
    std::basic_istream<C>& stream, half& lhs
)
{
    float32_t value;
    stream >> value;
    lhs.operator =(value);
    return stream;
}

//---------------------------------------------------------------------------//

#endif // _HALF_H_

// half.hpp
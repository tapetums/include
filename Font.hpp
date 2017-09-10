#pragma once

//---------------------------------------------------------------------------//
//
// Font.hpp
//  RAII class for Windows fonts
//   Copyright (C) 2014-2017 tapetums
//
//---------------------------------------------------------------------------//

#include <windows.h>

//---------------------------------------------------------------------------//
// Forward Declarations
//---------------------------------------------------------------------------//

namespace tapetums 
{
    class Font;
}

//---------------------------------------------------------------------------//
// Classes
//---------------------------------------------------------------------------//

class tapetums::Font
{
private: // members
    HFONT   m_font   { nullptr };
    INT32   m_size   { 0 };
    LPCTSTR m_name   { nullptr };
    INT32   m_weight { 0 };

public: // ctor / dtor
    Font() = default;
    ~Font() { Free(); }

    Font(const Font& lhs)             { Create(lhs.m_size, lhs.m_name, lhs.m_weight); }
    Font& operator =(const Font& lhs) { Create(lhs.m_size, lhs.m_name, lhs.m_weight); return *this; }

    Font(Font&&)             noexcept = default;
    Font& operator =(Font&&) noexcept = default;

    Font(INT32 size, LPCTSTR name, INT32 weight = FW_REGULAR)
    {
        Create(size, name, weight);
    }

public: // accessors
    HFONT handle() const noexcept { return m_font; }

public: // operators
    operator HFONT() const noexcept { return m_font; }

public: // methods
    HFONT Create
    (
        INT32 size, LPCTSTR name, INT32 weight = FW_REGULAR
    )
    {
        m_font = ::CreateFont
        (
            size, 0, 0, 0,
            weight, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH,
            name
        );

        if ( m_font )
        {
            m_size   = size;
            m_name   = name;
            m_weight = weight;
        }

        return m_font;
    }

    void Free()
    {
        m_weight = 0;
        m_name   = nullptr;
        m_size   = 0;

        if ( m_font )
        {
            ::DeleteObject(m_font);
            m_font = nullptr;
        }
    }
};

//---------------------------------------------------------------------------//

// Font.hpp
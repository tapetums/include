#pragma once

//---------------------------------------------------------------------------//
//
// Font.hpp
//  RAII class for Windows fonts
//   Copyright (C) 2014-2018 tapetums
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

class tapetums::Font final
{
private: // members
    HFONT m_font { nullptr };

public: // ctor / dtor
    Font() = default;
    ~Font() { Free(); }

    Font(const Font& lhs)             = delete;
    Font& operator =(const Font& lhs) = delete;

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
    HFONT Create(INT32 size, LPCSTR  name, INT32 weight = FW_REGULAR);
    HFONT Create(INT32 size, LPCWSTR name, INT32 weight = FW_REGULAR);
    void  Free();
};

//---------------------------------------------------------------------------//
// Font Methods
//---------------------------------------------------------------------------//

inline HFONT tapetums::Font::Create
(
    INT32 size, LPCSTR name, INT32 weight
)
{
    m_font = ::CreateFontA
    (
        size, 0, 0, 0,
        weight, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH,
        name
    );

    return m_font;
}

//---------------------------------------------------------------------------//

inline HFONT tapetums::Font::Create
(
    INT32 size, LPCWSTR name, INT32 weight
)
{
    m_font = ::CreateFontW
    (
        size, 0, 0, 0,
        weight, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH,
        name
    );

    return m_font;
}

//---------------------------------------------------------------------------//

inline void tapetums::Font::Free()
{
    if ( m_font )
    {
        ::DeleteObject(m_font);
        m_font = nullptr;
    }
}

//---------------------------------------------------------------------------//

// Font.hpp
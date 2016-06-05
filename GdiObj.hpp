#pragma once

//---------------------------------------------------------------------------//
//
// GdiObj.hpp
//  GDIオブジェクトの RAII
//   Copyright (C) 2016 tapetums
//
//---------------------------------------------------------------------------//

#include <utility>

#include <windows.h>

//---------------------------------------------------------------------------//
// 前方宣言
//---------------------------------------------------------------------------//

namespace tapetums
{
    class Pen;
};

//---------------------------------------------------------------------------//
// クラス
//---------------------------------------------------------------------------//

class tapetums::Pen
{
public:
    enum Style : INT32
    {
        Solid      = PS_SOLID,
        Dash       = PS_DASH,
        Dot        = PS_DOT,
        DashDot    = PS_DASHDOT,
        DashDotDot = PS_DASHDOTDOT,
        Null       = PS_NULL,
    };

private:
    Style    m_style { Pen::Style::Solid };
    INT32    m_width { 1 };
    COLORREF m_color { 0x00FFFFFF }; // alpha を 0 以外にすると不具合
    HPEN     m_pen   { nullptr };

public:
    Pen() = default;
    ~Pen() { destroy(); }

    Pen(const Pen& lhs)             { copy(lhs); }
    Pen& operator =(const Pen& lhs) { copy(lhs); return *this; }

    Pen(Pen&& rhs)             { swap(std::move(rhs)); }
    Pen& operator =(Pen&& rhs) { swap(std::move(rhs)); return *this; }

    Pen(Style style, INT32 width, COLORREF color) { create(style, width, color); }

public:
    void copy(const Pen& lhs)
    {
        if ( this == &lhs ) { return; }

        create(lhs.m_style, lhs.m_width, lhs.m_color);
    }

    void swap(Pen&& rhs)
    {
        if ( this == &rhs ) { return; }

        std::swap(m_style, rhs.m_style);
        std::swap(m_width, rhs.m_width);
        std::swap(m_color, rhs.m_color);
        std::swap(m_pen,   rhs.m_pen);
    }

public:
    operator HPEN() const noexcept { return m_pen; }

public:
    auto style() const noexcept { return INT32(m_style); }
    auto width() const noexcept { return m_width; }
    auto color() const noexcept { return m_color; }

public:
    Pen& create(Style style, INT32 width, COLORREF color)
    {
        m_style = style;
        m_width = width;
        m_color = color;

        m_pen = ::CreatePen(style, width, color);
        return *this;
    }

    void destroy()
    {
        if ( m_pen ) { ::DeleteObject(m_pen); m_pen = nullptr; }
    }
};

//---------------------------------------------------------------------------//

// GdiObj.hpp
#pragma once

//---------------------------------------------------------------------------//
//
// AeroWnd.hpp
//  エアロウィンドウをカプセル化するクラス
//   Copyright (C) 2005-2016 tapetums
//
//---------------------------------------------------------------------------//

#include <windows.h>
#include <windowsx.h>

#include <uxtheme.h>
#pragma comment(lib, "uxtheme.lib")

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#include "UWnd.hpp"

//---------------------------------------------------------------------------//
// 前方宣言
//---------------------------------------------------------------------------//

namespace tapetums
{
    class AeroWnd;
}

//---------------------------------------------------------------------------//
// AeroGlass対応 ウィンドウの基底クラス
//---------------------------------------------------------------------------//

class tapetums::AeroWnd : public tapetums::UWnd
{
    using super = UWnd;

protected: // members
    HANDLE m_hTheme { nullptr };

public: // ctor / dtor
    AeroWnd();
    ~AeroWnd() = default;

    AeroWnd(const AeroWnd&)             = delete;
    AeroWnd& operator= (const AeroWnd&) = delete;

    AeroWnd(AeroWnd&& rhs)            noexcept = default;
    AeroWnd& operator=(AeroWnd&& rhs) noexcept = default;

public: // methods
    bool IsCompositionEnabled() const;
    void EnableAero();
    void DisableAero();
    void ClearWindow(HDC hdc, HBRUSH hbr = nullptr);
    void CloseThemeData();

public: // window procedures
    LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wp, LPARAM lp) override;

protected: // event handlers
    LRESULT CALLBACK OnThemeChanged();
    LRESULT CALLBACK OnDwmCompositionChanged();
};

//---------------------------------------------------------------------------//
// AeroWnd ctor / dtor
//---------------------------------------------------------------------------//

inline tapetums::AeroWnd::AeroWnd()
{
    struct BufferedPaintRAII
    {
        BufferedPaintRAII()  { ::BufferedPaintInit(); }
        ~BufferedPaintRAII() { ::BufferedPaintUnInit(); }
    };

    static BufferedPaintRAII bp;
}

//---------------------------------------------------------------------------//
// AeroWnd ウィンドウプロシージャ
//---------------------------------------------------------------------------//

inline LRESULT CALLBACK tapetums::AeroWnd::WndProc
(
    HWND hwnd, UINT uMsg, WPARAM wp, LPARAM lp
)
{
    switch ( uMsg )
    {
        case WM_CREATE:
        case WM_THEMECHANGED:
        {
            return OnThemeChanged();
        }
        case WM_DWMCOMPOSITIONCHANGED:
        {
            return OnDwmCompositionChanged();
        }
        case WM_DESTROY:
        {
            CloseThemeData();
            break;
        }
        default:
        {
            break;
        }
    }

    return super::WndProc(hwnd, uMsg, wp, lp);
}

//---------------------------------------------------------------------------//
// AeroWnd メソッド
//---------------------------------------------------------------------------//

inline bool tapetums::AeroWnd::IsCompositionEnabled() const

{
    BOOL is_enabled;
    ::DwmIsCompositionEnabled(&is_enabled);

    return is_enabled ? true : false;
}

//---------------------------------------------------------------------------//

inline void tapetums::AeroWnd::EnableAero()
{
    DWM_BLURBEHIND bb{ };
    bb.dwFlags = DWM_BB_ENABLE;
    bb.fEnable = TRUE;
    bb.hRgnBlur = nullptr;
    ::DwmEnableBlurBehindWindow(m_hwnd, &bb);

    MARGINS margins{ -1 };
    ::DwmExtendFrameIntoClientArea(m_hwnd, &margins);

    Refresh();
}

//---------------------------------------------------------------------------//

inline void tapetums::AeroWnd::DisableAero()
{
    DWM_BLURBEHIND bb{ };
    bb.dwFlags = DWM_BB_ENABLE;
    bb.fEnable = FALSE;
    bb.hRgnBlur = nullptr;
    ::DwmEnableBlurBehindWindow(m_hwnd, &bb);

    MARGINS margins{ 0 };
    ::DwmExtendFrameIntoClientArea(m_hwnd, &margins);

    Refresh();
}

//---------------------------------------------------------------------------//

inline void tapetums::AeroWnd::ClearWindow
(
    HDC hdc, HBRUSH hbr
)
{
    if ( nullptr == hbr )
    {
        // 背景のブラシを取得
        if ( ::IsThemeActive() )
        {
            hbr = GetStockBrush(BLACK_BRUSH);
        }
        else
        {
            hbr = ::GetSysColorBrush(COLOR_BTNFACE);
        }
    }

    // 背景をクリア
    RECT rc { 0, 0, m_w, m_h };
    ::FillRect(hdc, &rc, hbr);
}

//---------------------------------------------------------------------------//

inline void tapetums::AeroWnd::CloseThemeData()
{
    if ( m_hTheme )
    {
        ::CloseThemeData(m_hTheme);
        m_hTheme = nullptr;
    }
}

//---------------------------------------------------------------------------//
// AeroWnd イベントハンドラ
//---------------------------------------------------------------------------//

inline LRESULT CALLBACK tapetums::AeroWnd::OnThemeChanged()
{
    CloseThemeData();

    if ( ::IsThemeActive() )
    {
        m_hTheme = ::OpenThemeData(m_hwnd, L"BUTTON");
    }

    return OnDwmCompositionChanged();
}

//---------------------------------------------------------------------------//

inline LRESULT CALLBACK tapetums::AeroWnd::OnDwmCompositionChanged()
{
    if ( IsCompositionEnabled() )
    {
        EnableAero();
    }
    else
    {
        DisableAero();
    }

    Refresh();

    return 0;
}

//---------------------------------------------------------------------------//

// AeroWnd.hpp
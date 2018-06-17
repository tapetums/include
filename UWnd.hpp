#pragma once

//---------------------------------------------------------------------------//
//
// UWnd.hpp
//  Class to capsulize window objects
//   Copyright (C) 2005-2017 tapetums
//
//---------------------------------------------------------------------------//

#include <utility>

#include <windows.h>
#include <windowsx.h>
#include <strsafe.h>

#include <shellapi.h>
#pragma comment(lib, "shell32.lib") // Shell_NotifyIcon

#ifndef WM_DPICHANGED
  #define WM_DPICHANGED 0x02E0
#endif

//---------------------------------------------------------------------------//
// Global Variables
//---------------------------------------------------------------------------//

extern UINT WM_NOTIFYICON;     // Declare the entity somewhere before use
extern UINT WM_TASKBARCREATED; // Declare the entity somewhere before use

//---------------------------------------------------------------------------//
// Forward Declarations
//---------------------------------------------------------------------------//

namespace tapetums
{
    class UWnd;

    inline void ShowLastError    (LPCTSTR window_title);
    inline void AdjustRect       (HWND hwnd, INT32* w, INT32* h);
    inline void GetRectForMonitor(HWND hwnd, RECT* rect);
    inline void GetRectForMonitor(const POINT& pt, RECT* rect);
    inline void GetDpiForMonitor (HWND hwnd, POINT* dpi);
    inline void GetDpiForMonitor (const POINT& pt,POINT* dpi);
}

//---------------------------------------------------------------------------//
// Classes
//---------------------------------------------------------------------------//

class tapetums::UWnd
{
private: // static members
    static constexpr LPCTSTR uwnd_class_name { TEXT("UWnd") };

protected: // members
    HWND m_hwnd { nullptr };

public: // accessors
    HWND handle() const noexcept { return m_hwnd; }

public: // ctor / dtor
    UWnd();
    virtual ~UWnd() { Destroy(); }

    UWnd(const UWnd&)             = delete;
    UWnd& operator= (const UWnd&) = delete;

    UWnd(UWnd&& rhs)             noexcept = default;
    UWnd& operator =(UWnd&& rhs) noexcept = default;

public: // operators
    operator HWND() const noexcept { return m_hwnd; }

public: // methods
    ATOM WINAPI Register(LPCTSTR lpszClassName);
    HWND WINAPI Create  (DWORD style = WS_OVERLAPPEDWINDOW, DWORD styleEx = 0, HWND hwndParent = nullptr, HMENU hMenu = nullptr);
    void WINAPI Destroy ();
    void WINAPI Close   ();
    void WINAPI Bounds  (INT32 x, INT32 y, INT32 w, INT32 h);
    void WINAPI Hide    ();
    void WINAPI Move    (INT32 x, INT32 y);
    void WINAPI Refresh ();
    void WINAPI Resize  (INT32 w, INT32 h);
    void WINAPI Show    ();
    void WINAPI ToCenter();

    bool WINAPI AddNotifyIcon     (UINT uID, HICON hIcon);
    void WINAPI DeleteNotifyIcon  (UINT uID);
    void WINAPI SetNotifyIconTip  (UINT uID, LPCTSTR szTip);
    bool WINAPI ShowNotifyIconInfo(UINT uID, DWORD dwInfoFlags, LPCTSTR szInfoTitle, LPCTSTR szInfo, UINT uTimeout = -1);

    LRESULT WINAPI Send(UINT msg, WPARAM wp, LPARAM lp) const;
    BOOL    WINAPI Post(UINT msg, WPARAM wp, LPARAM lp) const;

public: // properties
    DWORD  WINAPI GetStyle       () const;
    DWORD  WINAPI GetStyleEx     () const;
    HWND   WINAPI GetParent      () const;
    HFONT  WINAPI GetFont        () const;
    HICON  WINAPI GetWindowIcon  () const;
    HICON  WINAPI GetWindowIconSm() const;
    SIZE_T WINAPI GetText        (LPSTR buf, SIZE_T buf_size) const;
    SIZE_T WINAPI GetText        (LPWSTR buf, SIZE_T buf_size) const;

    void WINAPI SetStyle       (DWORD style);
    void WINAPI SetStyleEx     (DWORD styleEx);
    void WINAPI SetParent      (HWND parent);
    void WINAPI SetFont        (HFONT font);
    void WINAPI SetWindowIcon  (HMODULE hInst, LPCTSTR lpszIconName);
    void WINAPI SetWindowIcon  (HICON hIcon, HICON hIconSm);
    void WINAPI SetWindowIcon  (HICON hIcon);
    void WINAPI SetWindowIconSm(HICON hIconSm);
    void WINAPI SetText        (LPCSTR text);
    void WINAPI SetText        (LPCWSTR text);

public: // window procedures
    static  LRESULT CALLBACK WndMapProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
    virtual LRESULT CALLBACK WndProc   (HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
};

//---------------------------------------------------------------------------//
// UWnd ctor
//---------------------------------------------------------------------------//

inline tapetums::UWnd::UWnd()
{
    static ATOM atom { 0 };
    if ( atom == 0 ) { atom = Register(uwnd_class_name); }
}

//---------------------------------------------------------------------------//
// UWnd Methods
//---------------------------------------------------------------------------//

inline ATOM WINAPI tapetums::UWnd::Register
(
    LPCTSTR lpszClassName
)
{
    WNDCLASSEX wcex
    {
        sizeof(WNDCLASSEX),
        CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
        WndMapProc,
        0, 0,
        ::GetModuleHandle(nullptr),
        nullptr,
        ::LoadCursor(nullptr, IDC_ARROW),
        nullptr,
        nullptr,
        lpszClassName,
        nullptr,
    };

    return ::RegisterClassEx(&wcex);
}

//---------------------------------------------------------------------------//

inline HWND WINAPI tapetums::UWnd::Create
(
    DWORD style,
    DWORD styleEx,
    HWND  hwndParent,
    HMENU hMenu
)
{
    if ( m_hwnd ) { return m_hwnd; }

    const auto hwnd = ::CreateWindowEx
    (
        styleEx, uwnd_class_name, nullptr, style,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        hwndParent, hMenu, ::GetModuleHandle(nullptr),
        reinterpret_cast<LPVOID>(this)
    );
    if ( hwnd == nullptr )
    {
        ShowLastError(uwnd_class_name);
    }
    else
    {
        ::UpdateWindow(hwnd);
    }

    return hwnd;
}

//---------------------------------------------------------------------------//

inline void WINAPI tapetums::UWnd::Destroy()
{
    if ( m_hwnd )
    {
        ::DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
}

//---------------------------------------------------------------------------//

inline void WINAPI tapetums::UWnd::Close()
{
    ::SendMessage(m_hwnd, WM_CLOSE, 0, 0);
}

//---------------------------------------------------------------------------//

inline void WINAPI tapetums::UWnd::Bounds
(
    INT32 x, INT32 y, INT32 w, INT32 h
)
{
    AdjustRect(m_hwnd, &w, &h);

    ::SetWindowPos
    (
        m_hwnd, nullptr,
        x, y, w, h,
        SWP_NOZORDER | SWP_FRAMECHANGED
    );
}

//---------------------------------------------------------------------------//

inline void WINAPI tapetums::UWnd::Hide()
{
    ::ShowWindowAsync(m_hwnd, SW_HIDE);
}

//---------------------------------------------------------------------------//

inline void WINAPI tapetums::UWnd::Move
(
    INT32 x, INT32 y
)
{
    ::SetWindowPos
    (
        m_hwnd, nullptr,
        x, y, 0, 0,
        SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
    );
}

//---------------------------------------------------------------------------//

inline void WINAPI tapetums::UWnd::Refresh()
{
    ::InvalidateRect(m_hwnd, nullptr, FALSE);
}

//---------------------------------------------------------------------------//

inline void WINAPI tapetums::UWnd::Resize
(
    INT32 w, INT32 h
)
{
    AdjustRect(m_hwnd, &w, &h);

    ::SetWindowPos
    (
        m_hwnd, nullptr,
        0, 0, w, h,
        SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED
    );
}

//---------------------------------------------------------------------------//

inline void WINAPI tapetums::UWnd::Show()
{
    ::ShowWindowAsync(m_hwnd, SW_SHOWNORMAL);
}

//---------------------------------------------------------------------------//

inline void WINAPI tapetums::UWnd::ToCenter()
{
    RECT rc;
    INT32 mx, my, mw, mh;

    if ( const auto parent = GetParent() )
    {
        ::GetWindowRect(parent, &rc);
    }
    else
    {
        POINT pt;
        ::GetCursorPos(&pt);

        GetRectForMonitor(pt, &rc);
    }

    mx = rc.left;
    my = rc.top;
    mw = rc.right  - rc.left;
    mh = rc.bottom - rc.top;

    ::GetClientRect(m_hwnd, &rc);
    const auto w = rc.right  - rc.left;
    const auto h = rc.bottom - rc.top;

    const auto x = (mw - w) / 2 + mx;
    const auto y = (mh - h) / 2 + my;

    return Move(x, y);
}

//---------------------------------------------------------------------------//

inline bool WINAPI tapetums::UWnd::AddNotifyIcon
(
    UINT uID, HICON hIcon
)
{
    if ( WM_TASKBARCREATED == 0 )
    {
        WM_TASKBARCREATED = ::RegisterWindowMessage(TEXT("TaskbarCreated"));
    }
    if ( WM_NOTIFYICON == 0 )
    {
        WM_NOTIFYICON = ::RegisterWindowMessage(TEXT("NotifyIcon"));
    }

    NOTIFYICONDATA nid;
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd   = m_hwnd;
    nid.uID    = uID;
    nid.uFlags = NIF_MESSAGE | NIF_ICON;
    nid.uCallbackMessage = WM_NOTIFYICON;
    nid.hIcon  = hIcon;

    for ( ; ; )
    {
        if ( ::Shell_NotifyIcon(NIM_ADD, &nid) )
        {
            return true;
        }

        // 以下、スタートアッププログラムとして起動した際
        // タイミングによってアイコンの登録が失敗する問題への対策
        // Microsoft: KB418138
        if ( ::GetLastError() != ERROR_TIMEOUT )
        {
            ::MessageBox
            (
                nullptr, nullptr, TEXT("Failed to add notify icon"),
                MB_ICONEXCLAMATION | MB_OK
            );
            return false;
        }

        if ( ::Shell_NotifyIcon(NIM_MODIFY, &nid) )
        {
            return true;
        }
        else
        {
            ::Sleep(1000);
        }
    }
}

//---------------------------------------------------------------------------//

inline void WINAPI tapetums::UWnd::DeleteNotifyIcon
(
    UINT uID
)
{
    NOTIFYICONDATA nid;
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd   = m_hwnd;
    nid.uID    = uID;
    nid.uFlags = 0;

    ::Shell_NotifyIcon(NIM_DELETE, &nid);
}

//---------------------------------------------------------------------------//

inline void WINAPI tapetums::UWnd::SetNotifyIconTip
(
    UINT uID, LPCTSTR szTip
)
{
    NOTIFYICONDATA nid;
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd   = m_hwnd;
    nid.uID    = uID;
    nid.uFlags = NIF_TIP;
  #if (NTDDI_VERSION < NTDDI_WIN2K)
    ::StringCchCopy(nid.szTip, 64, szTip);
  #else
    ::StringCchCopy(nid.szTip, 128, szTip);
  #endif

    ::Shell_NotifyIcon(NIM_MODIFY, &nid);
}

//---------------------------------------------------------------------------//

inline bool WINAPI tapetums::UWnd::ShowNotifyIconInfo
(
    UINT uID, DWORD dwInfoFlags,
    LPCTSTR szInfoTitle, LPCTSTR szInfo, UINT uTimeout
)
{
  #if NTDDI_VERSION < NTDDI_WIN2K
    return false;
  #endif

    NOTIFYICONDATA nid;
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd   = m_hwnd;
    nid.uID    = uID;
    nid.uFlags = NIF_INFO;
    ::StringCchCopy(nid.szInfo, 256, szInfo);
    nid.uTimeout = uTimeout;
    ::StringCchCopy(nid.szInfoTitle, 64, szInfoTitle);
    nid.dwInfoFlags = dwInfoFlags;

    ::Shell_NotifyIcon(NIM_MODIFY, &nid);

    return true;
}

//---------------------------------------------------------------------------//

inline LRESULT WINAPI tapetums::UWnd::Send
(
    UINT msg, WPARAM wp, LPARAM lp
)
const
{
    return ::SendMessage(m_hwnd, msg, wp, lp);
}

//---------------------------------------------------------------------------//

inline BOOL WINAPI tapetums::UWnd::Post
(
    UINT msg, WPARAM wp, LPARAM lp
)
const
{
    return ::PostMessage(m_hwnd, msg, wp, lp);
}

//---------------------------------------------------------------------------//
// UWnd Properties
//---------------------------------------------------------------------------//

inline DWORD WINAPI tapetums::UWnd::GetStyle() const
{
    return (DWORD)::GetWindowLongPtr(m_hwnd, GWL_STYLE);
}

//---------------------------------------------------------------------------//

inline DWORD WINAPI tapetums::UWnd::GetStyleEx() const
{
    return (DWORD)::GetWindowLongPtr(m_hwnd, GWL_EXSTYLE);
}

//---------------------------------------------------------------------------//

inline HWND WINAPI tapetums::UWnd::GetParent() const
{
    return (HWND)::GetWindowLongPtr(m_hwnd, GWLP_HWNDPARENT);
}

//---------------------------------------------------------------------------//

inline HFONT WINAPI tapetums::UWnd::GetFont() const
{
    return (HFONT)::SendMessage(m_hwnd, WM_GETFONT, 0, 0);
}

//---------------------------------------------------------------------------//

inline HICON WINAPI tapetums::UWnd::GetWindowIcon() const
{
    return (HICON)::GetClassLongPtr(m_hwnd, GCLP_HICON);
}

//---------------------------------------------------------------------------//

inline HICON WINAPI tapetums::UWnd::GetWindowIconSm() const
{
    return (HICON)::GetClassLongPtr(m_hwnd, GCLP_HICONSM);
}

//---------------------------------------------------------------------------//

inline SIZE_T WINAPI tapetums::UWnd::GetText
(
    LPSTR buf, SIZE_T buf_size
)
const
{
    return (SIZE_T)::SendMessageA
    (
        m_hwnd, WM_GETTEXT, WPARAM(buf_size), LPARAM(buf)
    );
}

//---------------------------------------------------------------------------//

inline SIZE_T WINAPI tapetums::UWnd::GetText
(
    LPWSTR buf, SIZE_T buf_size
)
const
{
    return (SIZE_T)::SendMessageW
    (
        m_hwnd, WM_GETTEXT, WPARAM(buf_size), LPARAM(buf)
    );
}

//---------------------------------------------------------------------------//

inline void WINAPI tapetums::UWnd::SetStyle
(
    DWORD style
)
{
    ::SetWindowLongPtr(m_hwnd, GWL_STYLE, LONG_PTR(style));
}

//---------------------------------------------------------------------------//

inline void WINAPI tapetums::UWnd::SetStyleEx
(
    DWORD styleEx
)
{
    ::SetWindowLongPtr(m_hwnd, GWL_EXSTYLE, LONG_PTR(styleEx));
}

//---------------------------------------------------------------------------//

inline void WINAPI tapetums::UWnd::SetParent
(
    HWND parent
)
{
    ::SetWindowLongPtr(m_hwnd, GWLP_HWNDPARENT, LONG_PTR(parent));
}

//---------------------------------------------------------------------------//

inline void WINAPI tapetums::UWnd::SetFont
(
    HFONT font
)
{
    ::SendMessage(m_hwnd, WM_SETFONT, WPARAM(font), LPARAM(FALSE));
}

//---------------------------------------------------------------------------//

inline void WINAPI tapetums::UWnd::SetWindowIcon
(
    HMODULE hInst, LPCTSTR lpszIconName
)
{
    const auto hIcon = (HICON)::LoadImage
    (
        hInst, lpszIconName,
        IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED
    );

    const auto hIconSm = (HICON)::LoadImage
    (
        hInst, lpszIconName,
        IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR | LR_SHARED
    );

    return SetWindowIcon(hIcon, hIconSm);
}

//---------------------------------------------------------------------------//

inline void WINAPI tapetums::UWnd::SetWindowIcon
(
    HICON hIcon, HICON hIconSm
)
{
    ::SetClassLongPtr(m_hwnd, GCLP_HICON,   LONG_PTR(hIcon));
    ::SetClassLongPtr(m_hwnd, GCLP_HICONSM, LONG_PTR(hIconSm));
}

//---------------------------------------------------------------------------//

inline void WINAPI tapetums::UWnd::SetWindowIcon
(
    HICON hIcon
)
{
    ::SetClassLongPtr(m_hwnd, GCLP_HICON, LONG_PTR(hIcon));
}

//---------------------------------------------------------------------------//

inline void WINAPI tapetums::UWnd::SetWindowIconSm
(
    HICON hIconSm
)
{
    ::SetClassLongPtr(m_hwnd, GCLP_HICONSM, LONG_PTR(hIconSm));
}

//---------------------------------------------------------------------------//

inline void WINAPI tapetums::UWnd::SetText
(
    LPCSTR text
)
{
    ::SendMessageA(m_hwnd, WM_SETTEXT, 0, LPARAM(text));
}

//---------------------------------------------------------------------------//

inline void WINAPI tapetums::UWnd::SetText
(
    LPCWSTR text
)
{
    ::SendMessageW(m_hwnd, WM_SETTEXT, 0, LPARAM(text));
}

//---------------------------------------------------------------------------//
// UWnd ウィンドウプロシージャ
//---------------------------------------------------------------------------//

inline LRESULT CALLBACK tapetums::UWnd::WndMapProc
(
    HWND hwnd, UINT msg, WPARAM wp, LPARAM lp
)
{
    UWnd* wnd;

    if ( msg == WM_NCCREATE )
    {
        wnd = (UWnd*)((CREATESTRUCT*)lp)->lpCreateParams;

        ::SetWindowLongPtr(hwnd, GWLP_USERDATA, LONG_PTR(wnd));
    }
    else
    {
        wnd = (UWnd*)::GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if ( wnd == nullptr )
    {
        return ::DefWindowProc(hwnd, msg, wp, lp);
    }

    if ( msg == WM_CREATE )
    {
        wnd->m_hwnd = hwnd;
    }

    return wnd->WndProc(hwnd, msg, wp, lp);
}

//---------------------------------------------------------------------------//

inline LRESULT CALLBACK tapetums::UWnd::WndProc
(
    HWND hwnd, UINT msg, WPARAM wp, LPARAM lp
)
{
    return ::DefWindowProc(hwnd, msg, wp, lp);
}

//---------------------------------------------------------------------------//
// Utility Functions
//---------------------------------------------------------------------------//

inline void tapetums::ShowLastError
(
    LPCTSTR window_title
)
{
    LPTSTR lpMsgBuf{ nullptr };
    ::FormatMessage
    (
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        ::GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        LPTSTR(&lpMsgBuf),
        0,
        nullptr
    );
    ::MessageBox(nullptr, lpMsgBuf, window_title, MB_OK);
    ::LocalFree(lpMsgBuf);
}

//---------------------------------------------------------------------------//

inline void tapetums::AdjustRect
(
    HWND hwnd, INT32* w, INT32* h
)
{
    RECT rc{ 0, 0, *w, *h };
    const auto style   = (DWORD)::GetWindowLongPtr(hwnd, GWL_STYLE);
    const auto styleEx = (DWORD)::GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    const auto isChild = style & WS_CHILD;
    const BOOL hasMenu = (!isChild && ::GetMenu(hwnd)) ? TRUE : FALSE;

    ::AdjustWindowRectEx(&rc, style, hasMenu, styleEx);
    *w = rc.right  - rc.left;
    *h = rc.bottom - rc.top;
}

//---------------------------------------------------------------------------//

inline void tapetums::GetRectForMonitor
(
    HWND hwnd, RECT* rect
)
{
    const auto hMonitor = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

    MONITORINFOEX miex{ };
    miex.cbSize = (DWORD)sizeof(MONITORINFOEX);
    ::GetMonitorInfo(hMonitor, &miex);

    *rect = miex.rcMonitor;
}

//---------------------------------------------------------------------------//

inline void tapetums::GetRectForMonitor
(
    const POINT& pt, RECT* rect
)
{
    const auto hMonitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

    MONITORINFOEX miex{ };
    miex.cbSize = (DWORD)sizeof(MONITORINFOEX);
    ::GetMonitorInfo(hMonitor, &miex);

    *rect = miex.rcMonitor;
}

//---------------------------------------------------------------------------//

#include <ShellScalingApi.h>

inline void tapetums::GetDpiForMonitor
(
    HWND hwnd, POINT* dpi
)
{
    static const auto Shcore = ::LoadLibraryEx
    (
        TEXT("Shcore.dll"), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH
    );

    using F = HRESULT (__stdcall *)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);
    static const auto GetDpiForMonitor = (F)::GetProcAddress
    (
        Shcore, "GetDpiForMonitor"
    );

    if ( GetDpiForMonitor )
    {
        const auto hMonitor = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);

        GetDpiForMonitor
        (
            hMonitor, MONITOR_DPI_TYPE::MDT_EFFECTIVE_DPI,
            (UINT*)&dpi->x, (UINT*)&dpi->y
        );
    }
    else
    {
        const auto hDC = ::GetDC(nullptr);
        if ( hDC )
        {
            dpi->x = ::GetDeviceCaps(hDC, LOGPIXELSX);
            dpi->y = ::GetDeviceCaps(hDC, LOGPIXELSY);

            ::ReleaseDC(nullptr, hDC);
        }
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::GetDpiForMonitor
(
    const POINT& pt, POINT* dpi
)
{
    static const auto Shcore = ::LoadLibraryEx
    (
        TEXT("Shcore.dll"), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH
    );

    using F = HRESULT (__stdcall *)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);
    static const auto GetDpiForMonitor = (F)::GetProcAddress
    (
        Shcore, "GetDpiForMonitor"
    );

    if ( GetDpiForMonitor )
    {
        const auto hMonitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

        GetDpiForMonitor
        (
            hMonitor, MONITOR_DPI_TYPE::MDT_EFFECTIVE_DPI,
            (UINT*)&dpi->x, (UINT*)&dpi->y
        );
    }
    else
    {
        const auto hDC = ::GetDC(nullptr);
        if ( hDC )
        {
            dpi->x = ::GetDeviceCaps(hDC, LOGPIXELSX);
            dpi->y = ::GetDeviceCaps(hDC, LOGPIXELSY);

            ::ReleaseDC(nullptr, hDC);
        }
    }
}

//---------------------------------------------------------------------------//

// UWnd.hpp
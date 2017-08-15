#pragma once

//---------------------------------------------------------------------------//
//
// OpenGLWnd.hpp
//  OpenGL で描画するウィンドウの基底クラス
//   Copyright (C) 2005-2016 tapetums
//
//---------------------------------------------------------------------------//

#include <gl/gl.h>
#include <gl/glext.h>
#pragma comment(lib, "opengl32.lib")

#include "AeroWnd.hpp"

//---------------------------------------------------------------------------//
// 前方宣言
//---------------------------------------------------------------------------//

namespace tapetums
{
    class OpenGLWnd;
}

//---------------------------------------------------------------------------//

class tapetums::OpenGLWnd : public tapetums::AeroWnd
{
protected:
    HDC   m_dc   { nullptr };
    HGLRC m_glrc { nullptr };

public:
    OpenGLWnd()  = default;
    ~OpenGLWnd() = default;

    OpenGLWnd(const OpenGLWnd&)             = delete;
    OpenGLWnd& operator= (const OpenGLWnd&) = delete;

    OpenGLWnd(OpenGLWnd&& rhs)            noexcept = default;
    OpenGLWnd& operator=(OpenGLWnd&& rhs) noexcept = default;

public:
    virtual void Update() = 0;

protected:
    bool CreateContext(HWND hwnd);
    void ReleaseContext();
};

//---------------------------------------------------------------------------//

inline bool tapetums::OpenGLWnd::CreateContext(HWND hwnd)
{
    m_dc = ::GetDC(hwnd);

    constexpr PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,   // color
        0, 0, // R
        0, 0, // G
        0, 0, // B
        0, 0, // A
        0,    // AC
        0, 0, 0, 0, // RGBA
        24,   // depth
        8,    // stencil
        0,    // aux
        PFD_MAIN_PLANE,
        0,    // reserved
        0,    // layer mask
        0,    // visible mask
        0     // damage mask
    };

    const auto pxfmt = ::ChoosePixelFormat(m_dc, &pfd);
    if ( pxfmt == 0 )
    {
        ShowLastError(TEXT("OpenGLWnd::CreateContext::ChoosePixelFormat()"));
        ReleaseContext();
        return false;
    }

    const auto result = ::SetPixelFormat(m_dc, pxfmt, &pfd);
    if ( ! result )
    {
        ShowLastError(TEXT("OpenGLWnd::CreateContext::SetPixelFormat()"));
        ReleaseContext();
        return false;
    }

    m_glrc = ::wglCreateContext(m_dc);
    ::wglMakeCurrent(m_dc, m_glrc);

    return true;
}

//---------------------------------------------------------------------------//

inline void tapetums::OpenGLWnd::ReleaseContext()
{
    ::wglMakeCurrent(m_dc, nullptr);

    ::wglDeleteContext(m_glrc);
    m_glrc = nullptr;

    ::ReleaseDC(m_hwnd, m_dc);
    m_dc = nullptr;
}

//---------------------------------------------------------------------------//

// OpenGLWnd.hpp
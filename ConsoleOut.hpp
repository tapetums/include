#pragma once

//---------------------------------------------------------------------------//
//
// ConsoleOut.hpp
// デバッグウィンドウへの出力関数
//   Copyright (C) 2014-2015 tapetums
//
//---------------------------------------------------------------------------//

#ifndef CONSOLE_OUT

  #define console_outA(x, ...)
  #define console_outW(x, ...)
  #define console_out(x, ...)

#else

  inline void console_outA(const char*    format, ...);
  inline void console_outW(const wchar_t* format, ...);

  #if defined(_UNICODE) || defined(UNICODE)
    #define console_out console_outW
  #else
    #define console_out console_outA
  #endif

#endif

//---------------------------------------------------------------------------//

#ifdef CONSOLE_OUT

#include <array>

#include <windows.h>
#include <strsafe.h>

#include "Lock.hpp"

//---------------------------------------------------------------------------//
// 定数
//---------------------------------------------------------------------------//

constexpr size_t TIME_BUFSIZE    = 32;
constexpr size_t CONSOLE_BUFSIZE = 1024;

//---------------------------------------------------------------------------//
// 排他ロック
//---------------------------------------------------------------------------//

struct lock
{
    static tapetums::CsLock::Lock& get()
    {
        static tapetums::CsLock::Lock g_lock;
        return g_lock;
    }
};

//---------------------------------------------------------------------------//
// 関数
//---------------------------------------------------------------------------//

inline void console_outA(const char* format, ...)
{
    if ( nullptr == format )
    {
        return;
    }

    thread_local std::array<char, TIME_BUFSIZE>    time;
    thread_local std::array<char, CONSOLE_BUFSIZE> buff;
    thread_local SYSTEMTIME st;
    thread_local DWORD threadId = ::GetCurrentThreadId();

    // 現在時刻を取得
    ::GetLocalTime(&st);

    // 時刻を文字列に
    ::StringCchPrintfA
    (
        time.data(), time.size(),
        "*%04x %02u:%02u:%02u;%03u> ", threadId,
        st.wHour, st.wMinute, st.wSecond, st.wMilliseconds
    );

    // 引数を文字列に
    va_list al;
    va_start(al, format);
    {
        ::StringCchVPrintfA(buff.data(), CONSOLE_BUFSIZE, format, al);
    }
    va_end(al);

    // 排他制御
    {
        tapetums::CsLock::CS cs(lock::get());
        cs.enter();

        ::OutputDebugStringA(time.data());
        ::OutputDebugStringA(buff.data());
        ::OutputDebugStringA("\n");
    }
}

//---------------------------------------------------------------------------//

inline void console_outW(const wchar_t* format, ...)
{
    if ( nullptr == format )
    {
        return;
    }

    thread_local std::array<wchar_t, TIME_BUFSIZE>    time;
    thread_local std::array<wchar_t, CONSOLE_BUFSIZE> buff;
    thread_local SYSTEMTIME st;
    thread_local DWORD threadId = ::GetCurrentThreadId();

    // 現在時刻を取得
    ::GetLocalTime(&st);

    // 時刻を文字列に
    ::StringCchPrintfW
    (
        time.data(), time.size(),
        L"*%04x %02u:%02u:%02u;%03u> ", threadId,
        st.wHour, st.wMinute, st.wSecond, st.wMilliseconds
    );

    // 引数を文字列に
    va_list al;
    va_start(al, format);
    {
        ::StringCchVPrintfW(buff.data(), CONSOLE_BUFSIZE, format, al);
    }
    va_end(al);

    // 排他制御
    {
        tapetums::CsLock::CS cs(lock::get());
        cs.enter();

        ::OutputDebugStringW(time.data());
        ::OutputDebugStringW(buff.data());
        ::OutputDebugStringW(L"\n");
    }
}

//---------------------------------------------------------------------------//

#endif

//---------------------------------------------------------------------------//

// ConsoleOut.hpp
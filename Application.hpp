#pragma once

//---------------------------------------------------------------------------//
//
// Application.hpp
//  メッセージループをカプセル化するクラス
//   Copyright (C) 2013-2017 tapetums
//
//---------------------------------------------------------------------------//

#include <windows.h>

//---------------------------------------------------------------------------//

namespace tapetums
{
    class Application;
}

//---------------------------------------------------------------------------//
// Class
//---------------------------------------------------------------------------//

// メッセージループをカプセル化するクラス
class tapetums::Application
{
public:
    Application()  = delete;
    ~Application() = delete;

    Application(const Application&)             = delete;
    Application& operator= (const Application&) = delete;

    Application(Application&&)             noexcept = delete;
    Application& operator= (Application&&) noexcept = delete;

public:
    static DWORD thread_id() noexcept { return m_thread_id(); }
    static bool  is_loop()   noexcept { return m_loop(); }

public:
    static INT32 Run();
    static bool  Exit();
    static void  Pause();
    static void  Resume();

    template<typename Updater, typename... Args>
    static INT32 Run(Updater& update, Args&... args);

private:
    static DWORD& m_thread_id() noexcept { static DWORD id   { 0 };    return id; }
    static bool&  m_loop()      noexcept { static bool  loop { true }; return loop; }
};

//---------------------------------------------------------------------------//
// Methods
//---------------------------------------------------------------------------//

// メッセージループ
inline INT32 tapetums::Application::Run()
{
    // 静的変数を初期化
    m_thread_id() = ::GetCurrentThreadId();

    MSG msg;

    // メインループ
    while ( ::GetMessage(&msg, nullptr, 0, 0) > 0 )
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    return static_cast<INT32>(msg.wParam);
}

//---------------------------------------------------------------------------//

// ゲームループ
template<typename Updater, typename... Args>
inline INT32 tapetums::Application::Run
(
    Updater&  update
    Args&...  args
)
{
    // 静的変数を初期化
    m_thread_id() = ::GetCurrentThreadId();
    m_loop()      = true;

    MSG msg;

    // メインループ
    for ( ; ; )
    {
        if ( ::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) )
        {
            if ( msg.message == WM_QUIT ) { break; }
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        else if ( is_loop() )
        {
            update(args...); // ファンクタの呼び出し
        }
        else
        {
            ::MsgWaitForMultipleObjects
            (
                0, nullptr, FALSE, INFINITE, QS_ALLINPUT
            );
        }
    }

    return static_cast<INT32>(msg.wParam);
}

//---------------------------------------------------------------------------//

// メッセージループの終了
inline bool tapetums::Application::Exit()
{
    return ::PostThreadMessage(thread_id(), WM_QUIT, 0, 0) ? true : false;
}

//---------------------------------------------------------------------------//

// ゲームループを停止
inline void tapetums::Application::Pause()
{
    m_loop() = false;
}

//---------------------------------------------------------------------------//

// ゲームループを再開
inline void tapetums::Application::Resume()
{
    m_loop() = true;
}

//---------------------------------------------------------------------------//

// Application.hpp
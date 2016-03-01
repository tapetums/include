#pragma once

//---------------------------------------------------------------------------//
//
// WinMutex.hpp
//  ミューテックスのRAII
//   Copyright (C) 2016 tapetums
//
//---------------------------------------------------------------------------//

#include <windows.h>

//---------------------------------------------------------------------------//

namespace tapetums
{
    class WinMutex;
}


//---------------------------------------------------------------------------//

class tapetums::WinMutex
{
private:
    HANDLE mutex { nullptr };

public:
    WinMutex() noexcept = default;
    ~WinMutex() { release(); ::CloseHandle(mutex); }

    WinMutex(const WinMutex&) = delete;
    WinMutex& operator =(const WinMutex&) = delete;

    WinMutex(WinMutex&&) noexcept = delete;
    WinMutex& operator =(WinMutex&&) noexcept = delete;

    explicit WinMutex(LPCTSTR name, bool own = true){ lock(name, own); }

public:
    operator HANDLE() { return mutex; }

public:
    bool lock(LPCTSTR name, bool own)
    {
        if ( mutex ) { return true; }

        mutex = ::CreateMutex(nullptr, own, name));
        if ( ::GetLastError() == ERROR_ALREADY_EXISTS )
        {
            mutex = ::OpenMutex(MUTEX_ALL_ACCESS , own, name));
        }

        return mutex != nullptr;
    }

    bool lock()
    {
        if ( nullptr == mutex ) { return false; }

        mutex = ::OpenMutex(MUTEX_ALL_ACCESS , own, name));

        return mutex != nullptr;
    }

    void release()
    {
        if ( nullptr == mutex ) { return; }

        ::ReleaseMutex(mutex);
    }
};

//---------------------------------------------------------------------------//

// WinMutex.hpp
#pragma once

//---------------------------------------------------------------------------//
//
// Lock.hpp
//  Lock の RAII クラス
//   Copyright (C) 2015 tapetums
//
//---------------------------------------------------------------------------//

#include <climits>

#include <atomic>

#include <windows.h>

//---------------------------------------------------------------------------//

namespace tapetums {

//---------------------------------------------------------------------------//
//
// CsLock
//  Windows CriticalSection の RAII 実装
//
//---------------------------------------------------------------------------//

namespace CsLock {

//---------------------------------------------------------------------------//

struct Lock final
{
private:
    CRITICAL_SECTION cs;

public:
    Lock()
    {
        ::InitializeCriticalSection(&cs);
    }

    ~Lock()
    {
        ::DeleteCriticalSection(&cs);
    }

    Lock(const Lock&) = delete;
    Lock& operator =(const Lock&) = delete;

    Lock(Lock&& rhs) noexcept = default;
    Lock& __stdcall operator=(Lock&& rhs) noexcept = default;

public:
    inline void __stdcall enter() noexcept { ::EnterCriticalSection(&cs); }
    inline void __stdcall leave() noexcept { ::LeaveCriticalSection(&cs); }
};

//---------------------------------------------------------------------------//

struct CS final
{
public:
    explicit CS(Lock& lock) : m_lock(lock) { }
    ~CS() { m_lock.leave(); }

    inline void __stdcall enter() noexcept { m_lock.enter(); }
    inline void __stdcall leave() noexcept { m_lock.leave(); }

private:
    Lock& m_lock;
};

//---------------------------------------------------------------------------//

} // namespace CsLock

//---------------------------------------------------------------------------//
//
// SRWLock
//  Windows Slim Reader/Writer Lock の RAII 実装
//
//---------------------------------------------------------------------------//

namespace SRWLock {

//---------------------------------------------------------------------------//

struct Lock final
{
public:
    Lock()
    {
        ::InitializeSRWLock(&m_srwl);
    }

    ~Lock()
    {
    }

    Lock(Lock&& rhs) noexcept
    {
        operator =(std::move(rhs));
    }

    Lock& __stdcall operator=(Lock&& rhs) noexcept
    {
        std::swap(m_srwl, rhs.m_srwl); return *this;
    }

public:
    inline SRWLOCK* __stdcall ptr() const noexcept
    {
        return (SRWLOCK*)&m_srwl;
    }

public:
    inline bool __stdcall read_lock() noexcept
    {
        if ( ::TryAcquireSRWLockShared(&m_srwl) )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    inline void __stdcall read_unlock() noexcept
    {
        ::ReleaseSRWLockShared(&m_srwl);
    }

    inline bool __stdcall write_lock() noexcept
    {
        if ( ::TryAcquireSRWLockExclusive(&m_srwl) )
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    inline void __stdcall write_unlock() noexcept
    {
        ::ReleaseSRWLockExclusive(&m_srwl);
    }

private:
    SRWLOCK m_srwl;

private:
    Lock(const Lock&) = delete;
    Lock& operator =(const Lock&) = delete;
};

//---------------------------------------------------------------------------//

struct ReadLock final
{
public:
    explicit ReadLock(Lock& lock) : m_lock(lock)
    {
    }

    ~ReadLock()
    {
        release();
    }

public:
    inline bool __stdcall is_acquired() const noexcept
    {
        return m_acquired;
    }

    inline bool __stdcall is_upgraded() const noexcept
    {
        return m_upgraded;
    }

public:
    inline bool __stdcall acquire() noexcept
    {
        if ( m_acquired )
        {
            return true;
        }

        if ( !m_lock.read_lock() )
        {
            return false;
        }

        m_acquired = true;
        return true;
    }

    inline void __stdcall release() noexcept
    {
        if ( m_acquired )
        {
            m_lock.read_unlock();
            m_acquired = false;
        }
    }

private:
    Lock& m_lock;
    bool m_acquired{ false };
    bool m_upgraded{ false };

private:
    ReadLock(const ReadLock&) = delete;
    ReadLock(ReadLock&&) = delete;
    ReadLock& operator =(const ReadLock&) = delete;
    ReadLock& operator =(ReadLock&&) = delete;
};

//---------------------------------------------------------------------------//

struct WriteLock final
{
public:
    explicit WriteLock(Lock& lock) : m_lock(lock)
    {
    }

    ~WriteLock()
    {
        release();
    }

public:
    inline bool __stdcall is_acquired() const noexcept
    {
        return m_acquired;
    }

public:
    inline bool __stdcall acquire() noexcept
    {
        if ( m_acquired )
        {
            return true;
        }

        if ( !m_lock.write_lock() )
        {
            return false;
        }

        m_acquired = true;
        return true;
    }

    inline void __stdcall release() noexcept
    {
        if ( m_acquired )
        {
            m_lock.write_unlock();
            m_acquired = false;
        }
    }

private:
    Lock& m_lock;
    bool m_acquired{ false };

private:
    WriteLock(const WriteLock&) = delete;
    WriteLock(WriteLock&&) = delete;
    WriteLock& operator =(const WriteLock&) = delete;
    WriteLock& operator =(WriteLock&&) = delete;
};

//---------------------------------------------------------------------------//

} // namespace SRWLock

//---------------------------------------------------------------------------//
//
// RWLock
//  std::atomic による Readers-Writer Lock の RAII 実装
//
// FIX IT: ロックの獲得に失敗したスレッドのうち、先に失敗したもののほうが
//         再度獲得を試みた際に後まわしにされる傾向がある
//      -> ReentrantLock スタイルに改造すべき？
//
//---------------------------------------------------------------------------//
// Usage:
/*
class Foo
{
public:
    size_t read(const void* object, uint8_t* data, size_t size)
    {
        RWLock::ReadLock lock{ rwlock_ };
        if ( !lock.aquire() )
        {
            return 0;
        }

        size_t cb_read{ 0 };
        // read object...

        return cb_read;
    }

    size_t write(void* object, const uint8_t* data, size_t size)
    {
        RWLock::WriteLock lock{ rwlock_ };
        if ( !lock.aquire() )
        {
            return 0;
        }

        size_t cb_wrote{ 0 };
        // write data...

        return cb_wrote;
    }

private:
    RWLock::Lock rwlock_;
};
*/
//--------------------------------------------------------------------------//

namespace RWLock {

//---------------------------------------------------------------------------//

// ロック状態を保持するインスタンス用のクラス
struct Lock final
{
public:
    std::atomic<size_t> readers_cnt { 0 };
    std::atomic<bool>   is_writing  { false };

private:
    Lock(const Lock&)             = delete;
    Lock(Lock&&)                  = delete;
    Lock& operator =(const Lock&) = delete;
    Lock& operator =(Lock&&)      = delete;
};

//---------------------------------------------------------------------------//

// 読み出しロックの RAII
struct ReadLock final
{
public:
    explicit ReadLock(Lock& lock) : m_lock(lock)
    {
    }

    ~ReadLock()
    {
        release();
    }

public:
    inline bool __stdcall is_acquired() const noexcept
    {
        return m_acquired;
    }

    inline bool __stdcall is_upgraded() const noexcept
    {
        return m_upgraded;
    }

public:
    inline bool __stdcall acquire() noexcept
    {
        if ( m_acquired )
        {
            return true;
        }

        if ( m_lock.is_writing.load() )
        {
            return false;
        }

        if ( m_lock.readers_cnt.load() == SIZE_MAX ) // Integer Overflow
        {
            return false;
        }

        ++m_lock.readers_cnt;
        m_acquired = true;

        return true;
    }

    inline bool __stdcall upgrade() noexcept
    {
        if ( !m_acquired )
        {
            return false;
        }

        bool expected{ false };

        // 排他制御 (Non-blocking / Lock-free)
        if ( !m_lock.is_writing.compare_exchange_weak(expected, true) )
        {
            return false;
        }

        // 排他制御 (Blocking / Lock-free)
        while ( m_lock.readers_cnt.load() > 1 ) // 自分自身が1つ持ってる
        {
            continue; // spinning
        }

        m_upgraded = true;

        return true;
    }

    inline void __stdcall downgrade() noexcept
    {
        if ( m_upgraded )
        {
            m_lock.is_writing = false;
            m_upgraded = false;
        }
    }

    inline void __stdcall release() noexcept
    {
        downgrade();

        if ( m_acquired )
        {
            --m_lock.readers_cnt;
            m_acquired = false;
        }
    }

private:
    Lock& m_lock;
    bool m_acquired{ false };
    bool m_upgraded{ false };

private:
    ReadLock(const ReadLock&) = delete;
    ReadLock(ReadLock&&) = delete;
    ReadLock& operator =(const ReadLock&) = delete;
    ReadLock& operator =(ReadLock&&) = delete;
};

//---------------------------------------------------------------------------//

// 書き出しロックの RAII
struct WriteLock final
{
public:
    explicit WriteLock(Lock& lock) : m_lock(lock)
    {
    }

    ~WriteLock()
    {
        release();
    }

public:
    inline bool __stdcall is_acquired() const noexcept
    {
        return m_acquired;
    }

public:
    inline bool __stdcall acquire() noexcept
    {
        if ( m_acquired )
        {
            return true;
        }

        bool expected{ false };

        // 排他制御 (Non-blocking / Lock-free)
        if ( !m_lock.is_writing.compare_exchange_weak(expected, true) )
        {
            return false;
        }

        // 排他制御 (Blocking / Lock-free)
        while ( m_lock.readers_cnt.load() > 0 )
        {
            continue; // spinning
        }

        m_acquired = true;

        return true;
    }

    inline void __stdcall release() noexcept
    {
        if ( m_acquired )
        {
            m_lock.is_writing = false;
            m_acquired = false;
        }
    }

private:
    Lock& m_lock;
    bool m_acquired{ false };

private:
    WriteLock(const WriteLock&) = delete;
    WriteLock(WriteLock&&) = delete;
    WriteLock& operator =(const WriteLock&) = delete;
    WriteLock& operator =(WriteLock&&) = delete;
};

//---------------------------------------------------------------------------//

} // namespace RWLock;

//---------------------------------------------------------------------------//

} //namespace tapetums

//---------------------------------------------------------------------------//

// Lock.hpp
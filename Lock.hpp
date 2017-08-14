#pragma once

//---------------------------------------------------------------------------//
//
// Lock.hpp
//  Lock の RAII クラス
//   Copyright (C) 2015-2017 tapetums
//
//---------------------------------------------------------------------------//

#include <windows.h>

//---------------------------------------------------------------------------//
// Forward Declarations
//---------------------------------------------------------------------------//

namespace tapetums
{
    namespace CS
    {
        class Lock;
        class LockGuard;
    }

    namespace SRWL
    {
        class Lock;
        class ReadGuard;
        class WriteGuard;
    }
}

//---------------------------------------------------------------------------//
// Critical Section
//---------------------------------------------------------------------------//

class tapetums::CS::Lock final
{
private:
    CRITICAL_SECTION cs;

public:
    Lock()  noexcept { ::InitializeCriticalSection(&cs); }
    ~Lock() noexcept { ::DeleteCriticalSection(&cs); }

    Lock(const Lock&) = delete;
    Lock& operator =(const Lock&) = delete;

    Lock(Lock&& rhs) noexcept = default;
    Lock& operator=(Lock&& rhs) noexcept = default;

public:
    inline void enter() noexcept { ::EnterCriticalSection(&cs); }
    inline void leave() noexcept { ::LeaveCriticalSection(&cs); }
};

//---------------------------------------------------------------------------//

class tapetums::CS::LockGuard final
{
private:
    Lock& m_lock;

public:
    LockGuard() = delete;

    LockGuard(const LockGuard&) = delete;
    LockGuard& operator =(const LockGuard&) = delete;

    LockGuard(LockGuard&& rhs) noexcept = delete;
    LockGuard& operator=(LockGuard&& rhs) noexcept = delete;

    explicit LockGuard(Lock& lock) noexcept : m_lock(lock) { m_lock.enter(); }

    ~LockGuard() noexcept { m_lock.leave(); }
};

//---------------------------------------------------------------------------//
// Slim Reader/Writer Lock
//---------------------------------------------------------------------------//

class tapetums::SRWL::Lock final
{
private:
    SRWLOCK m_srwl;

public:
    Lock() noexcept { ::InitializeSRWLock(&m_srwl); }

    Lock(const Lock&) = delete;
    Lock& operator =(const Lock&) = delete;

    Lock(Lock&&) noexcept = default;
    Lock& operator =(Lock&&) noexcept = default;

    ~Lock() noexcept = default;

public:
    auto operator &() const noexcept { return &m_srwl; }
    auto operator &() noexcept       { return &m_srwl; }

public:
    bool read_lock() noexcept
    {
        return ::TryAcquireSRWLockShared(&m_srwl) ? true : false;
    }

    void read_lock_with_blocking() noexcept
    {
        ::AcquireSRWLockShared(&m_srwl);
    }

    void read_unlock() noexcept
    {
        ::ReleaseSRWLockShared(&m_srwl);
    }

    bool write_lock() noexcept
    {
        return ::TryAcquireSRWLockExclusive(&m_srwl) ? true : false;
    }

    void write_lock_with_blocking() noexcept
    {
        ::AcquireSRWLockExclusive(&m_srwl);
    }

    void write_unlock() noexcept
    {
        ::ReleaseSRWLockExclusive(&m_srwl);
    }
};

//---------------------------------------------------------------------------//

class tapetums::SRWL::ReadGuard final
{
private:
    Lock& m_lock;
    bool  m_acquired { false };

public:
    ReadGuard() noexcept = delete;

    ReadGuard(const ReadGuard&) = delete;
    ReadGuard& operator =(const ReadGuard&) = delete;

    ReadGuard(ReadGuard&&) noexcept = delete;
    ReadGuard& operator =(ReadGuard&&) noexcept = delete;

    explicit ReadGuard(Lock& lock) : m_lock(lock) { }

    ~ReadGuard() noexcept { release(); }

public:
    bool is_acquired() const noexcept { return m_acquired; }

public:
    bool acquire() noexcept
    {
        if ( is_acquired() )
        {
            return true;
        }

        if ( ! m_lock.read_lock() )
        {
            return false;
        }

        m_acquired = true;
        return true;
    }

    void acquire_with_blocking() noexcept
    {
        if ( m_acquired )
        {
            return;
        }

        m_lock.read_lock_with_blocking();

        m_acquired = true;
    }

    void release() noexcept
    {
        if ( m_acquired )
        {
            m_lock.read_unlock();
            m_acquired = false;
        }
    }
};

//---------------------------------------------------------------------------//

class tapetums::SRWL::WriteGuard final
{
private:
    Lock& m_lock;
    bool  m_acquired { false };

public:
    WriteGuard() noexcept = delete;

    WriteGuard(const WriteGuard&) = delete;
    WriteGuard& operator =(const WriteGuard&) = delete;

    WriteGuard(WriteGuard&&) = delete;
    WriteGuard& operator =(WriteGuard&&) = delete;

    explicit WriteGuard(Lock& lock) : m_lock(lock) { }

    ~WriteGuard() noexcept { release(); }

public:
    bool is_acquired() const noexcept { return m_acquired; }

public:
    bool acquire() noexcept
    {
        if ( m_acquired )
        {
            return true;
        }

        if ( ! m_lock.write_lock() )
        {
            return false;
        }

        m_acquired = true;
        return true;
    }

    void acquire_with_blocking() noexcept
    {
        if ( m_acquired )
        {
            return;
        }

        m_lock.write_lock_with_blocking();

        m_acquired = true;
    }

    void release() noexcept
    {
        if ( m_acquired )
        {
            m_lock.write_unlock();
            m_acquired = false;
        }
    }
};

//---------------------------------------------------------------------------//

// Lock.hpp
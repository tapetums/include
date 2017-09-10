#pragma once

//---------------------------------------------------------------------------//
//
// Lock.hpp
//  RAII classes for lock objects
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
    Lock()  { ::InitializeCriticalSection(&cs); }
    ~Lock() { ::DeleteCriticalSection(&cs); }

    Lock(const Lock&)             = delete;
    Lock& operator =(const Lock&) = delete;

    Lock(Lock&& rhs)            noexcept = default;
    Lock& operator=(Lock&& rhs) noexcept = default;

public:
    void enter() { ::EnterCriticalSection(&cs); }
    void leave() { ::LeaveCriticalSection(&cs); }
};

//---------------------------------------------------------------------------//

class tapetums::CS::LockGuard final
{
private:
    Lock& m_lock;

public:
    LockGuard() = delete;

    LockGuard(const LockGuard&)             = delete;
    LockGuard& operator =(const LockGuard&) = delete;

    LockGuard(LockGuard&& rhs)            noexcept = delete;
    LockGuard& operator=(LockGuard&& rhs) noexcept = delete;

    explicit LockGuard(Lock& lock) : m_lock(lock) { m_lock.enter(); }

    ~LockGuard() { m_lock.leave(); }
};

//---------------------------------------------------------------------------//
// Slim Reader/Writer Lock
//---------------------------------------------------------------------------//

class tapetums::SRWL::Lock final
{
private:
    SRWLOCK m_srwl;

public:
    Lock() { ::InitializeSRWLock(&m_srwl); }

    Lock(const Lock&)             = delete;
    Lock& operator =(const Lock&) = delete;

    Lock(Lock&&)             noexcept = default;
    Lock& operator =(Lock&&) noexcept = default;

    ~Lock() noexcept = default;

public:
    auto operator &() const noexcept { return &m_srwl; }
    auto operator &() noexcept       { return &m_srwl; }

public:
    bool read_lock()
    {
        return ::TryAcquireSRWLockShared(&m_srwl) ? true : false;
    }

    void read_lock_with_blocking()
    {
        ::AcquireSRWLockShared(&m_srwl);
    }

    void read_unlock()
    {
        ::ReleaseSRWLockShared(&m_srwl);
    }

    bool write_lock()
    {
        return ::TryAcquireSRWLockExclusive(&m_srwl) ? true : false;
    }

    void write_lock_with_blocking()
    {
        ::AcquireSRWLockExclusive(&m_srwl);
    }

    void write_unlock()
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

    ReadGuard(const ReadGuard&)             = delete;
    ReadGuard& operator =(const ReadGuard&) = delete;

    ReadGuard(ReadGuard&&)             noexcept = delete;
    ReadGuard& operator =(ReadGuard&&) noexcept = delete;

    explicit ReadGuard(Lock& lock) noexcept : m_lock(lock) { }

    ~ReadGuard() { release(); }

public:
    bool is_acquired() const noexcept { return m_acquired; }

public:
    bool acquire()
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

    void acquire_with_blocking()
    {
        if ( m_acquired )
        {
            return;
        }

        m_lock.read_lock_with_blocking();

        m_acquired = true;
    }

    void release()
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

    WriteGuard(const WriteGuard&)             = delete;
    WriteGuard& operator =(const WriteGuard&) = delete;

    WriteGuard(WriteGuard&&)             = delete;
    WriteGuard& operator =(WriteGuard&&) = delete;

    explicit WriteGuard(Lock& lock) noexcept : m_lock(lock) { }

    ~WriteGuard() { release(); }

public:
    bool is_acquired() const noexcept { return m_acquired; }

public:
    bool acquire()
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

    void acquire_with_blocking()
    {
        if ( m_acquired )
        {
            return;
        }

        m_lock.write_lock_with_blocking();

        m_acquired = true;
    }

    void release()
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
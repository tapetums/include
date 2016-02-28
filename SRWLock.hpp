#pragma once

//---------------------------------------------------------------------------//
//
// SRWLock.hpp
//  Slim Reader/Writer Lock の RAII クラス
//   Copyright (C) 2014-2015 tapetums
//
//---------------------------------------------------------------------------//

#include <windows.h>

//---------------------------------------------------------------------------//

namespace SRWLock {

//---------------------------------------------------------------------------//

struct LockHolder final
{
private:
    SRWLOCK m_srwl;
    bool m_is_rlocked = false;
    bool m_is_wlocked = false;

public:
    LockHolder()
    {
        ::InitializeSRWLock(&m_srwl);
    }

    LockHolder(LockHolder&& rhs)
    {
        m_srwl     = rhs.m_srwl;
        rhs.m_srwl = SRWLOCK();
    }

    LockHolder& operator =(LockHolder&& rhs)
    {
        m_srwl     = rhs.m_srwl;
        rhs.m_srwl = SRWLOCK();

        return *this;
    }

    ~LockHolder()
    {
    }

public:
    bool read_lock()
    {
        if ( ::TryAcquireSRWLockShared(&m_srwl) )
        {
            m_is_rlocked = true;
            return true;
        }
        else
        {
            return false;
        }
    }

    void read_unlock()
    {
        ::ReleaseSRWLockShared(&m_srwl);
        m_is_rlocked = false;
    }

    bool write_lock()
    {
        if ( ::TryAcquireSRWLockExclusive(&m_srwl) )
        {
            m_is_wlocked = true;
            return true;
        }
        else
        {
            return false;
        }
    }

    void write_unlock()
    {
        ::ReleaseSRWLockExclusive(&m_srwl);
        m_is_wlocked = false;
    }

public:
    SRWLOCK* ptr() const
    {
        return (SRWLOCK*)&m_srwl;
    }

    bool is_rlocked() const
    {
        return m_is_rlocked;
    }

    bool is_wlocked() const
    {
        return m_is_wlocked;
    }

private:
    LockHolder(const LockHolder&)             = delete;
    LockHolder& operator =(const LockHolder&) = delete;
};

//---------------------------------------------------------------------------//

struct ReadLock final
{
private:
    LockHolder& m_holder;

public:
    ReadLock(LockHolder& holder) : m_holder(holder)
    {
    }

    ~ReadLock()
    {
        unlock();
    }

public:
    bool lock()
    {
        return m_holder.read_lock();
    }

    void unlock()
    {
        if ( m_holder.is_rlocked() ) m_holder.read_unlock();
    }

private:
    ReadLock(const ReadLock&)             = delete;
    ReadLock(ReadLock&&)                  = delete;
    ReadLock& operator =(const ReadLock&) = delete;
    ReadLock& operator =(ReadLock&&)      = delete;
};

//---------------------------------------------------------------------------//

struct WriteLock final
{
private:
    LockHolder& m_holder;

public:
    WriteLock(LockHolder& holder) : m_holder(holder)
    {
    }

    ~WriteLock()
    {
        unlock();
    }

public:
    bool lock()
    {
        return m_holder.write_lock();
    }

    void unlock()
    {
        if ( m_holder.is_wlocked() ) m_holder.write_unlock();
    }

private:
    WriteLock(const WriteLock&)             = delete;
    WriteLock(WriteLock&&)                  = delete;
    WriteLock& operator =(const WriteLock&) = delete;
    WriteLock& operator =(WriteLock&&)      = delete;
};

//---------------------------------------------------------------------------//

} // namespace SRWLock

//---------------------------------------------------------------------------//

// SRWLock.hpp
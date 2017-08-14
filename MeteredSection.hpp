#pragma once

//---------------------------------------------------------------------------//
//
// MeteredSection.hpp
//  Original Author: Dan Chou
//  Description: Defines the metered section synchronization object
//  Bug Fixed by: しゅう
//  Bug Fixed by: JVR
//  Modified  by: tapetums
//
//  See Also:
//   http://msdn.microsoft.com/en-us/library/ms810428.aspx
//   http://msdn.microsoft.com/ja-jp/library/cc429052.aspx
//   http://anpcf.blog71.fc2.com/blog-entry-7.html
//   http://s-project.appspot.com/notes_vc.html
//   http://jibaravr.blog51.fc2.com/blog-category-19.html
//
//---------------------------------------------------------------------------//

#include <utility>

#include <windows.h>
#include <strsafe.h>

//---------------------------------------------------------------------------//
//
// Class
//
//---------------------------------------------------------------------------//

class MeteredSection final
{
    static constexpr size_t MAX_METSECT_NAMELEN = 128;

private:
    struct METSECT_SHARED_INFO
    {
        BOOL fInitialized;    // Is the metered section initialized?
        LONG lSpinLock;       // Used to gain access to this structure
        LONG lThreadsWaiting; // Count of threads waiting
        LONG lAvailableCount; // Available resource count
        LONG lMaximumCount;   // Maximum resource count
    };

    struct METERED_SECTION
    {
        HANDLE hEvent;   // Handle to a kernel event object
        HANDLE hFileMap; // Handle to memory mapped file
        METSECT_SHARED_INFO* lpSharedInfo;
    };

private:
    METSECT_SHARED_INFO m_met_info;
    METERED_SECTION     m_met_sect;

public:
    MeteredSection() noexcept { m_met_sect.lpSharedInfo = nullptr; }

    MeteredSection(const MeteredSection&) = delete;
    MeteredSection& operator =(const MeteredSection&) = delete;

    MeteredSection(MeteredSection&&) noexcept = default;
    MeteredSection& operator =(MeteredSection&&) noexcept = default;

    ~MeteredSection() { Close(); }

public:
    BOOL initialized    () const noexcept { return m_met_sect.lpSharedInfo ? m_met_sect.lpSharedInfo->fInitialized    : FALSE; }
    LONG threads_waiting() const noexcept { return m_met_sect.lpSharedInfo ? m_met_sect.lpSharedInfo->lThreadsWaiting : 0; }
    LONG available_count() const noexcept { return m_met_sect.lpSharedInfo ? m_met_sect.lpSharedInfo->lAvailableCount : 0; }
    LONG maximum_count  () const noexcept { return m_met_sect.lpSharedInfo ? m_met_sect.lpSharedInfo->lMaximumCount   : 0; }

public:
    bool Create(LONG lInitialCount, LONG lMaximumCount, LPCSTR  lpName);
    bool Create(LONG lInitialCount, LONG lMaximumCount, LPCWSTR lpName);

    bool Open(LPCSTR  lpName);
    bool Open(LPCWSTR lpName);

    DWORD Enter(DWORD dwMilliseconds);
    bool  Leave(LONG lReleaseCount, LONG* lpPreviousCount = nullptr);
    void  Close();

private:
    bool Initialize     (LONG lInitialCount, LONG lMaximumCount, LPCWSTR lpName, BOOL bOpenOnly);
    bool CreateSectEvent(LPCWSTR lpName, BOOL bOpenOnly);
    bool CreateFileView (LONG lInitialCount, LONG lMaximumCount, LPCWSTR lpName, BOOL bOpenOnly);

    void GetLock    ();
    void ReleaseLock();
};

//---------------------------------------------------------------------------//
//
// Interface functions
//
//---------------------------------------------------------------------------//

inline bool MeteredSection::Create
(
    LONG   lInitialCount,
    LONG   lMaximumCount,
    LPCSTR lpName
)
{
    WCHAR lpNameW[MAX_PATH];

    // Convert lpName to UTF-16 from MBCS
    ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpName, -1, lpNameW, MAX_PATH);

    return Create(lInitialCount, lMaximumCount, lpNameW);
}

//---------------------------------------------------------------------------//

inline bool MeteredSection::Create
(
    LONG    lInitialCount,
    LONG    lMaximumCount,
    LPCWSTR lpName
)
{
    // Verify the parameters
    if ( (lMaximumCount < 1)             ||
         (lInitialCount > lMaximumCount) ||
         (lInitialCount < 0)             ||
         ((lpName) && (wcslen(lpName) > MAX_METSECT_NAMELEN)) )
    {
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return false;
    }

    // Initialize it
    if ( ! Initialize(lInitialCount, lMaximumCount, lpName, FALSE) )
    {
        // Metered section failed to initialize
        Close();
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------//

inline bool MeteredSection::Open
(
    LPCSTR lpName
)
{
    WCHAR lpNameW [MAX_PATH];

    // Convert lpName to UTF-16 from MBCS
    ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, lpName, -1, lpNameW, MAX_PATH);

    return Open(lpNameW);
}

//---------------------------------------------------------------------------//

inline bool MeteredSection::Open
(
    LPCWSTR lpName
)
{
    if ( lpName == nullptr ) { return false; }

    if ( ! Initialize(0, 0, lpName, TRUE) )
    {
        // Metered section failed to initialize
        Close();
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------//

inline DWORD MeteredSection::Enter
(
    DWORD dwMilliseconds
)
{
    if ( ! initialized() )
    {
        ::SetLastError(ERROR_NOT_READY);
        return WAIT_FAILED;
    }

    for ( ; ; )
    {
        GetLock();

        // We have access to the metered section, everything we do now will be atomic
        if ( m_met_sect.lpSharedInfo->lAvailableCount >= 1 )
        {
            m_met_sect.lpSharedInfo->lAvailableCount--;
            ReleaseLock();
            return WAIT_OBJECT_0;
        }

        // Couldn't get in. Wait on the event object
        m_met_sect.lpSharedInfo->lThreadsWaiting++;
        ::ResetEvent(m_met_sect.hEvent);

        ReleaseLock();

        if ( WAIT_TIMEOUT == ::WaitForSingleObject(m_met_sect.hEvent, dwMilliseconds) )
        {
            return WAIT_TIMEOUT;
        }
    }
}

//---------------------------------------------------------------------------//

inline bool MeteredSection::Leave
(
    LONG  lReleaseCount,
    LONG* lpPreviousCount
)
{
    if ( ! initialized() )
    {
        ::SetLastError(ERROR_NOT_READY);
        return false;
    }

    GetLock();

    // Save the old value if they want it
    if ( lpPreviousCount )
    {
        *lpPreviousCount = m_met_sect.lpSharedInfo->lAvailableCount;
    }

    // We have access to the metered section, everything we do now will be atomic
    if ( (lReleaseCount < 0) ||
         (m_met_sect.lpSharedInfo->lAvailableCount + lReleaseCount > m_met_sect.lpSharedInfo->lMaximumCount) )
    {
        ReleaseLock();
        ::SetLastError(ERROR_INVALID_PARAMETER);
        return false;
    }

    m_met_sect.lpSharedInfo->lAvailableCount += lReleaseCount;

    // Set the event the appropriate number of times
    lReleaseCount = min(lReleaseCount, m_met_sect.lpSharedInfo->lThreadsWaiting);
    if ( m_met_sect.lpSharedInfo->lThreadsWaiting )
    {
        for ( int iCount = 0; iCount < lReleaseCount; ++iCount )
        {
            m_met_sect.lpSharedInfo->lThreadsWaiting--;
        }

        ReleaseLock();
        ::SetEvent(m_met_sect.hEvent);
    }
    else
    {
        ReleaseLock();
    }

    return true;
}

//---------------------------------------------------------------------------//

void MeteredSection::Close()
{
    if ( ! initialized() ) { return; }

    // Block so that other threads cannot enter anymore
    GetLock();

    // Clean up
    if ( m_met_sect.lpSharedInfo )
    {
        ::UnmapViewOfFile(m_met_sect.lpSharedInfo );
        m_met_sect.lpSharedInfo = nullptr;
    }
    if ( m_met_sect.hFileMap )
    {
        ::CloseHandle(m_met_sect.hFileMap );
        m_met_sect.hFileMap = nullptr;
    }
    if ( m_met_sect.hEvent )
    {
        ::CloseHandle(m_met_sect.hEvent );
        m_met_sect.hEvent = nullptr;
    }
}

//---------------------------------------------------------------------------//

inline bool MeteredSection::Initialize
(
    LONG    lInitialCount,
    LONG    lMaximumCount,
    LPCWSTR lpName,
    BOOL    bOpenOnly
)
{
    if ( initialized() )
    {
        return false; // Already initialized
    }

    m_met_sect.hEvent       = nullptr;
    m_met_sect.hFileMap     = nullptr;
    m_met_sect.lpSharedInfo = nullptr;

    // Try to create the event object
    if ( CreateSectEvent(lpName, bOpenOnly) )
    {
        // Try to create the memory mapped file
        if ( CreateFileView(lInitialCount, lMaximumCount, lpName, bOpenOnly) )
        {
            return true;
        }
    }

    // Error occured, return false so the caller knows to clean up
    return false;
}

//---------------------------------------------------------------------------//

inline bool MeteredSection::CreateSectEvent
(
    LPCWSTR lpName,
    BOOL    bOpenOnly
)
{
    WCHAR sz [MAX_PATH];

    if ( lpName )
    {
        ::StringCchPrintfW(sz, MAX_PATH, L"DKC_MSECT_EVT_%s", lpName);

        if ( bOpenOnly )
        {
            m_met_sect.hEvent = ::OpenEventW(EVENT_ALL_ACCESS, FALSE, sz);
        }
        else
        {
            // Create an auto-reset named event object
            m_met_sect.hEvent = ::CreateEventW(nullptr, FALSE, FALSE, sz);
        }
    }
    else
    {
        // Create an auto-reset unnamed event object
        m_met_sect.hEvent = ::CreateEventW(nullptr, FALSE, FALSE, nullptr);
    }

    return m_met_sect.hEvent ? true : false;
}

//---------------------------------------------------------------------------//

inline bool MeteredSection::CreateFileView
(
    LONG    lInitialCount,
    LONG    lMaximumCount,
    LPCWSTR lpName,
    BOOL    bOpenOnly
)
{
    WCHAR sz [MAX_PATH];
    DWORD dwLastError; 

    if ( lpName )
    {
        ::StringCchPrintfW(sz, MAX_PATH, L"DKC_MSECT_MMF_%s", lpName);

        if ( bOpenOnly )
        {
            m_met_sect.hFileMap = ::OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, sz);
        }
        else
        {
            // Create a named file mapping
            m_met_sect.hFileMap = ::CreateFileMappingW
            (
                INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE,
                0, sizeof(METSECT_SHARED_INFO), sz
            );
        }
    }
    else
    {
        // Create an unnamed file mapping
        m_met_sect.hFileMap = ::CreateFileMappingW
        (
            INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE,
            0, sizeof(METSECT_SHARED_INFO), nullptr
        );
    }

    // Map a view of the file
    if ( m_met_sect.hFileMap )
    {
        dwLastError = ::GetLastError();

        m_met_sect.lpSharedInfo = (METSECT_SHARED_INFO*)::MapViewOfFile
        (
            m_met_sect.hFileMap, FILE_MAP_WRITE, 0, 0, 0
        );
        if ( m_met_sect.lpSharedInfo )
        {
            if ( (! bOpenOnly) && dwLastError != ERROR_ALREADY_EXISTS )
            {
                m_met_sect.lpSharedInfo->lSpinLock       = 0;
                m_met_sect.lpSharedInfo->lThreadsWaiting = 0;
                m_met_sect.lpSharedInfo->lAvailableCount = lInitialCount;
                m_met_sect.lpSharedInfo->lMaximumCount   = lMaximumCount;
                ::InterlockedExchange((LONG*)&(m_met_sect.lpSharedInfo->fInitialized), (LONG)TRUE);
            }
            else
            {    // Already exists; wait for it to be initialized by the creator
                while ( ! m_met_sect.lpSharedInfo->fInitialized ) { ::Sleep(0); }
            }

            return true;
        }
    }

    return false;
}

//---------------------------------------------------------------------------//

inline void MeteredSection::GetLock()
{
    // Spin and get access to the metered section lock
    while ( ::InterlockedExchange(&(m_met_sect.lpSharedInfo->lSpinLock), 1) != 0 )
    {
        ::Sleep(0);
    }
}

//---------------------------------------------------------------------------//

inline void MeteredSection::ReleaseLock()
{
    ::InterlockedExchange(&(m_met_sect.lpSharedInfo->lSpinLock), 0);
}

//---------------------------------------------------------------------------//

// MeteredSection.hpp
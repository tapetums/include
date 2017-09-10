#pragma once

//---------------------------------------------------------------------------//
//
// File.hpp
//  RAII class for Windows files
//   Copyright (C) 2014-2017 tapetums
//
//---------------------------------------------------------------------------//

#include <cstdint>

#include <algorithm>

#include <windows.h>
#include <strsafe.h>

#if defined(DELETE)
  #undef DELETE
#endif

#if defined(max)
  #undef max
#endif
#if defined(min)
  #undef min
#endif

//---------------------------------------------------------------------------//
// Forward Declarations
//---------------------------------------------------------------------------//

namespace tapetums
{
    class File;
}

//---------------------------------------------------------------------------//
// Classes
//---------------------------------------------------------------------------//

class tapetums::File final
{
public:
    enum class ACCESS : DWORD
    {
        UNKNOWN = 0,
        READ    = GENERIC_READ,
        WRITE   = GENERIC_READ | GENERIC_WRITE,
    };

    enum class SHARE : DWORD
    {
        EXCLUSIVE = 0,
        READ      = FILE_SHARE_READ,
        WRITE     = FILE_SHARE_READ | FILE_SHARE_WRITE,
        DELETE    = FILE_SHARE_DELETE,
    };

    enum class OPEN : DWORD
    {
        NEW         = CREATE_NEW,        // Fails   if existing
        OR_TRUNCATE = CREATE_ALWAYS,     // Clears  if existing
        EXISTING    = OPEN_EXISTING,     // Fails   if not existing
        OR_CREATE   = OPEN_ALWAYS,       // Creates if not existing
        TRUNCATE    = TRUNCATE_EXISTING, // Fails   if not existing
    };

    enum class ORIGIN : DWORD
    {
        BEGIN   = FILE_BEGIN,
        CURRENT = FILE_CURRENT,
        END     = FILE_END,
    };

protected:
    HANDLE   m_handle { INVALID_HANDLE_VALUE };
    HANDLE   m_map    { nullptr };
    uint8_t* m_ptr    { nullptr };
    int64_t  m_pos    { 0 };
    int64_t  m_size   { 0 };

public:
    File() = default;
    ~File() { Close(); }

    File(const File&)             = delete;
    File& operator =(const File&) = delete;

    File(File&& rhs)             noexcept { swap(std::move(rhs)); }
    File& operator =(File&& rhs) noexcept { swap(std::move(rhs)); return *this; }

public:
    void swap(File&& rhs);

public:
    auto is_open()   const noexcept { return m_handle != INVALID_HANDLE_VALUE; }
    auto is_mapped() const noexcept { return m_map != nullptr; }
    auto handle()    const noexcept { return m_handle; }
    auto position()  const noexcept { return m_pos; }
    auto pointer()   const noexcept { return m_map ? m_ptr + m_pos : nullptr; }
    auto size()      const noexcept { return m_size; }

public:
    bool    Open        (LPCSTR  lpFileName, ACCESS accessMode, SHARE shareMode, OPEN createMode);
    bool    Open        (LPCWSTR lpFileName, ACCESS accessMode, SHARE shareMode, OPEN createMode);
    bool    Open        (LPCSTR  lpName, ACCESS accessMode);
    bool    Open        (LPCWSTR lpName, ACCESS accessMode);
    void    Close       ();
    bool    Map         (ACCESS accessMode);
    bool    Map         (int64_t size, LPCSTR  lpName, ACCESS accessMode);
    bool    Map         (int64_t size, LPCWSTR lpName, ACCESS accessMode);
    void    UnMap       ();
    size_t  Read        (void* buf, size_t size);
    size_t  Write       (const void* const buf, size_t size);
    int64_t Seek        (int64_t distance, ORIGIN origin = ORIGIN::BEGIN);
    bool    SetEndOfFile();
    bool    Flush       (size_t dwNumberOfBytesToFlush = 0);

    template<typename T>
    size_t Read(T* t) { return Read(t, sizeof(T)); }

    template<typename T>
    size_t Write(const T& t) { return Write((const T* const)&t, sizeof(T)); }
};

//---------------------------------------------------------------------------//
// ムーブコンストラクタ
//---------------------------------------------------------------------------//

inline void tapetums::File::swap(File&& rhs)
{
    if ( this == &rhs ) { return; }

    std::swap(m_handle, rhs.m_handle);
    std::swap(m_map,    rhs.m_map);
    std::swap(m_ptr,    rhs.m_ptr);
    std::swap(m_pos,    rhs.m_pos);
    std::swap(m_size,   rhs.m_size);
}

//---------------------------------------------------------------------------//
// メソッド
//---------------------------------------------------------------------------//

// ファイルを開く (ANSI版)
inline bool tapetums::File::Open
(
    LPCSTR lpFileName,
    ACCESS accessMode,
    SHARE  shareMode,
    OPEN   createMode
)
{
    if ( m_handle != INVALID_HANDLE_VALUE ) { return true; }

    m_handle = ::CreateFileA
    (
        lpFileName, (DWORD)accessMode, (DWORD)shareMode, nullptr,
        (DWORD)createMode, FILE_ATTRIBUTE_NORMAL, nullptr
    );
    if ( m_handle == INVALID_HANDLE_VALUE )
    {
        return false;
    }

    LARGE_INTEGER li;
    ::GetFileSizeEx(m_handle, &li);
    m_size = li.QuadPart;

    return true;
}

//---------------------------------------------------------------------------//

// ファイルを開く (UNICODE版)
inline bool tapetums::File::Open
(
    LPCWSTR lpFileName,
    ACCESS  accessMode,
    SHARE   shareMode,
    OPEN    createMode
)
{
    if ( m_handle != INVALID_HANDLE_VALUE ) { return true; }

    m_handle = ::CreateFileW
    (
        lpFileName, (DWORD)accessMode, (DWORD)shareMode, nullptr,
        (DWORD)createMode, FILE_ATTRIBUTE_NORMAL, nullptr
    );
    if ( m_handle == INVALID_HANDLE_VALUE )
    {
        return false;
    }

    LARGE_INTEGER li;
    ::GetFileSizeEx(m_handle, &li);
    m_size = li.QuadPart;

    return true;
}

//---------------------------------------------------------------------------//

// メモリマップトファイルを開く (ANSI版)
inline bool tapetums::File::Open
(
    LPCSTR lpName, ACCESS accessMode
)
{
    if ( m_map ) { return true; }
    if ( m_handle != INVALID_HANDLE_VALUE ) { return false; }

    m_map = ::OpenFileMappingA
    (
        accessMode == ACCESS::READ ? FILE_MAP_READ : FILE_MAP_WRITE,
        FALSE, lpName
    );
    if ( m_map == nullptr )
    {
        return false;
    }

    m_ptr = (uint8_t*)::MapViewOfFile
    (
        m_map,
        accessMode == ACCESS::READ ? FILE_MAP_READ : FILE_MAP_WRITE,
        0, 0, 0
    );
    if ( m_ptr == nullptr )
    {
        UnMap();
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------//

// メモリマップトファイルを開く (UNICODE版)
inline bool tapetums::File::Open
(
    LPCWSTR lpName, ACCESS accessMode
)
{
    if ( m_map ) { return true; }
    if ( m_handle != INVALID_HANDLE_VALUE ) { return false; }

    m_map = ::OpenFileMappingW
    (
        accessMode == ACCESS::READ ? FILE_MAP_READ : FILE_MAP_WRITE,
        FALSE, lpName
    );
    if ( m_map == nullptr )
    {
        return false;
    }

    m_ptr = (uint8_t*)::MapViewOfFile
    (
        m_map,
        accessMode == ACCESS::READ ? FILE_MAP_READ : FILE_MAP_WRITE,
        0, 0, 0
    );
    if ( m_ptr == nullptr )
    {
        UnMap();
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------//

// ファイルを閉じる
inline void tapetums::File::Close()
{
    UnMap();
    Flush();

    if ( m_handle != INVALID_HANDLE_VALUE )
    {
        ::CloseHandle(m_handle);
        m_handle = INVALID_HANDLE_VALUE;
    }

    m_pos  = 0;
    m_size = 0;
}

//---------------------------------------------------------------------------//

// 既存のファイルをメモリにマップする
inline bool tapetums::File::Map
(
    ACCESS accessMode
)
{
    if ( m_handle == INVALID_HANDLE_VALUE ) { return false; }

    return Map(0, LPCWSTR(nullptr), accessMode);
}

//---------------------------------------------------------------------------//

// メモリマップトファイルを生成する (ANSI版)
inline bool tapetums::File::Map
(
    int64_t size, LPCSTR lpName, ACCESS accessMode
)
{
    if ( m_map ) { return true; }

    LARGE_INTEGER li;
    li.QuadPart = (size > 0) ? size : m_size;
    if ( li.QuadPart == 0 )
    {
        return false;
    }

    m_map = ::CreateFileMappingA
    (
        m_handle, nullptr,
        accessMode == ACCESS::READ ? PAGE_READONLY : PAGE_READWRITE,
        li.HighPart, li.LowPart, lpName
    );
    if ( m_map == nullptr )
    {
        return false;
    }

    m_ptr = (uint8_t*)::MapViewOfFile
    (
        m_map,
        accessMode == ACCESS::READ ? FILE_MAP_READ : FILE_MAP_WRITE,
        0, 0, 0
    );
    if ( m_ptr == nullptr )
    {
        UnMap();
        return false;
    }

    m_size = li.QuadPart;
    return true;
}

//---------------------------------------------------------------------------//

// メモリマップトファイルを生成する (UNICODE版)
inline bool tapetums::File::Map
(
    int64_t size, LPCWSTR lpName, ACCESS accessMode
)
{
    if ( m_map ) { return true; }

    LARGE_INTEGER li;
    li.QuadPart = (size > 0) ? size : m_size;
    if ( li.QuadPart == 0 )
    {
        return false;
    }

    m_map = ::CreateFileMappingW
    (
        m_handle, nullptr,
        accessMode == ACCESS::READ ? PAGE_READONLY : PAGE_READWRITE,
        li.HighPart, li.LowPart, lpName
    );
    if ( m_map == nullptr )
    {
        return false;
    }

    m_ptr = (uint8_t*)::MapViewOfFile
    (
        m_map,
        accessMode == ACCESS::READ ? FILE_MAP_READ : FILE_MAP_WRITE,
        0, 0, 0
    );
    if ( m_ptr == nullptr )
    {
        UnMap();
        return false;
    }

    m_size = li.QuadPart;
    return true;
}

//---------------------------------------------------------------------------//

// メモリマップトファイルを閉じる
inline void tapetums::File::UnMap()
{
    if ( m_ptr )
    {
        ::FlushViewOfFile(m_ptr, 0);
        ::UnmapViewOfFile(m_ptr);
        m_ptr = nullptr;
    }
    if ( m_map )
    {
        ::CloseHandle(m_map);
        m_map = nullptr;
    }
}

//---------------------------------------------------------------------------//

// ファイルもしくはメモリマップトファイルから読み込む
inline size_t tapetums::File::Read
(
    void* buf, size_t size
)
{
    size_t cb { 0 };

    if ( m_map )
    {
        cb = std::min(size, (size_t)m_size - size);
        ::memcpy(buf, m_ptr + m_pos, cb);
    }
    else
    {
        ::ReadFile(m_handle, buf, (DWORD)size, (DWORD*)&cb, nullptr);
    }

    m_pos += cb;

    return cb;
}

//---------------------------------------------------------------------------//

// ファイルもしくはメモリマップトファイルに書き込む
inline size_t tapetums::File::Write
(
    const void* const buf, size_t size
)
{
    size_t cb { 0 };

    if ( m_map )
    {
        cb = std::min(size, (size_t)m_size - size);
        ::memcpy((m_ptr + m_pos), buf, cb);
    }
    else
    {
        ::WriteFile(m_handle, buf, (DWORD)size, (DWORD*)&cb, nullptr);
    }

    m_pos += cb;

    return cb;
}

//---------------------------------------------------------------------------//

// ファイルポインタを移動する
inline int64_t tapetums::File::Seek
(
    int64_t distance, ORIGIN origin
)
{
    if ( m_map )
    {
        if ( origin == ORIGIN::END )
        {
            m_pos = m_size - distance;
        }
        else if ( origin == ORIGIN::CURRENT )
        {
            m_pos += distance;
        }
        else
        {
            m_pos = distance;
        }

        if ( m_pos < 0 )
        {
            m_pos = 0;
        }
        else if ( m_pos >= m_size )
        {
            m_pos = (distance > 0) ? m_size : 0;
        }

        return (intptr_t)m_ptr + m_pos;
    }
    else
    {
        LARGE_INTEGER li;
        li.QuadPart = distance;
        ::SetFilePointerEx(m_handle, li, &li, (DWORD)origin);

        m_pos = li.QuadPart;
        return m_pos;
    }
}

//---------------------------------------------------------------------------//

// ファイルを終端する
inline bool tapetums::File::SetEndOfFile()
{
    if ( m_handle == INVALID_HANDLE_VALUE )
    {
        return true;
    }
    else
    {
        return ::SetEndOfFile(m_handle) ? true : false;
    }
}

//---------------------------------------------------------------------------//

// ファイルもしくはメモリマップトファイルをフラッシュする
inline bool tapetums::File::Flush
(
    size_t dwNumberOfBytesToFlush
)
{
    if ( m_ptr )
    {
        return ::FlushViewOfFile(m_ptr, dwNumberOfBytesToFlush) ? true : false;
    }
    else
    {
        return ::FlushFileBuffers(m_handle) ? true : false;
    }
}

//---------------------------------------------------------------------------//

// File.hpp
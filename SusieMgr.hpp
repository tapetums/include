#pragma once

//---------------------------------------------------------------------------//
//
// SusieMgr.hpp
//  Susie プラグイン管理クラス (C++17)
//   Copyright (C) 2013-2017 tapetums
//
//---------------------------------------------------------------------------//

#if __cplusplus > 201700L
  #include <optional>
#elif 1 // __cplusplus >= 201402L
  #include <experimental/optional>
  namespace std { template<typename T> using optional = experimental::optional<T>; }
#else
  #error No library <optional>
#endif

#include <vector>
#include <stdexcept>
#include <string>
namespace std { using tstring = basic_string<TCHAR>; }

#include "Susie.hpp"

//---------------------------------------------------------------------------//

namespace tapetums
{
    class SusieMgr;
}

//---------------------------------------------------------------------------//

// Susie プラグイン管理クラス
class tapetums::SusieMgr final
{
public:
    using value_type     = Susie;
    using ref_type       = std::optional<value_type&>;
    using const_ref_type = std::optional<const value_type&>;
    using container_type = std::vector<value_type>;

    using iterator               = container_type::iterator;
    using const_iterator         = container_type::const_iterator;
    using reverse_iterator       = container_type::reverse_iterator;
    using const_reverse_iterator = container_type::const_reverse_iterator;

private:
    container_type plugins;

public:
    SusieMgr()  = default;
    ~SusieMgr() = default;

    SusieMgr(const SusieMgr&)             = delete;
    SusieMgr& operator =(const SusieMgr&) = delete;

    SusieMgr(SusieMgr&&)             noexcept = default;
    SusieMgr& operator =(SusieMgr&&) noexcept = default;

    template <typename... Args>
    SusieMgr(Susie&& susie, Args&&... args) { init(susie, args...); }

    template <typename... Args>
    SusieMgr(LPCTSTR path, Args... args)    { init(path, args...); }

public:
    size_t         size()           const noexcept;
    const_ref_type at(size_t index) const noexcept;
    ref_type       at(size_t index) noexcept;

public:
    const_ref_type operator[](size_t index) const noexcept { return at(index); }
    ref_type       operator[](size_t index) noexcept       { return at(index); }

public:
    void append(Susie&& susie);
    bool append(LPCTSTR path);

    void collect_susie(LPCTSTR dir_path, bool serach_in_subdir);

    ref_type acquire(LPCSTR filename,  uint8_t* dw);
    ref_type acquire(LPCWSTR filename, uint8_t* dw);

public:
    iterator begin() noexcept { return plugins.begin(); }
    iterator end()   noexcept { return plugins.end(); }

    const_iterator begin() const noexcept { return plugins.begin(); }
    const_iterator end()   const noexcept { return plugins.end(); }

    const_iterator cbegin() const noexcept { return plugins.cbegin(); }
    const_iterator cend()   const noexcept { return plugins.cend(); }

    reverse_iterator rbegin() noexcept { return plugins.rbegin(); }
    reverse_iterator rend()   noexcept { return plugins.rend(); }

    const_reverse_iterator rbegin() const noexcept { return plugins.rbegin(); }
    const_reverse_iterator rend()   const noexcept { return plugins.rend(); }

    const_reverse_iterator crbegin() const noexcept { return plugins.crbegin(); }
    const_reverse_iterator crend()   const noexcept { return plugins.crend(); }

private:
    void init(Susie&& susie);
    void init(LPCTSTR path);

    template <typename... Args>
    void init(Susie&& susie, Args&&... args);

    template <typename... Args>
    void init(LPCTSTR path, Args... args);
};

//---------------------------------------------------------------------------//
// ctor
//---------------------------------------------------------------------------//

inline void tapetums::SusieMgr::init(Susie&& susie)
{
    append(std::move(susie));
}

//---------------------------------------------------------------------------//

inline void tapetums::SusieMgr::init(LPCTSTR path)
{
    append(path);
}

//---------------------------------------------------------------------------//

template<typename... Args>
inline void tapetums::SusieMgr::init(Susie&& susie, Args&&... args)
{
    append(std::move(susie));

    init(args...);
}

//---------------------------------------------------------------------------//

template<typename... Args>
inline void tapetums::SusieMgr::init(LPCTSTR path, Args... args)
{
    append(path);

    init(args...);
}

//---------------------------------------------------------------------------//
// Accessors
//---------------------------------------------------------------------------//

inline size_t tapetums::SusieMgr::size() const noexcept
{
    return plugins.size();
}

//---------------------------------------------------------------------------//

inline tapetums::SusieMgr::const_ref_type tapetums::SusieMgr::at(size_t index) const noexcept
{
    try
    {
        return const_ref_type(plugins.at(index));
    }
    catch ( std::out_of_range& )
    {
        return const_ref_type();
    }
}

//---------------------------------------------------------------------------//

inline tapetums::SusieMgr::ref_type tapetums::SusieMgr::at(size_t index) noexcept
{
    try
    {
        return ref_type(plugins.at(index));
    }
    catch ( std::out_of_range& )
    {
        return ref_type();
    }
}

//---------------------------------------------------------------------------//
// Methids
//---------------------------------------------------------------------------//

inline void tapetums::SusieMgr::append
(
    Susie&& susie
)
{
    plugins.push_back(std::move(susie));
}

//---------------------------------------------------------------------------//

inline bool tapetums::SusieMgr::append
(
    LPCTSTR path
)
{
    Susie susie;

    if ( susie.Load(path) )
    {
        plugins.push_back(std::move(susie));
        return true;
    }
    else
    {
        return false;
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::SusieMgr::collect_susie
(
    LPCTSTR dir_path, bool serach_in_subdir
)
{
    std::tstring path { dir_path };
    path += TEXT(R"(\*)");

    WIN32_FIND_DATA fd { };
    const auto hFindFile = ::FindFirstFile(path.c_str(), &fd);
    if ( INVALID_HANDLE_VALUE == hFindFile )
    {
        // 指定されたフォルダが見つからなかった
        return;
    }

    // フォルダ内にあるものを列挙する
    do
    {
        // 隠しファイルは飛ばす
        if ( fd.cFileName[0] == '.' )
        {
            continue;
        }
        if ( fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN )
        {
            continue;
        }

        // フルパスを合成
        path = dir_path + std::tstring{ TEXT(R"(\)") } + fd.cFileName;

        if ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
        {
            // フォルダだったら
            if ( serach_in_subdir )
            {
                // サブフォルダを検索
                collect_susie(path.c_str(), serach_in_subdir);
            }
        }
        else
        {
            // DLLファイルだったら
            const auto pos = path.rfind(TEXT("."));
            if ( path.substr(pos) == SPI_EXT )
            {
                // コンポーネントデータベースに記憶
                append(path.c_str());
            }
        }
    }
    while ( ::FindNextFile(hFindFile, &fd) );

    ::FindClose( hFindFile );

    return;
}

//---------------------------------------------------------------------------//

inline tapetums::SusieMgr::ref_type tapetums::SusieMgr::acquire
(
    LPCSTR filename, uint8_t* dw
)
{
    for ( auto&& susie : plugins )
    {
        if ( susie.IsSupportedA(filename, dw) == SPI_SUPPORTED )
        {
            return ref_type(std::ref(susie));
        }
    }

    return ref_type();
}

//---------------------------------------------------------------------------//

inline tapetums::SusieMgr::ref_type tapetums::SusieMgr::acquire
(
    LPCWSTR filename, uint8_t* dw
)
{
    for ( auto&& susie : plugins )
    {
        if ( susie.IsSupportedW(filename, dw) == SPI_SUPPORTED )
        {
            return ref_type(std::ref(susie));
        }
    }

    return ref_type();
}

//---------------------------------------------------------------------------//

// Susie.hpp
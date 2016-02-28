#pragma once

//---------------------------------------------------------------------------//
//
// Susie.hpp
//  Susie プラグイン API 定義ファイル (C++11)
//   Copyright (C) 2013-2016 tapetums
//
//---------------------------------------------------------------------------//
//
// 下記を参考 :
//
// Susie Plug-in Programming Guide
// http://home.netyou.jp/cc/susumu/progSusie.html
//
// Susie Plug-in Specification Rev4+α on Win32
// http://www2f.biglobe.ne.jp/~kana/spi_api/index.html
//
// TORO's Software library
// http://homepage1.nifty.com/toro/slplugin.html
//
// Susie Plug-inプログラミング
// http://www.asahi-net.or.jp/~kh4s-smz/spi/index.html
//
// DでSusieプラグインを使うためのクラス(リンク切れ):
// http://moephp.org/?D%B8%C0%B8%EC#content_1_13
//
//---------------------------------------------------------------------------//

#include <cstdint>

#include <utility>

//---------------------------------------------------------------------------//
//
// 型宣言
//
//---------------------------------------------------------------------------//

#if defined (_WIN32) || defined(WIN32)
  #include <windows.h>
#else
  #include <dlfcn.h>

  struct HWND__;      using HWND      = HWND__*;
  struct HINSTANCE__; using HINSTANCE = HINSTANCE__*;

  using HMODULE = void*;
  using LPSTR   = char*;
  using LPCSTR  = const char*;
  using LPWSTR  = wchar_t*;
  using LPCWSTR = const wchar_t*;

  #if defined(_UNICODE) || defined(UNICODE)
    using TCHAR   = wchar_t;
    using LPCTSTR = LPCWSTR;
  #else
    using TCHAR   = char;
    using LPCTSTR = LPCSTR;
  #endif

  #define WINAPI
#endif

//---------------------------------------------------------------------------//
//
// 定数
//
//---------------------------------------------------------------------------//

// SusieFileInfo::path における最大要素数
static constexpr size_t SPI_MAX_PATH { 200 };

// IsSupported()関数へ渡すデータのサイズ
// 余白部分は <必ず> 0 で埋めること!!
static constexpr size_t SPI_TEST_BUF_SIZE { 2048 };

// Susie plugin の 拡張子
#if INTPTR_MAX == INT64_MAX
  static constexpr auto SPI_EXT = TEXT(".sph");
#elif INTPTR_MAX == INT32_MAX
  static constexpr auto SPI_EXT = TEXT(".spi");
#else
  #error Susie.hpp : Unsupported Environment
#endif

//---------------------------------------------------------------------------//
//
// 列挙体
//
//---------------------------------------------------------------------------//

// Susie エラーコード
enum SPI_RESULT : int32_t
{
    SPI_UNSUPPORTED      =  0, // 非対応のファイル形式
    SPI_SUPPORTED        =  1, // 対応可能なファイル形式

    SPI_NO_FUNCTION      = -1, // その機能はインプリメントされていない
    SPI_ALL_RIGHT        =  0, // 正常終了
    SPI_E_ABORT          =  1, // コールバック関数が非0を返したので展開を中止した
    SPI_NOT_SUPPORT      =  2, // 未知のフォーマット
    SPI_OUT_OF_ORDER     =  3, // データが壊れている
    SPI_NO_MEMORY        =  4, // メモリーが確保出来ない
    SPI_MEMORY_ERROR     =  5, // メモリーエラー
    SPI_FILE_READ_ERROR  =  6, // ファイルリードエラー
    SPI_WINDOW_ERROR     =  7, // 窓が開けない (非公開のエラーコード)
    SPI_OTHER_ERROR      =  8, // 内部エラー
    SPI_FILE_WRITE_ERROR =  9, // 書き込みエラー (非公開のエラーコード)
    SPI_END_OF_FILE      = 10, // ファイル終端 (非公開のエラーコード)
};

//---------------------------------------------------------------------------//

// Susie 追加情報フラグ
enum SPI_FLAG : uint32_t
{
    SPI_INPUT_FILE    = 0,      // 入力はディスクファイル
    SPI_INPUT_MEMORY  = 1,      // 入力はメモリ上のイメージ
    SPI_IGNORE_CASE   = 1 << 7, // ファイル名の大文字小文字を同一視する
    SPI_OUTPUT_FILE   = 0,      // 出力はディスクファイル
    SPI_OUTPUT_MEMORY = 1 << 8, // 出力はメモリ上のイメージ
};

//---------------------------------------------------------------------------//

// Susie 設定ダイアログ 機能コード
enum SPI_FNC_CODE : int32_t
{
    SPI_CONFIGDLG_ABOUT    = 0, // About ダイアログを表示する
    SPI_CONFIGDLG_SETTING  = 1, // 設定ダイアログを表示する
    SPI_CONFIGDLG_RESERVED = 2, // 予約済み
};

//---------------------------------------------------------------------------//
//
// 時間型
//
//---------------------------------------------------------------------------//

// Susie 独自の time_t 型
#if INTPTR_MAX == INT64_MAX
  using susie_time_t = int64_t;
#elif INTPTR_MAX == INT32_MAX
  using susie_time_t = int32_t;
#else
  #error Susie.hpp : Unsupported Environment
#endif

//---------------------------------------------------------------------------//
//
// 構造体
//
//---------------------------------------------------------------------------//

#pragma pack(push, 1)

//---------------------------------------------------------------------------//

// Susie 画像情報
struct SusiePictureInfo
{
    int32_t  left;       // 画像を展開する位置
    int32_t  top;        // 画像を展開する位置
    int32_t  width;      // 画像の幅(pixel)
    int32_t  height;     // 画像の高さ(pixel)
    uint16_t x_density;  // 画素の水平方向密度
    uint16_t y_density;  // 画素の垂直方向密度
    int16_t  colorDepth; // 画素当たりのbit数
  #if INTPTR_MAX == INT64_MAX
    uint8_t  dummy[2];   // アラインメント
  #endif
    void*    hInfo;      // 画像内のテキスト情報
};

//---------------------------------------------------------------------------//

// Susie ファイル情報 (ANSI)
struct SusieFileInfoA
{
    uint8_t      method[8];              // 圧縮法の種類
    size_t       position;               // ファイル上での位置
    size_t       compsize;               // 圧縮されたサイズ
    size_t       filesize;               // 元のファイルサイズ
    susie_time_t timestamp;              // ファイルの更新日時
    char         path[SPI_MAX_PATH];     // 相対パス
    char         filename[SPI_MAX_PATH]; // ファイルネーム
    uint32_t     crc;                    // CRC32
  #if INTPTR_MAX == INT64_MAX
    uint8_t      dummy[4];               // アラインメント
  #endif
};

//---------------------------------------------------------------------------//

// Susie ファイル情報 (Windows UNICODE)
struct SusieFileInfoW
{
    uint8_t      method[8];              // 圧縮法の種類
    size_t       position;               // ファイル上での位置
    size_t       compsize;               // 圧縮されたサイズ
    size_t       filesize;               // 元のファイルサイズ
    susie_time_t timestamp;              // ファイルの更新日時
    wchar_t      path[SPI_MAX_PATH];     // 相対パス
    wchar_t      filename[SPI_MAX_PATH]; // ファイルネーム
    uint32_t     crc;                    // CRC32
  #if INTPTR_MAX == INT64_MAX
    uint8_t      dummy[4];               // アラインメント
  #endif
};

//---------------------------------------------------------------------------//

#pragma pack(pop)

//---------------------------------------------------------------------------//
//
// 関数ポインタ宣言
//
//---------------------------------------------------------------------------//

#define SPI_FUNC SPI_RESULT (WINAPI *)

extern "C"
{
    // コールバック関数
    using SUSIE_PROGRESS = SPI_FUNC(int32_t nNum, int32_t nDenom, intptr_t lData);

    // 共通関数
    using SPI_GetPluginInfoA   = SPI_FUNC(int32_t, LPSTR,  int32_t);
    using SPI_GetPluginInfoW   = SPI_FUNC(int32_t, LPWSTR, int32_t);
    using SPI_IsSupportedA     = SPI_FUNC(LPCSTR,  uint8_t*);
    using SPI_IsSupportedW     = SPI_FUNC(LPCWSTR, uint8_t*);

    // '00IN'の関数
    using SPI_GetPictureInfoA  = SPI_FUNC(LPCSTR,  size_t, SPI_FLAG, SusiePictureInfo*);
    using SPI_GetPictureInfoW  = SPI_FUNC(LPCWSTR, size_t, SPI_FLAG, SusiePictureInfo*);
    using SPI_GetPictureA      = SPI_FUNC(LPCSTR,  size_t, SPI_FLAG, void**, void**, SUSIE_PROGRESS, intptr_t);
    using SPI_GetPictureW      = SPI_FUNC(LPCWSTR, size_t, SPI_FLAG, void**, void**, SUSIE_PROGRESS, intptr_t);
    using SPI_GetPreviewA      = SPI_FUNC(LPCSTR,  size_t, SPI_FLAG, void**, void**, SUSIE_PROGRESS, intptr_t);
    using SPI_GetPreviewW      = SPI_FUNC(LPCWSTR, size_t, SPI_FLAG, void**, void**, SUSIE_PROGRESS, intptr_t);

    // '00AM'の関数
    using SPI_GetArchiveInfoA  = SPI_FUNC(LPCSTR,  size_t, SPI_FLAG, HLOCAL*);
    using SPI_GetArchiveInfoW  = SPI_FUNC(LPCWSTR, size_t, SPI_FLAG, HLOCAL*);
    using SPI_GetFileInfoA     = SPI_FUNC(LPCSTR,  size_t, LPCSTR,  SPI_FLAG, SusieFileInfoA*);
    using SPI_GetFileInfoW     = SPI_FUNC(LPCWSTR, size_t, LPCWSTR, SPI_FLAG, SusieFileInfoW*);
    using SPI_GetFileA         = SPI_FUNC(LPCSTR,  size_t, LPSTR,  SPI_FLAG, SUSIE_PROGRESS, intptr_t);
    using SPI_GetFileW         = SPI_FUNC(LPCWSTR, size_t, LPWSTR, SPI_FLAG, SUSIE_PROGRESS, intptr_t);

    // オプション関数（Susie v0.40 以降）
    using SPI_ConfigurationDlg = SPI_FUNC(HWND, SPI_FNC_CODE);

    // 拡張子で保存形式を決定
    using SPI_CreatePictureA   = SPI_FUNC(LPCSTR,  SPI_FLAG, void**, void**, SusiePictureInfo*, SUSIE_PROGRESS, intptr_t);
    using SPI_CreatePictureW   = SPI_FUNC(LPCWSTR, SPI_FLAG, void**, void**, SusiePictureInfo*, SUSIE_PROGRESS, intptr_t);
}

//---------------------------------------------------------------------------//
//
// Susie 外部公開関数
//
//---------------------------------------------------------------------------//

#if defined (_WIN32) || defined(WIN32)
  #define SUSIE_EXPORT extern "C" __declspec(dllexport) SPI_RESULT WINAPI
#else
  #define SUSIE_EXPORT extern "C" SPI_RESULT
#endif

SUSIE_EXPORT GetPluginInfo (int32_t infono, LPSTR  buf, int32_t buflen);
SUSIE_EXPORT GetPluginInfoW(int32_t infono, LPWSTR buf, int32_t buflen);

SUSIE_EXPORT IsSupported (LPCSTR  filename, void* dw);
SUSIE_EXPORT IsSupportedW(LPCWSTR filename, void* dw);

SUSIE_EXPORT GetPictureInfo (LPCSTR  buf, size_t len, SPI_FLAG flag, SusiePictureInfo* lpInfo);
SUSIE_EXPORT GetPictureInfoW(LPCWSTR buf, size_t len, SPI_FLAG flag, SusiePictureInfo* lpInfo);

SUSIE_EXPORT GetPicture (LPCSTR  buf, size_t len, SPI_FLAG flag, void** pHBInfo, void** pHBm, SUSIE_PROGRESS progressCallback, intptr_t lData);
SUSIE_EXPORT GetPictureW(LPCWSTR buf, size_t len, SPI_FLAG flag, void** pHBInfo, void** pHBm, SUSIE_PROGRESS progressCallback, intptr_t lData);

SUSIE_EXPORT GetPreview (LPCSTR  buf, size_t len, SPI_FLAG flag, void** pHBInfo, void** pHBm, SUSIE_PROGRESS progressCallback, intptr_t lData);
SUSIE_EXPORT GetPreviewW(LPCWSTR buf, size_t len, SPI_FLAG flag, void** pHBInfo, void** pHBm, SUSIE_PROGRESS progressCallback, intptr_t lData);

SUSIE_EXPORT GetArchiveInfo (LPCSTR  buf, size_t len, SPI_FLAG flag, HLOCAL* lphInf);
SUSIE_EXPORT GetArchiveInfoW(LPCWSTR buf, size_t len, SPI_FLAG flag, HLOCAL* lphInf);

SUSIE_EXPORT GetFileInfo (LPCSTR  buf, size_t len, LPCSTR  filename, SPI_FLAG flag, SusieFileInfoA* lpInfo);
SUSIE_EXPORT GetFileInfoW(LPCWSTR buf, size_t len, LPCWSTR filename, SPI_FLAG flag, SusieFileInfoW* lpInfo);

SUSIE_EXPORT GetFile (LPCSTR  src, size_t len, LPSTR  dest, SPI_FLAG flag, SUSIE_PROGRESS progressCallback, intptr_t lData);
SUSIE_EXPORT GetFileW(LPCWSTR src, size_t len, LPWSTR dest, SPI_FLAG flag, SUSIE_PROGRESS progressCallback, intptr_t lData);

SUSIE_EXPORT ConfigurationDlg(HWND parent, SPI_FNC_CODE fnc);

SUSIE_EXPORT CreatePicture (LPCSTR  filepath, SPI_FLAG flag, void** pHBInfo, void** pHBm, SusiePictureInfo* lpInfo, SUSIE_PROGRESS progressCallback, intptr_t lData);
SUSIE_EXPORT CreatePictureW(LPCWSTR filepath, SPI_FLAG flag, void** pHBInfo, void** pHBm, SusiePictureInfo* lpInfo, SUSIE_PROGRESS progressCallback, intptr_t lData);

//---------------------------------------------------------------------------//
//
// Susie コールバック用ダミー関数
// （プラグインの中には progressCallback に nullptr を渡すと落ちるものがあるため）
//
//---------------------------------------------------------------------------//

namespace
{
    extern "C" constexpr SPI_RESULT WINAPI SusieCallbackDummy
    (
        int32_t, int32_t, intptr_t
    )
    {
        return SPI_ALL_RIGHT;
    }
}

//---------------------------------------------------------------------------//
//
// C++ クラス
//
//---------------------------------------------------------------------------//

namespace tapetums
{
    class Susie;
}

//---------------------------------------------------------------------------//

// Susie プラグインのラッパークラス
class tapetums::Susie
{
public:
    Susie() = default;
    ~Susie() { Free(); }

    Susie(const Susie&)             = delete;
    Susie& operator =(const Susie&) = delete;

    Susie(Susie&&)             noexcept;
    Susie& operator =(Susie&&) noexcept;

    explicit Susie(LPCTSTR path) { Load(path); }

public:
    constexpr LPCTSTR path() const noexcept { return path_; }

public:
    bool Load(LPCTSTR path);
    void Free();

    SPI_RESULT GetPluginInfoA(int32_t infono, LPSTR  buf, int32_t buflen);
    SPI_RESULT GetPluginInfoW(int32_t infono, LPWSTR buf, int32_t buflen);
    SPI_RESULT IsSupportedA  (LPCSTR  filename, uint8_t* dw);
    SPI_RESULT IsSupportedW  (LPCWSTR filename, uint8_t* dw);

    SPI_RESULT GetPictureInfoA(LPCSTR  buf, size_t len, SPI_FLAG flag, SusiePictureInfo* lpInfo);
    SPI_RESULT GetPictureInfoW(LPCWSTR buf, size_t len, SPI_FLAG flag, SusiePictureInfo* lpInfo);
    SPI_RESULT GetPictureA    (LPCSTR  buf, size_t len, SPI_FLAG flag, void** pHBInfo, void** pHBm, SUSIE_PROGRESS progressCallback, intptr_t lData);
    SPI_RESULT GetPictureW    (LPCWSTR buf, size_t len, SPI_FLAG flag, void** pHBInfo, void** pHBm, SUSIE_PROGRESS progressCallback, intptr_t lData);
    SPI_RESULT GetPreviewA    (LPCSTR  buf, size_t len, SPI_FLAG flag, void** pHBInfo, void** pHBm, SUSIE_PROGRESS progressCallback, intptr_t lData);
    SPI_RESULT GetPreviewW    (LPCWSTR buf, size_t len, SPI_FLAG flag, void** pHBInfo, void** pHBm, SUSIE_PROGRESS progressCallback, intptr_t lData);

    SPI_RESULT GetArchiveInfoA(LPCSTR  buf, size_t len, SPI_FLAG flag, HLOCAL* lphInf);
    SPI_RESULT GetArchiveInfoW(LPCWSTR buf, size_t len, SPI_FLAG flag, HLOCAL* lphInf);
    SPI_RESULT GetFileInfoA   (LPCSTR  buf, size_t len, LPCSTR  filename, SPI_FLAG flag, SusieFileInfoA* lpInfo);
    SPI_RESULT GetFileInfoW   (LPCWSTR buf, size_t len, LPCWSTR filename, SPI_FLAG flag, SusieFileInfoW* lpInfo);
    SPI_RESULT GetFileA       (LPCSTR  src, size_t len, LPSTR  dest, SPI_FLAG flag, SUSIE_PROGRESS progressCallback, intptr_t lData);
    SPI_RESULT GetFileW       (LPCWSTR src, size_t len, LPWSTR dest, SPI_FLAG flag, SUSIE_PROGRESS progressCallback, intptr_t lData);

    SPI_RESULT ConfigurationDlg(HWND parent, SPI_FNC_CODE fnc);

    SPI_RESULT CreatePictureA(LPCSTR  filepath, SPI_FLAG flag, void** pHBInfo, void** pHBm, SusiePictureInfo* lpInfo, SUSIE_PROGRESS progressCallback, intptr_t lData);
    SPI_RESULT CreatePictureW(LPCWSTR filepath, SPI_FLAG flag, void** pHBInfo, void** pHBm, SusiePictureInfo* lpInfo, SUSIE_PROGRESS progressCallback, intptr_t lData);

private:
    struct API
    {
        SPI_GetPluginInfoA   GetPluginInfoA   { nullptr };
        SPI_GetPluginInfoW   GetPluginInfoW   { nullptr };
        SPI_IsSupportedA     IsSupportedA     { nullptr };
        SPI_IsSupportedW     IsSupportedW     { nullptr };
        SPI_GetPictureInfoA  GetPictureInfoA  { nullptr };
        SPI_GetPictureInfoW  GetPictureInfoW  { nullptr };
        SPI_GetPictureA      GetPictureA      { nullptr };
        SPI_GetPictureW      GetPictureW      { nullptr };
        SPI_GetPreviewA      GetPreviewA      { nullptr };
        SPI_GetPreviewW      GetPreviewW      { nullptr };
        SPI_GetArchiveInfoA  GetArchiveInfoA  { nullptr };
        SPI_GetArchiveInfoW  GetArchiveInfoW  { nullptr };
        SPI_GetFileInfoA     GetFileInfoA     { nullptr };
        SPI_GetFileInfoW     GetFileInfoW     { nullptr };
        SPI_GetFileA         GetFileA         { nullptr };
        SPI_GetFileW         GetFileW         { nullptr };
        SPI_ConfigurationDlg ConfigurationDlg { nullptr };
        SPI_CreatePictureA   CreatePictureA   { nullptr };
        SPI_CreatePictureW   CreatePictureW   { nullptr };
    };

    HMODULE handle              { nullptr };
    TCHAR   path_[SPI_MAX_PATH] { };
    API     api                 { };
};

//---------------------------------------------------------------------------//
//
// 実装
//
//---------------------------------------------------------------------------//

inline tapetums::Susie::Susie(Susie&& rhs) noexcept
{
    std::swap(handle, rhs.handle);
    std::swap(path_,  rhs.path_);
    std::swap(api,    rhs.api);
}

//---------------------------------------------------------------------------//

inline tapetums::Susie& tapetums::Susie::operator =(Susie&& rhs) noexcept
{
    std::swap(handle, rhs.handle);
    std::swap(path_,  rhs.path_);
    std::swap(api,    rhs.api);

    return *this;
}

//---------------------------------------------------------------------------//

inline bool tapetums::Susie::Load(LPCTSTR path)
{
    if ( handle ) { return true; }

  #if defined (_WIN32) || defined(WIN32)
    #define dlopen(path, y) ::LoadLibraryEx(path, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
    #define dlsym(handle, proc_name) ::GetProcAddress(handle, proc_name)
  #endif

    // DLLの読み込み
    handle = dlopen(path, RTLD_LAZY);
    if ( nullptr == handle )
    {
        return false;
    }

    // DLLのフルパスを取得
  #if defined (_WIN32) || defined(WIN32)
    ::GetModuleFileName(handle, path_, SPI_MAX_PATH);
  #else
    strncpy(path_, path, SPI_MAX_PATH);
    path_[SPI_MAX_PATH - 1] = '\0';
  #endif

    api.GetPluginInfoA   = (SPI_GetPluginInfoA)  dlsym(handle, "GetPluginInfo");
    api.GetPluginInfoW   = (SPI_GetPluginInfoW)  dlsym(handle, "GetPluginInfoW");
    if ( nullptr == api.GetPluginInfoA && nullptr == api.GetPluginInfoW )
    {
        Free(); return false;
    }

    api.IsSupportedA     = (SPI_IsSupportedA)    dlsym(handle, "IsSupported");
    api.IsSupportedW     = (SPI_IsSupportedW)    dlsym(handle, "IsSupportedW");
    if ( nullptr == api.IsSupportedA && nullptr == api.IsSupportedW )
    {
        Free(); return false;
    }

    api.GetPictureInfoA  = (SPI_GetPictureInfoA) dlsym(handle, "GetPictureInfo");
    api.GetPictureInfoW  = (SPI_GetPictureInfoW) dlsym(handle, "GetPictureInfoW");

    api.GetPictureA      = (SPI_GetPictureA)     dlsym(handle, "GetPicture");
    api.GetPictureW      = (SPI_GetPictureW)     dlsym(handle, "GetPictureW");

    api.GetPreviewA      = (SPI_GetPreviewA)     dlsym(handle, "GetPreview");
    api.GetPreviewW      = (SPI_GetPreviewW)     dlsym(handle, "GetPreviewW");

    api.GetArchiveInfoA  = (SPI_GetArchiveInfoA) dlsym(handle, "GetArchiveInfo");
    api.GetArchiveInfoW  = (SPI_GetArchiveInfoW) dlsym(handle, "GetArchiveInfoW");
                                                 
    api.GetFileInfoA     = (SPI_GetFileInfoA)    dlsym(handle, "GetFileInfo");
    api.GetFileInfoW     = (SPI_GetFileInfoW)    dlsym(handle, "GetFileInfoW");

    api.GetFileA         = (SPI_GetFileA)        dlsym(handle, "GetFile");
    api.GetFileW         = (SPI_GetFileW)        dlsym(handle, "GetFileW");

    api.ConfigurationDlg = (SPI_ConfigurationDlg)dlsym(handle, "ConfigurationDlg");

    api.CreatePictureA   = (SPI_CreatePictureA)  dlsym(handle, "CreatePicture");
    api.CreatePictureW   = (SPI_CreatePictureW)  dlsym(handle, "CreatePictureW");

    return true;
}

//---------------------------------------------------------------------------//

inline void tapetums::Susie::Free()
{
  #if defined (_WIN32) || defined(WIN32)
    #define dlclose(handle) ::FreeLibrary(handle)
  #endif

    if ( handle )
    {
        dlclose(handle);
        handle = nullptr;
    }

    path_[0] = '\0';
    api.GetPluginInfoA   = nullptr;
    api.GetPluginInfoW   = nullptr;
    api.IsSupportedA     = nullptr;
    api.IsSupportedW     = nullptr;
    api.GetPictureInfoA  = nullptr;
    api.GetPictureInfoW  = nullptr;
    api.GetPictureA      = nullptr;
    api.GetPictureW      = nullptr;
    api.GetPreviewA      = nullptr;
    api.GetPreviewW      = nullptr;
    api.GetArchiveInfoA  = nullptr;
    api.GetArchiveInfoW  = nullptr;
    api.GetFileInfoA     = nullptr;
    api.GetFileInfoW     = nullptr;
    api.GetFileA         = nullptr;
    api.GetFileW         = nullptr;
    api.ConfigurationDlg = nullptr;
}

//---------------------------------------------------------------------------//

inline SPI_RESULT tapetums::Susie::GetPluginInfoA
(
    int32_t infono, LPSTR buf, int32_t buflen
)
{
    if ( api.GetPluginInfoA )
    {
        return api.GetPluginInfoA(infono, buf, buflen);
    }
    else
    {
        return SPI_NO_FUNCTION;
    }
}

//---------------------------------------------------------------------------//

inline SPI_RESULT tapetums::Susie::GetPluginInfoW
(
    int32_t infono, LPWSTR buf, int32_t buflen
)
{
    if ( api.GetPluginInfoW )
    {
        return api.GetPluginInfoW(infono, buf, buflen);
    }
    else
    {
        return SPI_NO_FUNCTION;
    }
}

//---------------------------------------------------------------------------//

inline SPI_RESULT tapetums::Susie::IsSupportedA
(
    LPCSTR filename, uint8_t* dw
)
{
    if ( api.IsSupportedA )
    {
        return api.IsSupportedA(filename, dw);
    }
    else
    {
        return SPI_NO_FUNCTION;
    }
}

//---------------------------------------------------------------------------//

inline SPI_RESULT tapetums::Susie::IsSupportedW
(
    LPCWSTR filename, uint8_t* dw
)
{
    if ( api.IsSupportedW )
    {
        return api.IsSupportedW(filename, dw);
    }
    else
    {
        return SPI_NO_FUNCTION;
    }
}

//---------------------------------------------------------------------------//

inline SPI_RESULT tapetums::Susie::GetPictureInfoA
(
    LPCSTR buf, size_t len, SPI_FLAG flag,
    SusiePictureInfo* lpInfo
)
{
    if ( api.GetPictureInfoA )
    {
        return api.GetPictureInfoA(buf, len, flag, lpInfo);
    }
    else
    {
        return SPI_NO_FUNCTION;
    }
}

//---------------------------------------------------------------------------//

inline SPI_RESULT tapetums::Susie::GetPictureInfoW
(
    LPCWSTR buf, size_t len, SPI_FLAG flag,
    SusiePictureInfo* lpInfo
)
{
    if ( api.GetPictureInfoW )
    {
        return api.GetPictureInfoW(buf, len, flag, lpInfo);
    }
    else
    {
        return SPI_NO_FUNCTION;
    }
}

//---------------------------------------------------------------------------//

inline SPI_RESULT tapetums::Susie::GetPictureA
(
    LPCSTR buf, size_t len, SPI_FLAG flag,
    void** pHBInfo, void** pHBm,
    SUSIE_PROGRESS progressCallback, intptr_t lData
)
{
    if ( api.GetPictureA )
    {
        return api.GetPictureA
        (
            buf, len, flag, pHBInfo, pHBm, progressCallback, lData
        );
    }
    else
    {
        return SPI_NO_FUNCTION;
    }
}

//---------------------------------------------------------------------------//

inline SPI_RESULT tapetums::Susie::GetPictureW
(
    LPCWSTR buf, size_t len, SPI_FLAG flag,
    void** pHBInfo, void** pHBm,
    SUSIE_PROGRESS progressCallback, intptr_t lData
)
{
    if ( api.GetPictureW )
    {
        return api.GetPictureW
        (
            buf, len, flag, pHBInfo, pHBm, progressCallback, lData
        );
    }
    else
    {
        return SPI_NO_FUNCTION;
    }
}

//---------------------------------------------------------------------------//

inline SPI_RESULT tapetums::Susie::GetPreviewA
(
    LPCSTR buf, size_t len, SPI_FLAG flag,
    void** pHBInfo, void** pHBm,
    SUSIE_PROGRESS progressCallback, intptr_t lData
)
{
    if ( api.GetPreviewA )
    {
        return api.GetPreviewA
        (
            buf, len, flag, pHBInfo, pHBm, progressCallback, lData
        );
    }
    else
    {
        return SPI_NO_FUNCTION;
    }
}

//---------------------------------------------------------------------------//

inline SPI_RESULT tapetums::Susie::GetPreviewW
(
    LPCWSTR buf, size_t len, SPI_FLAG flag,
    void** pHBInfo, void** pHBm,
    SUSIE_PROGRESS progressCallback, intptr_t lData
)
{
    if ( api.GetPreviewW )
    {
        return api.GetPreviewW
        (
            buf, len, flag, pHBInfo, pHBm, progressCallback, lData
        );
    }
    else
    {
        return SPI_NO_FUNCTION;
    }
}

//---------------------------------------------------------------------------//

inline SPI_RESULT tapetums::Susie::GetArchiveInfoA
(
    LPCSTR buf, size_t len, SPI_FLAG flag,
    HLOCAL* lphInf
)
{
    if ( api.GetArchiveInfoA )
    {
        return api.GetArchiveInfoA(buf, len, flag, lphInf);
    }
    else
    {
        return SPI_NO_FUNCTION;
    }
}

//---------------------------------------------------------------------------//

inline SPI_RESULT tapetums::Susie::GetArchiveInfoW
(
    LPCWSTR buf, size_t len, SPI_FLAG flag,
    HLOCAL* lphInf
)
{
    if ( api.GetArchiveInfoW )
    {
        return api.GetArchiveInfoW(buf, len, flag, lphInf);
    }
    else
    {
        return SPI_NO_FUNCTION;
    }
}

//---------------------------------------------------------------------------//

inline SPI_RESULT tapetums::Susie::GetFileInfoA
(
    LPCSTR buf, size_t len, LPCSTR filename, SPI_FLAG flag,
    SusieFileInfoA* lpInfo
)
{
    if ( api.GetFileInfoA )
    {
        return api.GetFileInfoA(buf, len, filename, flag, lpInfo);
    }
    else
    {
        return SPI_NO_FUNCTION;
    }
}

//---------------------------------------------------------------------------//

inline SPI_RESULT tapetums::Susie::GetFileInfoW
(
    LPCWSTR buf, size_t len, LPCWSTR filename, SPI_FLAG flag,
    SusieFileInfoW* lpInfo
)
{
    if ( api.GetFileInfoW )
    {
        return api.GetFileInfoW(buf, len, filename, flag, lpInfo);
    }
    else
    {
        return SPI_NO_FUNCTION;
    }
}

//---------------------------------------------------------------------------//

inline SPI_RESULT tapetums::Susie::GetFileA
(
    LPCSTR src, size_t len, LPSTR dest, SPI_FLAG flag,
    SUSIE_PROGRESS progressCallback, intptr_t lData
)
{
    if ( api.GetFileA )
    {
        return api.GetFileA(src, len, dest, flag, progressCallback, lData);
    }
    else
    {
        return SPI_NO_FUNCTION;
    }
}

//---------------------------------------------------------------------------//

inline SPI_RESULT tapetums::Susie::GetFileW
(
    LPCWSTR src, size_t len, LPWSTR dest, SPI_FLAG flag,
    SUSIE_PROGRESS progressCallback, intptr_t lData
)
{
    if ( api.GetFileW )
    {
        return api.GetFileW(src, len, dest, flag, progressCallback, lData);
    }
    else
    {
        return SPI_NO_FUNCTION;
    }
}

//---------------------------------------------------------------------------//

inline SPI_RESULT tapetums::Susie::ConfigurationDlg
(
    HWND parent, SPI_FNC_CODE fnc
)
{
    if ( api.ConfigurationDlg )
    {
        return api.ConfigurationDlg(parent, fnc);
    }
    else
    {
        return SPI_NO_FUNCTION;
    }
}

//---------------------------------------------------------------------------//

inline SPI_RESULT tapetums::Susie::CreatePictureA
(
    LPCSTR  filepath, SPI_FLAG flag,
    void** pHBInfo, void** pHBm,
    SusiePictureInfo* lpInfo, SUSIE_PROGRESS progressCallback, intptr_t lData
)
{
    if ( api.CreatePictureA )
    {
        return api.CreatePictureA
        (
            filepath, flag, pHBInfo, pHBm, lpInfo, progressCallback, lData
        );
    }
    else
    {
        return SPI_NO_FUNCTION;
    }
}

//---------------------------------------------------------------------------//

inline SPI_RESULT tapetums::Susie::CreatePictureW
(
    LPCWSTR filepath, SPI_FLAG flag,
    void** pHBInfo, void** pHBm,
    SusiePictureInfo* lpInfo, SUSIE_PROGRESS progressCallback, intptr_t lData
)
{
    if ( api.CreatePictureW )
    {
        return api.CreatePictureW
        (
            filepath, flag, pHBInfo, pHBm, lpInfo, progressCallback, lData
        );
    }
    else
    {
        return SPI_NO_FUNCTION;
    }
}

//---------------------------------------------------------------------------//

// Susie.hpp
#pragma once

//---------------------------------------------------------------------------//
//
// Bitmap.hpp
//  ビットマップ画像クラス
//   Copyright (C) 2009 - 2016 tapetums
//
//---------------------------------------------------------------------------//

#include <cstdint>

#include <utility>

#include <windows.h>

#include "File.hpp"

//---------------------------------------------------------------------------//
// 定数
//---------------------------------------------------------------------------//

constexpr auto BM = 0x4D42;

//---------------------------------------------------------------------------//
// 前方宣言
//---------------------------------------------------------------------------//

namespace tapetums
{
    class  Bitmap;
}

//---------------------------------------------------------------------------//
// クラス
//---------------------------------------------------------------------------//

class tapetums::Bitmap
{
private:
    INT32       m_width     { 0 };
    INT32       m_height    { 0 };
    INT32       m_stride    { 0 };
    UINT16      m_bit_count { 0 };
    UINT32      m_clr_used  { 0 };
    HDC         m_hdc       { nullptr };
    HBITMAP     m_bitmap    { nullptr };
    HPALETTE    m_palette   { nullptr };
    BITMAPINFO* m_info      { nullptr };
    UINT8*      m_pbits     { nullptr };

public:
    Bitmap() = default;
    Bitmap(INT32 width, INT32 height, UINT16 bit_count, UINT32 clr_used) { Create(width, height, bit_count, clr_used); }
    Bitmap(const BITMAPINFO* bi)                   { Create(bi); }
    Bitmap(UINT16 rsrcId, HMODULE hInst = nullptr) { Load(rsrcId, hInst); }
    Bitmap(LPCWSTR filename)                       { Load(filename); }
    ~Bitmap()                                      { Dispose(); }

    Bitmap(const Bitmap& lhs)             { copy(lhs); }
    Bitmap& operator =(const Bitmap& lhs) { copy(lhs); return *this; }

    Bitmap(Bitmap&& rhs)             noexcept { swap(std::move(rhs)); }
    Bitmap& operator =(Bitmap&& rhs) noexcept { swap(std::move(rhs)); return *this; }

private:
    void copy(const Bitmap& lhs);
    void swap(Bitmap&& rhs) noexcept;

public:
    auto width()      const noexcept { return m_width; }
    auto height()     const noexcept { return m_height; }
    auto stride()     const noexcept { return m_stride; }
    auto bit()        const noexcept { return m_bit_count; }
    auto color_used() const noexcept { return m_clr_used; }
    auto size()       const noexcept { return m_stride * m_height; }
    auto hdc()        const noexcept { return m_hdc; }
    auto hbitmap()    const noexcept { return m_bitmap; }
    auto hpalette()   const noexcept { return m_palette; }
    auto info()       const noexcept { return m_info; }
    auto pbits()      const noexcept { return m_pbits; }

public:
    bool Create(INT32 width = 1, INT32 height = 1, UINT16 bit_count = 32, UINT32 clr_used = 0);
    bool Create(const BITMAPINFO* bi);
    void Dispose();
    bool Load(UINT16 rsrcId, HMODULE hInst = nullptr);
    bool Load(LPCWSTR filename);
    bool Save(LPCWSTR filename);

private:
    void Init(INT32 width, INT32 height, UINT16 bit_count, UINT32 clr_used);
    void Uninit();
    bool CheckHeader();
    bool CreatePalette();
    bool CreateCompatibleDC();
    bool CreateDIBSection();
    bool CreateBitmapObjects();
};

//---------------------------------------------------------------------------//
// コピー / ムーブ
//---------------------------------------------------------------------------//

inline void tapetums::Bitmap::copy(const tapetums::Bitmap& lhs)
{
    if ( this == &lhs ) { return; }

    if ( ! Create(lhs.m_info) ) { return; }

    ::memcpy(m_pbits, lhs.m_pbits, this->size());
}

//---------------------------------------------------------------------------//

inline void tapetums::Bitmap::swap(tapetums::Bitmap&& rhs) noexcept
{
    if ( this == &rhs ) { return; }

    std::swap(m_width,     rhs.m_width);
    std::swap(m_height,    rhs.m_height);
    std::swap(m_bit_count, rhs.m_bit_count);
    std::swap(m_clr_used,  rhs.m_clr_used);
    std::swap(m_stride,    rhs.m_stride);
    std::swap(m_hdc,       rhs.m_hdc);
    std::swap(m_bitmap,    rhs.m_bitmap);
    std::swap(m_palette,   rhs.m_palette);
    std::swap(m_info,      rhs.m_info);
    std::swap(m_pbits,     rhs.m_pbits);
}

//---------------------------------------------------------------------------//
// Bitmapメソッド
//---------------------------------------------------------------------------//

inline bool tapetums::Bitmap::Create
(
    INT32 w, INT32 h, UINT16 bit_count, UINT32 clr_used
)
{
    if ( m_info ) { return true; }

    // データをメンバ変数に記憶
    Init(w, h, bit_count, clr_used);

    // ヘッダ情報の作成
    const auto bmpInfoSize = sizeof(BITMAPINFOHEADER) + m_clr_used * sizeof(RGBQUAD);
    m_info = (BITMAPINFO*) new UINT8[bmpInfoSize];

    m_info->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
    m_info->bmiHeader.biWidth         = m_width;
    m_info->bmiHeader.biHeight        = h; // Bottom-up or Top-down
    m_info->bmiHeader.biPlanes        = 1;
    m_info->bmiHeader.biBitCount      = m_bit_count;
    m_info->bmiHeader.biCompression   = BI_RGB;
    m_info->bmiHeader.biSizeImage     = this->size();
    m_info->bmiHeader.biXPelsPerMeter = 0;
    m_info->bmiHeader.biYPelsPerMeter = 0;
    m_info->bmiHeader.biClrUsed       = m_clr_used;
    m_info->bmiHeader.biClrImportant  = 0;

    // 内部オブジェクトを生成
    const auto ret = CreateBitmapObjects();
    if ( ! ret )
    {
        Dispose();
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------//

inline bool tapetums::Bitmap::Create(const BITMAPINFO* bi)
{
    // 空のビットマップを作成
    const auto ret = Create
    (
        bi->bmiHeader.biWidth,    bi->bmiHeader.biHeight,
        bi->bmiHeader.biBitCount, bi->bmiHeader.biClrUsed
    );
    if ( ! ret )
    {
        return false;
    }

    // ヘッダ情報をコピー
    m_info->bmiHeader.biXPelsPerMeter = bi->bmiHeader.biXPelsPerMeter;
    m_info->bmiHeader.biYPelsPerMeter = bi->bmiHeader.biYPelsPerMeter;
    m_info->bmiHeader.biClrImportant  = bi->bmiHeader.biClrImportant;

    // パレットデータをコピー
    if ( m_clr_used )
    {
        auto p_dst = (UINT8*)m_info + sizeof(BITMAPINFOHEADER);
        const auto p_src = (UINT8*)bi + bi->bmiHeader.biSize;
        const auto pallete_size  = m_clr_used * sizeof(RGBQUAD);
        ::memcpy(p_dst, p_src, pallete_size);
    }

    return true;
}

//---------------------------------------------------------------------------//

inline void tapetums::Bitmap::Dispose()
{
    if ( m_hdc )
    {
        ::DeleteObject(m_hdc);
    }
    if ( m_bitmap )
    {
        ::DeleteObject(m_bitmap);
    }
    if ( m_palette )
    {
        ::DeleteObject(m_palette);
    }
    if ( m_info )
    {
        delete[] m_info;
    }

    Uninit();
}

//---------------------------------------------------------------------------//

inline bool tapetums::Bitmap::Load(UINT16 rsrcId, HMODULE hInst)
{
    // 読み込み元の指定がない場合は exe から読み込む
    if ( nullptr == hInst )
    {
        hInst = ::GetModuleHandle(nullptr);
    }
    if ( nullptr == hInst )
    {
        // GetModuleHandle() が 失敗
        return false;
    }

    // リソースの読み込み
    const auto hRsrc = ::FindResource
    (
        hInst, MAKEINTRESOURCE(rsrcId), RT_BITMAP
    );
    if ( nullptr == hRsrc )
    {
        // リソースが見つからなかった
        return false;
    }
    const auto hBmp = ::LoadResource(hInst, hRsrc);
    if ( nullptr == hBmp )
    {
        // リソースの読み込みに失敗
        return false;
    }

    // ヘッダ情報の読み込み
    const auto p = (UINT8*)::LockResource(hBmp);
    if ( nullptr == p )
    {
        // ヘッダ情報の読み込みに失敗
        return false;
    }

    // ヘッダ情報およびパレット情報のコピー
    const auto info = (BITMAPINFO*)p;
    const auto bmpInfoSize = info->bmiHeader.biSize + info->bmiHeader.biClrUsed * sizeof(RGBQUAD);
    m_info = (BITMAPINFO*) new UINT8[bmpInfoSize];
    ::memcpy(m_info, info, bmpInfoSize);

    // データをメンバ変数に記憶
    Init
    (
        m_info->bmiHeader.biWidth,    m_info->bmiHeader.biHeight,
        m_info->bmiHeader.biBitCount, m_info->bmiHeader.biClrUsed
    );

    // 内部オブジェクトを生成
    const auto ret = CreateBitmapObjects();
    if ( ! ret )
    {
        return false;
    }

    // ピクセルデータのコピー
    const auto p_src = p + bmpInfoSize;
    ::memcpy(m_pbits, p_src, this->size());

    return true;
}

//---------------------------------------------------------------------------//

inline bool tapetums::Bitmap::Load(LPCWSTR filename)
{
    File file
    (
        filename, File::ACCESS::READ, File::SHARE::READ, File::OPEN::EXISTING
    );

    // ファイルヘッダの読み込み
    BITMAPFILEHEADER bmpfh;
    file.Read(&bmpfh, sizeof(bmpfh));
    if ( bmpfh.bfType != BM )
    {
        // ビットマップではない
        return false;
    }

    // ヘッダ情報およびパレット情報の読み込み
    /// 仮にヘッダのバージョンがV4形式やV5形式でもこれで対応できます。
    const auto bmpInfoSize = bmpfh.bfOffBits - sizeof(bmpfh);
    m_info = (BITMAPINFO*) new UINT8[bmpInfoSize];

    const auto cb = file.Read(m_info, (DWORD)bmpInfoSize);
    if ( cb == 0 )
    {
        // ヘッダ情報の読み込みに失敗
        return false;
    }

    // データをメンバ変数に記憶
    Init
    (
        m_info->bmiHeader.biWidth,    m_info->bmiHeader.biHeight,
        m_info->bmiHeader.biBitCount, m_info->bmiHeader.biClrUsed
    );

    // 内部オブジェクトを生成
    const auto ret = CreateBitmapObjects();
    if ( ! ret )
    {
        return false;
    }

    // ピクセルデータの読み込み
    file.Read(m_pbits, size());

    return true;
}

//---------------------------------------------------------------------------//

inline bool tapetums::Bitmap::Save(LPCWSTR filename)
{
    File file
    (
        filename, File::ACCESS::WRITE, File::SHARE::READ, File::OPEN::OR_CREATE
    );

    const DWORD bmpfhSize = sizeof(BITMAPFILEHEADER);
    const DWORD bmpInfoSize = m_info->bmiHeader.biSize + m_clr_used * sizeof(RGBQUAD);

    // ファイルヘッダの書き出し
    BITMAPFILEHEADER bmpfh;
    bmpfh.bfType      = 0x4d42; // `BM`
    bmpfh.bfOffBits   = bmpfhSize + bmpInfoSize;
    bmpfh.bfReserved1 = 0;
    bmpfh.bfReserved2 = 0;
    bmpfh.bfSize      = bmpfh.bfOffBits + m_info->bmiHeader.biSizeImage;
    file.Write(&bmpfh, bmpfhSize);

    // ヘッダ情報およびパレット情報の書き出し
    file.Write(m_info, bmpInfoSize);

    // ピクセルデータの書き出し
    file.Write(m_pbits, size());

    return true;
}

//---------------------------------------------------------------------------//
// Bitmap 内部メソッド
//---------------------------------------------------------------------------//

inline void tapetums::Bitmap::Init
(
    INT32 width, INT32 height, UINT16 bit_count, UINT32 clr_used
)
{
    m_width     = width;
    m_height    = (height > 0) ? height : -1 * height;
    m_bit_count = bit_count;
    m_clr_used  = (bit_count > 8) ? 0 : (clr_used > 0) ? clr_used : (1 << bit_count);
    m_stride    = (((width * bit_count) + 31) & ~31) / 8;
}

//---------------------------------------------------------------------------//

inline void tapetums::Bitmap::Uninit()
{
    m_width     = 0;
    m_height    = 0;
    m_bit_count = 0;
    m_clr_used  = 0;
    m_stride    = 0;
    m_hdc       = nullptr;
    m_bitmap    = nullptr;
    m_palette   = nullptr;
    m_info      = nullptr;
    m_pbits     = nullptr;
}

//---------------------------------------------------------------------------//

inline bool tapetums::Bitmap::CheckHeader()
{
    if ( m_info->bmiHeader.biSize != sizeof(BITMAPINFOHEADER) )
    {
        // 対応していないヘッダ形式
        return false;
    }
        
    if ( BI_RGB == m_info->bmiHeader.biCompression )
    {
        switch ( m_bit_count )
        {
            case  1: case  4: case  8:
            case 16: case 24: case 32:
            {
                return true;
            }
            default:
            {
                // 対応していないビット深度
                return false;
            }
        }
    }
    else if ( BI_BITFIELDS == m_info->bmiHeader.biCompression )
    {
        switch ( m_bit_count )
        {
            case 16: case 32:
            {
                return true;
            }
            default:
            {
                // 対応していないビット深度
                return false;
            }
        }
    }
    else
    {
        // 圧縮ビットマップには非対応
        return false;
    }
}

//---------------------------------------------------------------------------//

inline bool tapetums::Bitmap::CreatePalette()
{
    if ( m_bit_count > 8 )
    {
        // フルカラーの場合は必要なし
        return true;
    }
        
    // 色数に応じて必要なメモリを確保
    const auto size = sizeof(LOGPALETTE) + m_clr_used * sizeof(PALETTEENTRY);
    auto lpLogPal = (LOGPALETTE*) new UINT8[size];
    lpLogPal->palVersion = 0x300;
    lpLogPal->palNumEntries = (WORD)m_clr_used;

    for ( size_t i = 0; i < m_clr_used; ++i )
    {
        lpLogPal->palPalEntry[i].peRed   = m_info->bmiColors[i].rgbRed;
        lpLogPal->palPalEntry[i].peGreen = m_info->bmiColors[i].rgbGreen;
        lpLogPal->palPalEntry[i].peBlue  = m_info->bmiColors[i].rgbBlue;
    }
            
    m_palette = ::CreatePalette(lpLogPal);
    delete[] lpLogPal;
    lpLogPal = nullptr;

    if ( nullptr == m_palette )
    {
        // パレットデータの作成に失敗
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------//

inline bool tapetums::Bitmap::CreateCompatibleDC()
{
    const auto hDesktopDC = ::GetDC(nullptr);

    m_hdc = ::CreateCompatibleDC(hDesktopDC);

    ::ReleaseDC(nullptr, hDesktopDC);

    if ( nullptr == m_hdc )
    {
        // デバイスコンテキストの作成に失敗
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------//

inline bool tapetums::Bitmap::CreateDIBSection()
{
    // DIBSectionの作成
    m_bitmap = ::CreateDIBSection
    (
        m_hdc, m_info, DIB_RGB_COLORS, (void**)&m_pbits, nullptr, 0
    );
    if ( nullptr == m_bitmap )
    {
        // DIBSectionの生成に失敗
        return false;
    }

    // ビットマップオブジェクトをデバイスコンテキストにセット
    const auto hOldBmp = ::SelectObject(m_hdc, m_bitmap);
    ::DeleteObject(hOldBmp);

    return true;
}

//---------------------------------------------------------------------------//

inline bool tapetums::Bitmap::CreateBitmapObjects()
{
    bool ret;

    // 対応している形式かどうか
    ret = CheckHeader();
    if ( ! ret )
    {
        return false;
    }

    // パレットデータの作成
    ret = CreatePalette();
    if ( ! ret )
    {
        return false;
    }

    // 互換DCの作成
    ret = CreateCompatibleDC();
    if ( ! ret )
    {
        return false;
    }

    // DIBSectionの生成
    ret = CreateDIBSection();
    if ( ! ret )
    {
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------//

// Bitmap.hpp
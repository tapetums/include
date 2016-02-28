#pragma once

//---------------------------------------------------------------------------//
//
// WIC.hpp
//  Windows Imaging Component のラッパー
//   Copyright (C) 2015 tapetums
//
//---------------------------------------------------------------------------//

#include <cstdint>

#include <vector>

#include <windows.h>

#include <wincodec.h>
#pragma comment (lib, "windowscodecs.lib")

#include <wrl.h>
#ifndef COM_PTR
  #define COM_PTR
  template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
#endif

#include "Bitmap.hpp"

//---------------------------------------------------------------------------//

namespace tapetums
{
    namespace WIC
    {
        inline HRESULT Load  (LPCWSTR filename, Bitmap* bitmap);
        inline HRESULT Decode(const void* const data, size_t size, Bitmap* bitmap);
    }
}

//---------------------------------------------------------------------------//

inline HRESULT tapetums::WIC::Load
(
    LPCWSTR filename, Bitmap* bitmap
)
{
    HRESULT hr;

    ComPtr<IWICImagingFactory> factory;
    hr = ::CoCreateInstance
    (
        CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&factory)
    );
    if ( FAILED(hr) )
    {
        return hr;
    }

    ComPtr<IWICBitmapDecoder> decoder;
    hr = factory->CreateDecoderFromFilename
    (
        filename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder
    );
    if ( FAILED(hr) )
    {
        return hr;
    }

    UINT frame_count{ 0 };
    hr = decoder->GetFrameCount(&frame_count);
    if ( frame_count == 0 )
    {
        return hr;
    }

    ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, &frame);
    if ( FAILED(hr) )
    {
        return hr;
    }

    ComPtr<IWICBitmapSource> source;
    hr = ::WICConvertBitmapSource
    (
        GUID_WICPixelFormat32bppPBGRA, frame.Get(), &source
    );
    if ( FAILED(hr) )
    {
        return hr;
    }

    UINT width { 0 };
    UINT height{ 0 };
    hr = source->GetSize(&width, &height);
    if ( width == 0 || height == 0 )
    {
        return hr;
    }

    // Top-Down DIB として作成
    const INT32 height_topdown = (height < 0) ? height : -1 * height;
    bitmap->Dispose();
    if ( ! bitmap->Create(width, height_topdown, 32, 0) )
    {
        return E_FAIL;
    }

    hr = source->CopyPixels
    (
        nullptr, bitmap->stride(), bitmap->size(), bitmap->pbits()
    );
    if ( FAILED(hr) )
    {
        return hr;
    }

    return S_OK;
}

//---------------------------------------------------------------------------//

inline HRESULT tapetums::WIC::Decode
(
    const void* const data, size_t size, tapetums::Bitmap* bitmap
)
{
    if ( nullptr == data || 0 == size || nullptr == bitmap )
    {
        return E_INVALIDARG;
    }

    ComPtr<IStream> stream;
    auto hr = ::CreateStreamOnHGlobal(nullptr, TRUE, &stream);
    if ( FAILED(hr) )
    {
        return hr;
    }

    hr = stream->Write(data, static_cast<ULONG>(size), nullptr);
    if ( FAILED(hr) )
    {
        return hr;
    }

    hr = stream->Seek(LARGE_INTEGER{ 0, 0 }, STREAM_SEEK_SET, nullptr);
    // シークポインタをリセットしないと 0x88982f61 エラーが出る
    if ( FAILED(hr) )
    {
        return hr;
    }

    ComPtr<IWICImagingFactory> factory;
    hr = ::CoCreateInstance
    (
        CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&factory)
    );
    if ( FAILED(hr) )
    {
        return hr;
    }

    ComPtr<IWICBitmapDecoder> decoder;
    hr = factory->CreateDecoderFromStream
    (
        stream.Get(), nullptr, WICDecodeMetadataCacheOnDemand, &decoder
    );
    if ( FAILED(hr) )
    {
        return hr;
    }

    UINT frame_count{ 0 };
    hr = decoder->GetFrameCount(&frame_count);
    if ( frame_count == 0 )
    {
        return hr;
    }

    ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, &frame);
    if ( FAILED(hr) )
    {
        return hr;
    }

    ComPtr<IWICBitmapSource> source;
    hr = ::WICConvertBitmapSource
    (
        GUID_WICPixelFormat32bppPBGRA, frame.Get(), &source
    );
    if ( FAILED(hr) )
    {
        return hr;
    }

    UINT width { 0 };
    UINT height{ 0 };
    hr = source->GetSize(&width, &height);
    if ( width == 0 || height == 0 )
    {
        return hr;
    }

    // Top-Down DIB として作成
    const INT32 height_topdown = (height < 0) ? height : -1 * height;
    bitmap->Dispose();
    if ( ! bitmap->Create(width, height_topdown, 32, 0) )
    {
        return E_FAIL;
    }

    hr = source->CopyPixels
    (
        nullptr, bitmap->stride(), bitmap->size(), bitmap->pbits()
    );
    if ( FAILED(hr) )
    {
        return hr;
    }

    return S_OK;
}

//---------------------------------------------------------------------------//

// WIC.hpp
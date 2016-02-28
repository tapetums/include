#pragma once

//---------------------------------------------------------------------------//
//
// Wave.hpp
//  音声データクラス
//   Copyright (C) 2013-2016 tapetums
//
//---------------------------------------------------------------------------//

#include <cstdint>

#include <vector>

#include <windows.h>
#include <mmreg.h>

#include "RIFF.hpp"
#include "File.hpp"
#include "GenerateUUIDString.hpp"

#pragma warning(disable: 4815)

//---------------------------------------------------------------------------//
// 前方宣言
//---------------------------------------------------------------------------//

namespace tapetums
{
    class Wave;
}

//---------------------------------------------------------------------------//
// クラス
//---------------------------------------------------------------------------//

class tapetums::Wave
{
private:
    File file;

    uint8_t* fmt_offset  { nullptr };
    uint8_t* data_offset { nullptr };

    int64_t  data_size    { 0 };
    uint32_t table_length { 0 };

    ChunkSize64* table_ds64 { nullptr };

public:
    Wave()  = default;
    ~Wave() { Dispose(); }

    Wave(const Wave&)             = delete;
    Wave& operator =(const Wave&) = delete;

    Wave(Wave&&)             noexcept = delete;
    Wave& operator =(Wave&&) noexcept = delete;

public:
    auto format() const noexcept { return fmt_offset ? (WAVEFORMATEXTENSIBLE*)(fmt_offset) : nullptr; }
    auto size()   const noexcept { return data_size; }
    auto data()   const noexcept { return data_offset; }

public:
    bool Create(LPCWSTR path, WAVEFORMATEXTENSIBLE& format, int64_t data_size);
    void Dispose();
    bool Load(LPCWSTR path);
    bool Save(LPCWSTR path);

private:
    bool ReadAllChunks      ();
    bool ReadHeader         (uint8_t* p);
    bool ReadFormatChunk    (uint8_t* p);
    void ReadDataSize64Chunk(uint8_t* p, uint32_t chunkSize);

    uint8_t* ForwardPointer(uint8_t* p, const char chunkId[4], uint32_t chunkSize);
    int64_t  LookUpSizeTable(const char chunkId[4]);
};

//---------------------------------------------------------------------------//
// Utility Function
//---------------------------------------------------------------------------//

uint32_t __stdcall MaskChannelMask(uint16_t channels)
{
    switch ( channels )
    {
        case 1: // monaural
        {
            return SPEAKER_FRONT_CENTER;
        }
        case 2: // stereo
        {
            return SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
        }
        case 4: // 4 way
        {
            return SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT |
                   SPEAKER_BACK_LEFT  | SPEAKER_BACK_RIGHT;
        }
        case 6: // 5.1 ch
        {
            return SPEAKER_FRONT_LEFT   | SPEAKER_FRONT_RIGHT   |
                   SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY |
                   SPEAKER_BACK_LEFT    | SPEAKER_BACK_RIGHT;
        }
        case 8: // 7.1 ch
        {
            return SPEAKER_FRONT_LEFT   | SPEAKER_FRONT_RIGHT   |
                   SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY |
                   SPEAKER_BACK_LEFT    | SPEAKER_BACK_RIGHT    |
                   SPEAKER_SIDE_LEFT    | SPEAKER_SIDE_RIGHT;
        }
        default:
        {
            return 0;
        }
    }
}

//---------------------------------------------------------------------------//
// メソッド
//---------------------------------------------------------------------------//

inline bool tapetums::Wave::Create
(
    LPCWSTR path, WAVEFORMATEXTENSIBLE& format, int64_t size
)
{
    if ( size >= UINT32_MAX && format.Format.wFormatTag != WAVE_FORMAT_EXTENSIBLE )
    {
        return false; // そんなムチャな
    }

    if ( file.is_mapped() ) { Dispose(); }

    // ファイルサイズの計算
    data_size = size;
    const auto riffSize = sizeof(RiffChunk) +
                          sizeof(DataSize64Chunk) +
                          sizeof(FormatExtensibleChunk) +
                          sizeof(DataChunk) +
                          size;

    // メモリマップトファイルの生成
    if ( path == nullptr || path[0] == L'\0' )
    {
        // メモリ上に生成
        wchar_t uuid [40];
        GenerateUUIDStringW(uuid, 40);
        file.Map(riffSize, uuid, File::ACCESS::WRITE);
    }
    else
    {
        // ディスク上に生成
        file.Open(path, File::ACCESS::WRITE, File::SHARE::WRITE, File::OPEN::NEW);
        file.Map(riffSize, path, File::ACCESS::WRITE);
    }
    if ( ! file.is_mapped() )
    {
        return false;
    }

    // ヘッダ部の生成
    RiffChunk riff;
    if ( riffSize > UINT32_MAX )
    {
        ::memcpy(riff.chunkId, chunkId_RF64, 4);
    }
    else
    {
        ::memcpy(riff.chunkId, chunkId_RIFF, 4);
    }
    riff.chunkSize = riffSize > UINT32_MAX ? uint32_t(-1) : uint32_t(riffSize - 8);
    ::memcpy(riff.riffType, riffType_WAVE, 4);

    file.Write(riff);

    DataSize64Chunk ds64;
    if ( riffSize > UINT32_MAX )
    {
        ::memcpy(ds64.chunkId, chunkId_ds64, 4);
    }
    else
    {
        ::memcpy(ds64.chunkId, chunkId_JUNK, 4);
    }
    ds64.chunkSize   = sizeof(ds64) - 8;
    ds64.riffSize    = riffSize - 8;
    ds64.dataSize    = size;
    ds64.sampleCount = 0;
    ds64.tableLength = 0;

    file.Write(ds64);

    if ( format.dwChannelMask == 0 )
    {
        format.dwChannelMask = MaskChannelMask(format.Format.nChannels);
    }

    FormatExtensibleChunk fmt;
    ::memcpy(fmt.chunkId, chunkId_fmt, 4);
    fmt.chunkSize = sizeof(fmt) - 8;
    ::memcpy(&fmt.formatType, &format.Format.wFormatTag, sizeof(format));

    fmt_offset = file.pointer() + 8;
    file.Write(fmt);

    DataChunk data;
    ::memcpy(data.chunkId, chunkId_data, 4);
    data.chunkSize = size > UINT32_MAX ? uint32_t(-1) : uint32_t(size);

    data_offset = file.pointer() + 8;
    file.Write(data);

    return true;
}

//---------------------------------------------------------------------------//

inline void tapetums::Wave::Dispose()
{
    if ( table_ds64 ) { delete[] table_ds64; table_ds64 = nullptr; }

    fmt_offset = data_offset = nullptr;

    file.Close();

    return;
}

//---------------------------------------------------------------------------//

inline bool tapetums::Wave::Load(LPCWSTR path)
{
    file.Open(path, File::ACCESS::WRITE, File::SHARE::WRITE, File::OPEN::EXISTING);
    if ( ! file.is_open() )
    {
        return false;
    }

    file.Map(File::ACCESS::WRITE);
    if ( ! file.is_mapped() )
    {
        return false;
    }

    file.Seek(0);

    return ReadAllChunks();
}

//---------------------------------------------------------------------------//

inline bool tapetums::Wave::Save(LPCWSTR path)
{
    file.Seek(0);

    File tmp(path, File::ACCESS::WRITE, File::SHARE::WRITE, File::OPEN::OR_TRUNCATE);
    tmp.Write(file.pointer(), size_t(file.size()));

    return true;
}

//---------------------------------------------------------------------------//

inline bool tapetums::Wave::ReadAllChunks()
{
    auto p = file.pointer();

    // RIFFチャンクの読み込み
    if ( ! ReadHeader(p) )
    {
        return false;
    }

    // データサイズを取得
    const auto end = p + *(uint32_t*)(p + 4) + 8;

    // ポインタを進める
    p += sizeof(RiffChunk);

    char     chunkId[4];
    uint32_t chunkSize;

    // RIFFサブチャンクの読み込み
    while ( p < end )
    {
        ::memcpy(chunkId,    p,     sizeof(chunkId));
        ::memcpy(&chunkSize, p + 4, sizeof(chunkSize));

        // チャンクデータの読み込み
        if ( 0 == ::strncmp(chunkId, chunkId_ds64, sizeof(chunkId)) )
        {
            // 'ds64' chunk
            ReadDataSize64Chunk(p + 8, chunkSize);
        }
        else if ( 0 == ::strncmp(chunkId, chunkId_fmt, sizeof(chunkId)) )
        {
            // 'fmt ' chunk
            if ( ! ReadFormatChunk(p + 8) )
            {
                return false;
            }
        }
        else if ( 0 == strncmp(chunkId, chunkId_data, sizeof(chunkId)) )
        {
            // 'data' chunk
            if ( chunkSize < UINT32_MAX )
            {
                data_size = chunkSize;
            }
            data_offset = p + 8;
        }

        // ポインタを次のチャンクまで進める
        p = ForwardPointer(p, chunkId, chunkSize);
        if ( nullptr == p )
        {
            return false;
        }
    }

    return true;
}

//---------------------------------------------------------------------------//

inline bool tapetums::Wave::ReadHeader(uint8_t* p)
{
    char     chunkId[4];
    uint32_t chunkSize;
    char     riffType[4];

    memcpy(chunkId,    p,     sizeof(chunkId));
    memcpy(&chunkSize, p + 4, sizeof(chunkSize));
    memcpy(riffType,   p + 8, sizeof(riffType));

    if ( 0 == ::strncmp(chunkId, chunkId_RF64, sizeof(chunkId)) )
    {
        if ( chunkSize != UINT32_MAX )
        {
            return false;
        }
    }
    else if ( 0 != ::strncmp(chunkId, chunkId_RIFF, sizeof(chunkId)) )
    {
        return false;
    }

    if ( 0 != ::strncmp(riffType, riffType_WAVE, sizeof(riffType)) )
    {
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------//

inline bool tapetums::Wave::ReadFormatChunk(uint8_t* p)
{
    WORD tag = WAVE_FORMAT_UNKNOWN;
    memcpy(&tag, p, sizeof(tag));

    if ( tag == WAVE_FORMAT_PCM || tag == WAVE_FORMAT_IEEE_FLOAT )
    {
        fmt_offset = p;
    }
    else if ( tag == WAVE_FORMAT_EXTENSIBLE )
    {
        fmt_offset = p;
    }
    else
    {
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------//

inline void tapetums::Wave::ReadDataSize64Chunk
(
    uint8_t* p, uint32_t chunkSize
)
{
    if ( chunkSize <= sizeof(DataSize64Chunk) )
    {
        // 最小限の情報しか格納されていないとき
        DataSize64ChunkLight chunk;
        ::memcpy(&chunk, p, sizeof(chunk));
        data_size = chunk.dataSize;
    }
    else
    {
        // チャンクサイズ情報が格納されているとき
        DataSize64Chunk chunk;
        ::memcpy(&chunk, p, sizeof(chunk));
        data_size    = chunk.dataSize;
        table_length = chunk.tableLength;

        table_ds64 = new ChunkSize64[table_length];
        ::memcpy(table_ds64, p + sizeof(DataSize64Chunk), table_length * sizeof(ChunkSize64));
    }
}

//---------------------------------------------------------------------------//

inline uint8_t* tapetums::Wave::ForwardPointer
(
    uint8_t* p, const char chunkId[4], uint32_t chunkSize
)
{
    if ( 8 < chunkSize && chunkSize < UINT32_MAX )
    {
        return p + 4 * sizeof(char) + sizeof(chunkSize) + chunkSize;
    }
    else
    {
        if ( 0 == strncmp(chunkId, chunkId_data, sizeof(chunkId)) )
        {
            return p + 4 * sizeof(char) + sizeof(chunkSize) + data_size;
        }
        else if ( table_ds64 )
        {
            return p + 4 * sizeof(char) + sizeof(chunkSize) + LookUpSizeTable(chunkId);
        }
        else
        {
            return nullptr;
        }
    }
}

//---------------------------------------------------------------------------//

inline int64_t tapetums::Wave::LookUpSizeTable(const char chunkId[4])
{
    for ( size_t index = 0; index < table_length; ++index )
    {
        const auto chunk = table_ds64[index];
        if ( 0 == strncmp(chunkId, chunk.chunkId, sizeof(chunkId)) )
        {
            return chunk.chunkSize;
        }
    }

    return 0;
}

//---------------------------------------------------------------------------//

// Wave.hpp
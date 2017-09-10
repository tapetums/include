#pragma once

//---------------------------------------------------------------------------//
//
// Wave.hpp
//  Audio data class
//   Copyright (C) 2013-2016 tapetums
//
//---------------------------------------------------------------------------//

#include <cstdint>

#include <windows.h>
#include <mmreg.h>

#include "RIFF.hpp"
#include "File.hpp"
#include "GenerateUUIDString.hpp"

#pragma warning(push)
#pragma warning(disable: 4815)

//---------------------------------------------------------------------------//
// Forward Declarations
//---------------------------------------------------------------------------//

namespace tapetums
{
    class Wave;

    inline uint32_t MaskChannelMask(uint16_t channels);
}

//---------------------------------------------------------------------------//
// Classes
//---------------------------------------------------------------------------//

class tapetums::Wave
{
private:
    File file;

    WAVEFORMATEXTENSIBLE wfex { };

    uint8_t* data_offset { nullptr };

    int64_t  data_size    { 0 };
    uint32_t table_length { 0 };

    ChunkSize64* table_ds64 { nullptr };

public:
    Wave()  = default;
    ~Wave() { Dispose(); }

    Wave(const Wave&)             = delete;
    Wave& operator =(const Wave&) = delete;

    Wave(Wave&& rhs)             noexcept { swap(std::move(rhs)); }
    Wave& operator =(Wave&& rhs) noexcept { swap(std::move(rhs)); return *this; }

public:
    void swap(Wave&& rhs) noexcept;

public:
    auto& format() const noexcept { return wfex; }
    auto  size()   const noexcept { return data_size; }
    auto  data()   const noexcept { return (const uint8_t*)data_offset; }
    auto  data()   noexcept       { return data_offset; }

public:
    bool Create (LPCWSTR path, const WAVEFORMATEXTENSIBLE& format, int64_t data_size);
    void Dispose();
    bool Load   (LPCWSTR path);
    bool Save   (LPCWSTR path);

private:
    bool ReadAllChunks      ();
    bool ReadHeader         (uint8_t* p);
    bool ReadFormatChunk    (uint8_t* p);
    void ReadDataSize64Chunk(uint8_t* p, uint32_t chunkSize);

    uint8_t* ForwardPointer (uint8_t* p, const char chunkId[4], uint32_t chunkSize);
    int64_t  LookUpSizeTable(const char chunkId[4]);
};

//---------------------------------------------------------------------------//
// Wave Move Constructor
//---------------------------------------------------------------------------//

inline void tapetums::Wave::swap(Wave&& rhs) noexcept
{
    if ( this == &rhs ) { return; }

    std::swap(file,         rhs.file);
    std::swap(wfex,         rhs.wfex);
    std::swap(data_offset,  rhs.data_offset);
    std::swap(data_size,    rhs.data_size);
    std::swap(table_length, rhs.table_length);
    std::swap(table_ds64,   rhs.table_ds64);
}

//---------------------------------------------------------------------------//
// Wave Methods
//---------------------------------------------------------------------------//

inline bool tapetums::Wave::Create
(
    LPCWSTR path, const WAVEFORMATEXTENSIBLE& format, int64_t size
)
{
    if ( file.is_mapped() ) { Dispose(); }

    // メンバ変数に保存
    wfex      = format;
    data_size = size;

    // ファイルサイズの計算
    const auto riffSize = sizeof(RiffChunk) +
                          sizeof(DataSize64Chunk) +
                          sizeof(FormatExtensibleChunk) +
                          sizeof(DataChunk) +
                          data_size;

    // メモリマップトファイルの生成
    if ( path == nullptr || path[0] == L'\0' )
    {
        // メモリ上に生成
        wchar_t uuid [40];
        GenerateUUIDStringW(uuid, 40); // ランダムな名前を生成
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
    ds64.dataSize    = data_size;
    ds64.sampleCount = 0;
    ds64.tableLength = 0;

    file.Write(ds64);

    if ( wfex.dwChannelMask == 0 )
    {
        wfex.dwChannelMask = MaskChannelMask(wfex.Format.nChannels);
    }

    FormatExtensibleChunk fmt;
    ::memcpy(fmt.chunkId, chunkId_fmt, 4);
    fmt.chunkSize = sizeof(fmt) - 8;
    ::memcpy(&fmt.formatType, &wfex.Format.wFormatTag, sizeof(wfex));

    file.Write(fmt);

    DataChunk data;
    ::memcpy(data.chunkId, chunkId_data, 4);
    data.chunkSize = data_size > UINT32_MAX ? uint32_t(-1) : uint32_t(data_size);

    data_offset = file.pointer() + 8;
    file.Write(data);

    return true;
}

//---------------------------------------------------------------------------//

inline void tapetums::Wave::Dispose()
{
    if ( table_ds64 )
    {
        delete[] table_ds64;
        table_ds64 = nullptr;
    }

    data_size = table_length = 0;
    data_offset = nullptr;

    ::memset(&wfex, 0, sizeof(wfex));

    file.Close();

    return;
}

//---------------------------------------------------------------------------//

inline bool tapetums::Wave::Load(LPCWSTR path)
{
    if ( file.is_mapped() ) { return false; }

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
    if ( ! file.is_mapped() ) { return false; }

    file.Seek(0);

    File tmp;
    tmp.Open
    (
        path, File::ACCESS::WRITE, File::SHARE::WRITE, File::OPEN::OR_TRUNCATE
    );

    tmp.Write(file.pointer(), size_t(file.size()));

    return true;
}

//---------------------------------------------------------------------------//
// Wave Internal Methods
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
        if ( 0 == ::memcmp(chunkId, chunkId_ds64, sizeof(chunkId)) )
        {
            // 'ds64' chunk
            ReadDataSize64Chunk(p + 8, chunkSize);
        }
        else if ( 0 == ::memcmp(chunkId, chunkId_fmt, sizeof(chunkId)) )
        {
            // 'fmt ' chunk
            if ( ! ReadFormatChunk(p + 8) )
            {
                return false;
            }
        }
        else if ( 0 == memcmp(chunkId, chunkId_data, sizeof(chunkId)) )
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

    if ( 0 == ::memcmp(chunkId, chunkId_RF64, sizeof(chunkId)) )
    {
        if ( chunkSize != UINT32_MAX )
        {
            return false;
        }
    }
    else if ( 0 != ::memcmp(chunkId, chunkId_RIFF, sizeof(chunkId)) )
    {
        return false;
    }

    if ( 0 != ::memcmp(riffType, riffType_WAVE, sizeof(riffType)) )
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
        memcpy(&wfex, p, sizeof(PCMWAVEFORMAT));
    }
    else if ( tag == WAVE_FORMAT_EXTENSIBLE )
    {
        memcpy(&wfex, p, sizeof(WAVEFORMATEXTENSIBLE));
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
    if ( 4 <= chunkSize && chunkSize < UINT32_MAX )
    {
        return p + 4 * sizeof(char) + sizeof(chunkSize) + chunkSize;
    }
    else
    {
        if ( 0 == memcmp(chunkId, chunkId_data, sizeof(chunkId)) )
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
        if ( 0 == memcmp(chunkId, chunk.chunkId, sizeof(chunkId)) )
        {
            return chunk.chunkSize;
        }
    }

    return 0;
}

//---------------------------------------------------------------------------//
// Utility Functions
//---------------------------------------------------------------------------//

inline uint32_t tapetums::MaskChannelMask(uint16_t channels)
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

#pragma warning(pop)

// Wave.hpp
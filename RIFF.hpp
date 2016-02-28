#pragma once

//---------------------------------------------------------------------------//
//
// RIFF.hpp
//  Structure declarations for RIFF/RF64 file format
//
// See Also:
//   https://tech.ebu.ch/docs/tech/tech3306.pdf
//   https://tech.ebu.ch/docs/tech/tech3306-2009.pdf
//   http://d0ec7852ef61.seesaa.net/article/183599694.html
//
//---------------------------------------------------------------------------//

#include <cstdint>

#pragma warning(disable: 4200)
#pragma warning(disable: 4201)

//---------------------------------------------------------------------------//

static constexpr char chunkId_RIFF[4] = { 'R', 'I', 'F', 'F' };
static constexpr char chunkId_JUNK[4] = { 'J', 'U', 'N', 'K' };
static constexpr char chunkId_fmt [4] = { 'f', 'm', 't', ' ' };
static constexpr char chunkId_data[4] = { 'd', 'a', 't', 'a' };

static constexpr char chunkId_RF64[4] = { 'R', 'F', '6', '4' };
static constexpr char chunkId_ds64[4] = { 'd', 's', '6', '4' };

static constexpr char chunkId_LIST[4] = { 'L', 'I', 'S', 'T' };
static constexpr char chunkId_INFO[4] = { 'I', 'N', 'F', 'O' };

static constexpr char chunkId_wavl[4] = { 'w', 'a', 'v', 'l' };
static constexpr char chunkId_slnt[4] = { 's', 'l', 'n', 't' };

static constexpr char chunkId_cue [4] = { 'c', 'u', 'e', ' ' };
static constexpr char chunkId_plst[4] = { 'p', 'l', 's', 't' };
static constexpr char chunkId_list[4] = { 'l', 'i', 's', 't' };
static constexpr char chunkId_labl[4] = { 'l', 'a', 'b', 'l' };
static constexpr char chunkId_note[4] = { 'n', 'o', 't', 'e' };
static constexpr char chunkId_ltxt[4] = { 'l', 't', 'x', 't' };
static constexpr char chunkId_smpl[4] = { 's', 'm', 'p', 'l' };
static constexpr char chunkId_inst[4] = { 'i', 'n', 's', 't' };
static constexpr char chunkId_r64m[4] = { 'r', '6', '4', 'm' };

static constexpr char chunkId_bext[4] = { 'b', 'e', 'x', 't' };
static constexpr char chunkId_iXML[4] = { 'i', 'X', 'M', 'L' };
static constexpr char chunkId_qlty[4] = { 'q', 'l', 't', 'y' };
static constexpr char chunkId_mext[4] = { 'm', 'e', 'x', 't' };
static constexpr char chunkId_levl[4] = { 'l', 'e', 'v', 'l' };
static constexpr char chunkId_link[4] = { 'l', 'i', 'n', 'k' };
static constexpr char chunkId_axml[4] = { 'a', 'x', 'm', 'l' };
static constexpr char chunkId_cont[4] = { 'c', 'o', 'n', 't' };

static constexpr char riffType_WAVE[4] = { 'W', 'A', 'V', 'E' };

//---------------------------------------------------------------------------//

struct Guid;
struct RiffChunk;
struct JunkChunk;
struct FormatChunk;
struct DataChunk;
struct RF64Chunk;
struct ChunkSize64;
struct DataSize64ChunkLight;
struct DataSize64Chunk;
struct FormatExtensibleChunk;
struct CuePoint;
struct CueChunk;
struct ListChunk;
struct LabelChunk;
struct MarkerEntry;
struct MarkerChunk;

//---------------------------------------------------------------------------//

#pragma pack(push, 1)

//---------------------------------------------------------------------------//

struct Guid
{
    uint32_t data1;
    uint16_t data2;
    uint16_t data3;
    uint32_t data4;
    uint32_t data5;
};

//---------------------------------------------------------------------------//

struct RiffChunk
{
    char     chunkId[4];     // 'RIFF'
    uint32_t chunkSize;      // 4 byte size of the traditional RIFF/WAVE file
    char     riffType[4];    // 'WAVE'
};

//---------------------------------------------------------------------------//

struct JunkChunk
{
    char     chunkId[4];     // 'JUNK'
    uint32_t chunkSize;      // 4 byte size of the 'JUNK' chunk. This must be
                             // at least 28 if the chunk is intended as a
                             // place-holder for a 'ds64' chunk.
    uint8_t  chunkData[0];   // dummy bytes
};

//---------------------------------------------------------------------------//

struct FormatChunk
{
    char     chunkId[4];     // 'fmt '
    uint32_t chunkSize;      // 4 byte size of the 'fmt ' chunk
    uint16_t formatType;     // WAVE_FORMAT_PCM = 0x0001, etc.
    uint16_t channelCount;   // 1 = mono, 2 = stereo, etc.
    uint32_t sampleRate;     // 32000, 44100, 48000, etc.
    uint32_t bytesPerSecond; // only important for compressed formats
    uint16_t blockAlignment; // container size (in bytes) of one set of samples
    uint16_t bitsPerSample;  // valid bits per sample 16, 20 or 24
    uint16_t cbSize = 0;     // extra information (after cbSize) to store
    uint8_t  extraData[22];  // extra data of WAVE_FORMAT_EXTENSIBLE when necessary
};

//---------------------------------------------------------------------------//

struct DataChunk
{
    char     chunkId[4];     // 'data'
    uint32_t chunkSize;      // 4 byte size of the 'data' chunk
    uint8_t  waveData[0];    // audio samples
};

//---------------------------------------------------------------------------//

struct RF64Chunk
{
    char     chunkId[4];     // 'RF64'
    uint32_t chunkSize;      // -1 = 0xFFFFFFFF means don't use this data, use
                             // riffSizeHigh and riffSizeLow in 'ds64' chunk instead
    char     rf64Type[4];    // 'WAVE'
};

//---------------------------------------------------------------------------//

struct ChunkSize64
{
    char chunkId[4];         // chunk ID (i.e. "big1" – this chunk is a big one)
    union
    {
        uint64_t chunkSize;  // 8 byte size of the chunk
        struct
        {
            uint32_t chunkSizeLow;
            uint32_t chunkSizeHigh;
        };
    };
};

//---------------------------------------------------------------------------//

struct DataSize64ChunkLight
{
    char     chunkId[4];     // 'ds64'
    uint32_t chunkSize;      // 4 byte size of the 'ds64' chunk
    union
    {
        uint64_t riffSize;   // 8 byte size of the RF64 block
        struct
        {
            uint32_t riffSizeLow;
            uint32_t riffSizeHigh;
        };
    };
    union
    {
        uint64_t dataSize;   // 8 byte size of the data chunk
        struct
        {
            uint32_t dataSizeLow;
            uint32_t dataSizeHigh;
        };
    };
};

//---------------------------------------------------------------------------//

struct DataSize64Chunk
{
    char     chunkId[4];     // 'ds64'
    uint32_t chunkSize;      // 4 byte size of the 'ds64' chunk
    union
    {
        uint64_t riffSize;   // 8 byte size of the RF64 block
        struct
        {
            uint32_t riffSizeLow;
            uint32_t riffSizeHigh;
        };
    };
    union
    {
        uint64_t dataSize;   // 8 byte size of the data chunk
        struct
        {
            uint32_t dataSizeLow;
            uint32_t dataSizeHigh;
        };
    };
    union
    {
        uint64_t sampleCount;// 8 byte sample count of fact chunk
        struct
        {
            uint32_t sampleCountLow;
            uint32_t sampleCountHigh;
        };
    };
    uint32_t tableLength;    // number of valid entries in array "table"
    ChunkSize64 table[0];
};

//---------------------------------------------------------------------------//

struct FormatExtensibleChunk
{
    char     chunkId[4];         // 'fmt '
    uint32_t chunkSize;          // 4 byte size of the 'fmt ' chunk
    uint16_t formatType;         // WAVE_FORMAT_EXTENSIBLE = 0xFFFE
    uint16_t channelCount;       // 1 = mono, 2 = stereo, etc.
    uint32_t sampleRate;         // 32000, 44100, 48000, etc.
    uint32_t bytesPerSecond;     // only important for compressed formats
    uint16_t blockAlignment;     // container size (in bytes) of one set of samples
    uint16_t bitsPerSample;      // bits per sample in container size * 8, i.e. 8, 16, 24
    uint16_t cbSize = 22;        // extra information (after cbSize) to store
    uint16_t validBitsPerSample; // valid bits per sample i.e. 8, 16, 20, 24
    uint32_t channelMask;        // channel mask for channel allocation
    Guid     subFormat;          // KSDATAFORMAT_SUBTYPE_PCM
                                 // data1 = 0x00000001
                                 // data2 = 0x0000
                                 // data3 = 0x0010
                                 // data4 = 0xAA000080
                                 // data5 = 0x719B3800
};

//---------------------------------------------------------------------------//

struct CuePoint // declare CuePoint structure
{
    uint32_t identifier;     // unique identifier for the cue point
    uint32_t position;       // position of the cue point in the play order
    char     dataChunkId[4]; // normally 'data'
    uint32_t chunkStart;     // used for wave lists
    uint32_t blockStart;     // Start of compressed data block containing the cue point
                             // (not used for PCM)
    uint32_t sampleOffset;   // sample offset of cue point (absolute for PCM,
                             // relative to block start for compressed data)
};

//---------------------------------------------------------------------------//

struct CueChunk // declare CueChunk structure
{
    char     chunkId[4];     // 'cue '
    uint32_t chunkSize;      // 4 byte size of the 'cue ' chunk
    uint32_t cuePointCount;  // number of cue points (markers)
    CuePoint cuePoints[0];   // cue points
};

//---------------------------------------------------------------------------//

struct ListChunk // declare ListChunk structure
{
    char     chunkId[4];     // 'list'
    uint32_t chunkSize;      // 4 byte size of the 'list' chunk
    char     typeId[4];      // 'adtl' associated data list
};

//---------------------------------------------------------------------------//

struct LabelChunk // declare LabelChunk structure
{
    char     chunkId[4];     // 'labl'
    uint32_t chunkSize;      // 4 byte size of the 'labl' chunk
    uint32_t identifier;     // unique identifier for the cue point
    char     text[0];        // label text: null terminated string (ANSI)
};

//---------------------------------------------------------------------------//

struct MarkerEntry // declare MarkerEntry structure
{
    uint32_t flags;               // flags field
    union
    {
        uint64_t sampleOffse;     // 8 byte marker's offset in samples in data chunk
        struct
        {
            uint32_t sampleOffsetLow;
            uint32_t sampleOffsetHigh;
        };
    };
    union
    {
        uint64_t byteOffset;      // 8 byte of the beginning of the nearest
        struct                    // compressed frame next to marker (timely before)
        {
            uint32_t byteOffsetLow;
            uint32_t byteOffsetHigh;
        };
    };
    union
    {
        uint64_t intraSmplOffset; // 8 byte of marker's offset in samples
        struct                    // relative to the position of the first sample in frame
        {
            uint32_t intraSmplOffsetHigh;
            uint32_t intraSmplOffsetLow;
        };
    };
    uint8_t  labelText[256];      // null terminated label string
                                  // (the encoding depends on the Bit 4 of "flags" field)
    uint32_t lablChunkIdentifier; // link to 'labl' subchunk of 'list' chunk8
    Guid     vendorAndProduct;    // GUID identifying specific vendor application
    uint32_t userData1;           // 4 byte application specific user data
    uint32_t userData2;           // 4 byte application specific user data
    uint32_t userData3;           // 4 byte application specific user data
    uint32_t userData4;           // 4 byte application specific user data
};

//---------------------------------------------------------------------------//

struct MarkerChunk // declare MarkerChunk structure
{
    char     chunkId[4];    // 'r64m'
    uint32_t chunkSize;     // 4 byte size of the 'r64m' chunk
    MarkerEntry markers[0]; // marker entries
};

//---------------------------------------------------------------------------//
//
// Definition of the flags field of MarkerEntry structure
//
// The flags field defines different features of the chunk and validity of fields in the struct.
//   Bit 0 0 entry is invalid (skip entry)
//         1 entry is valid
//   Bit 1 0 byteOffset is invalid (do not use)
//         1 byteOffset is valid
//   Bit 2 0 intraSmplOffset is invalid (do not use)
//         1 intraSmplOffset is valid
//   Bit 3 0 labelText is holding marker’s label string (if label string is empty, marker has no label)
//         1 marker’s label is stored in ‘labl’ chunk; use lablChunkIdentifier to retrieve
//   Bit 4 0 labelText string is ANSI
//         1 labelText string is UTF-8
//
//---------------------------------------------------------------------------//

#pragma pack(pop)

//---------------------------------------------------------------------------//

// RIFF.hpp
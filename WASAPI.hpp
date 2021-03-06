﻿#pragma once

//---------------------------------------------------------------------------//
//
// WASAPI.hpp
//  Windows Audio Session API による 音声出力モジュール (ヘッダオンリー)
//  排他/共有モードのイベント通知のみに対応 (requires Windows 7 or greater)
//   Copyright (C) 2011-2017 tapetums
//
//---------------------------------------------------------------------------//

#pragma region USAGE
/******************************************************************************

#include <Wave.hpp>
#include <WASAPI.hpp>


UINT WM_WASAPI_CALLBACK { 0 }; // Define the substance somewhere once


int32_t wmain(int32_t argc, wchar_t* argv[])
{
    HRESULT hr;

    ::CoInitialize(nullptr); // Necessary for CoCreateInstance()

    // Load wave file data
    tapetums::Wave wave;
    if ( argc < 2 || ! wave.Load(argv[1]) )
    {
        return -1;
    }
    const auto& wfex = *wave.format();

    // Mark the end of data
    auto wave_data = wave.data();
    const auto wave_end = wave_data + wave.size();

    // Initialize WASAPI manager
    tapetums::WASAPI::Manager mgr;
    hr = mgr.Init();
    if ( FAILED(hr) )
    {
        return -1;
    }

    // Get a device
    auto device = mgr.GetDefaultDevice();

    // Format configuration
    tapetums::WASAPI::Config cfg;
    cfg.format.Format.wFormatTag           = WAVE_FORMAT_EXTENSIBLE;
    cfg.format.Format.nChannels            = wfex.Format.nChannels;
    cfg.format.Format.nSamplesPerSec       = wfex.Format.nSamplesPerSec;
    cfg.format.Format.nAvgBytesPerSec      = wfex.Format.nAvgBytesPerSec;
    cfg.format.Format.nBlockAlign          = wfex.Format.nBlockAlign;
    cfg.format.Format.wBitsPerSample       = wfex.Format.wBitsPerSample;
    cfg.format.Format.cbSize               = 22;
    cfg.format.Samples.wValidBitsPerSample = wfex.Format.wBitsPerSample == 24 ? 32 : wfex.Format.wBitsPerSample;
    cfg.format.dwChannelMask               = tapetums::MaskChannelMask(wfex.Format.nChannels);
    cfg.format.SubFormat                   = KSDATAFORMAT_SUBTYPE_PCM;
    cfg.share_mode                         = AUDCLNT_SHAREMODE_SHARED;
    cfg.period                             = 50 * 1000 * 10; // = 50ms

    // Open the device
    hr = device.Open(cfg);
    if ( FAILED(hr) )
    {
        return -1;
    }

    // Fill the buffers in advance
    {
        auto buffer  = device.buffer();
        auto bufsize = device.buffer_size();

        ::memcpy(buffer, wave_data, bufsize);
        wave_data += bufsize;
    }

    // Make a message queue
    MSG msg;
    ::PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    // Start playing
    hr = device.Start(::GetCurrentThreadId());
    if ( FAILED(hr) )
    {
        return -1;
    }

    // Message loop
    while ( ::GetMessage(&msg, nullptr, 0, 0) > 0 )
    {
        if ( msg.message == WM_WASAPI_CALLBACK )
        {
            // See WASAPI::Device::MainLoop()
            auto buffer  = (uint8_t*)msg.wParam;
            auto bufsize = (size_t)  msg.lParam;

            // Fill the buffer
            ::memcpy(buffer, wave_data, bufsize);
            wave_data += bufsize;

            // If data reach the terminal
            if ( wave_data >= wave_end )
            {
                device.Stop();
                break;
            }
        }

        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    // Termination
    device.Close();

    ::CoUninitialize();

    return static_cast<int32_t>(msg.wParam);
}
******************************************************************************/
#pragma endregion

#include <string>
#include <thread>
#include <vector>

#include <windows.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <Functiondiscoverykeys_devpkey.h>

#include <Avrt.h>
#pragma comment(lib, "Avrt.lib")

#ifndef COM_PTR
  #define COM_PTR
  #include <wrl/client.h>
  template<typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;
#endif

//---------------------------------------------------------------------------//
// Clallback Message
//---------------------------------------------------------------------------//

extern UINT WM_WASAPI_CALLBACK; // Declare the entity somewhere before use

//---------------------------------------------------------------------------//
// Forward Declarations
//---------------------------------------------------------------------------//

namespace tapetums
{
    namespace WASAPI
    {
        struct Lock;
        struct LockGuard;
        struct PropVariant;

        struct Config;

        class Device;
        class Manager;

        inline HRESULT WriteSilence(IAudioRenderClient* renderer, UINT32 frame_count, size_t size);
        inline HRESULT WriteBuffer (IAudioRenderClient* renderer, UINT32 frame_count, const uint8_t* data, size_t size);
    }
}

//---------------------------------------------------------------------------//
// RAII Objects
//---------------------------------------------------------------------------//

struct tapetums::WASAPI::Lock final
{
    CRITICAL_SECTION cs;

    Lock()  { ::InitializeCriticalSection(&cs); }
    ~Lock() { ::DeleteCriticalSection(&cs); }

    Lock(const Lock&)             = delete;
    Lock& operator =(const Lock&) = delete;

    Lock(Lock&& rhs)            noexcept = default;
    Lock& operator=(Lock&& rhs) noexcept = default;

    void enter() { ::EnterCriticalSection(&cs); }
    void leave() { ::LeaveCriticalSection(&cs); }
};

//---------------------------------------------------------------------------//

struct tapetums::WASAPI::LockGuard final
{
    tapetums::WASAPI::Lock& m_lock;

    explicit LockGuard(Lock& lock) : m_lock(lock) { m_lock.enter(); }
    ~LockGuard()                                  { m_lock.leave(); }

    LockGuard(const LockGuard&)             = delete;
    LockGuard& operator =(const LockGuard&) = delete;

    LockGuard(LockGuard&& rhs)            noexcept = delete;
    LockGuard& operator=(LockGuard&& rhs) noexcept = delete;
};

//---------------------------------------------------------------------------//

struct tapetums::WASAPI::PropVariant final
{
    PROPVARIANT* v;

    explicit PropVariant (PROPVARIANT* v) : v(v) { ::PropVariantInit(v); }
    ~PropVariant()                               { ::PropVariantClear(v); }

    PropVariant(const PropVariant&)             = delete;
    PropVariant& operator =(const PropVariant&) = delete;

    PropVariant(PropVariant&& rhs)            noexcept = delete;
    PropVariant& operator=(PropVariant&& rhs) noexcept = delete;
};

//---------------------------------------------------------------------------//
// Structures
//---------------------------------------------------------------------------//

struct tapetums::WASAPI::Config final
{
    WAVEFORMATEXTENSIBLE format;
    AUDCLNT_SHAREMODE    share_mode;
    REFERENCE_TIME       period;
};

//---------------------------------------------------------------------------//
// Classes
//---------------------------------------------------------------------------//

class tapetums::WASAPI::Device final
{
    friend class Manager;

private:
    ComPtr<IMMDevice>          m_device;
    ComPtr<IAudioClient>       m_client;
    ComPtr<IAudioRenderClient> m_renderer;

    HANDLE         m_evt_empty   { nullptr };
    REFERENCE_TIME m_latency     { 0 };
    UINT           m_frame_count { 0 };
    DWORD          m_buf_size    { 0 };
    DWORD          m_listener    { 0 };
    bool           m_loop        { false };

    tapetums::WASAPI::Lock m_lock;
    std::thread            m_thread_write;
    std::vector<uint8_t>   m_buffer;

    tapetums::WASAPI::Config m_config;

public: // ctor / dtor
    Device() = default;
    ~Device() { Close(); }

    Device(const Device&)             = delete;
    Device& operator =(const Device&) = delete;

    Device(Device&& rhs)             noexcept { swap(std::move(rhs)); }
    Device& operator =(Device&& rhs) noexcept { swap(std::move(rhs)); return *this; }

private:
    void swap(Device&& rhs) noexcept;

public: // properties
    auto is_open    () const noexcept { return m_client != nullptr; }
    auto is_running () const noexcept { return m_thread_write.joinable(); }
    auto latency    () const noexcept { return m_latency; }
    auto frame_count() const noexcept { return m_frame_count; }
    auto buffer_size() const noexcept { return m_buf_size; }
    auto buffer     () const noexcept { return m_buffer.data(); }
    auto buffer     () noexcept       { return m_buffer.data(); }

    auto& config() const noexcept { return m_config; }

public: // methods
    HRESULT Open (const Config& cfg);
    HRESULT Close();
    HRESULT Start(DWORD listener_thread_id);
    HRESULT Stop ();

private:
    HRESULT Activate();
    HRESULT GetMixFormat();
    HRESULT IsFormatSupported(const Config& cfg);
    HRESULT GetDevicePeriod(REFERENCE_TIME period);
    HRESULT Initialize();
    HRESULT SetEventHandle();
    HRESULT GetStreamLatency();
    HRESULT GetBufferSize();
    HRESULT GetService();
    void    MainLoop();
};

//---------------------------------------------------------------------------//

class tapetums::WASAPI::Manager final
{
private:
    ComPtr<IMMDeviceEnumerator> device_enumerator;
    ComPtr<IMMDeviceCollection> device_collection;
    std::vector<std::wstring>   device_names;
    UINT                        default_index;

public: // ctor / dtor
    Manager();
    ~Manager() = default;

    Manager(const Manager&)             = delete;
    Manager& operator =(const Manager&) = delete;

    Manager(Manager&&)             noexcept = default;
    Manager& operator =(Manager&&) noexcept = default;

public: // properties
    auto is_initialized      ()           const noexcept { return device_names.size() != 0; }
    auto device_count        ()           const noexcept { return UINT(device_names.size()); }
    auto device_name         (UINT index) const          { return device_names[index].c_str(); }
    auto default_device_index()           const noexcept { return default_index; }
    auto default_device_name ()           const          { return device_name(default_index); }

public: // methods
    HRESULT Init     ();
    void    Uninit   ();
    Device  GetDevice(UINT index);
    Device  GetDevice(LPCWSTR name);

    Device  GetDefaultDevice() { return GetDevice(default_index); }
};

//---------------------------------------------------------------------------//
// WASAPI::Manager ctor
//---------------------------------------------------------------------------//

inline tapetums::WASAPI::Manager::Manager()
{
    if ( WM_WASAPI_CALLBACK == 0 )
    {
        WM_WASAPI_CALLBACK = ::RegisterWindowMessageW(L"WM_WASAPI_CALLBACK");
    }
}

//---------------------------------------------------------------------------//
// WASAPI::Manager Methods
//---------------------------------------------------------------------------//

inline HRESULT tapetums::WASAPI::Manager::Init()
{
    if ( is_initialized() ) { return S_FALSE; }

    HRESULT hr;

    // インターフェイスを取得
    {
        hr = ::CoCreateInstance
        (
            __uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&device_enumerator)
        );
        if ( FAILED(hr) )
        {
            return hr;
        }

        hr = device_enumerator->EnumAudioEndpoints
        (
            eRender, DEVICE_STATE_ACTIVE, &device_collection
        );
        if ( FAILED(hr) )
        {
            return hr;
        }
    }

    // デフォルトのデバイス名を求める
    std::wstring default_device_name;
    {
        ComPtr<IMMDevice> device;
        hr = device_enumerator->GetDefaultAudioEndpoint
        (
            eRender, eConsole, &device
        );
        if ( FAILED(hr) )
        {
            return hr;
        }

        ComPtr<IPropertyStore> ps;
        hr = device->OpenPropertyStore(STGM_READ, &ps);
        if ( FAILED(hr) )
        {
            return hr;
        }

        PROPVARIANT friendly_name;
        PropVariant v { &friendly_name };

        hr = ps->GetValue(PKEY_Device_FriendlyName, &friendly_name);
        if ( FAILED(hr) || friendly_name.vt != VT_LPWSTR )
        {
            return hr;
        }

        default_device_name = friendly_name.pwszVal;
    }

    // デバイス名をすべて求める
    for ( UINT index = 0; ; ++index )
    {
        ComPtr<IMMDevice> device;
        hr = device_collection->Item(index, &device);
        if ( FAILED(hr) )
        {
            return S_OK;
        }

        ComPtr<IPropertyStore> ps;
        hr = device->OpenPropertyStore(STGM_READ, &ps);
        if ( FAILED(hr) )
        {
            continue;
        }

        PROPVARIANT friendly_name;
        PropVariant v { &friendly_name };
        hr = ps->GetValue(PKEY_Device_FriendlyName, &friendly_name);
        if ( FAILED(hr) )
        {
            continue;
        }

        const auto name = (friendly_name.vt == VT_LPWSTR) ?
                          friendly_name.pwszVal : L"Unknown";

        device_names.emplace_back(name);

        // デフォルトデバイスのインデックスを記憶
        if ( name == default_device_name )
        {
            default_index = index;
        }
    }
}

//---------------------------------------------------------------------------//

inline void tapetums::WASAPI::Manager::Uninit()
{
    default_index = UINT(-1);
    device_names.clear();
    device_collection = nullptr;
    device_enumerator = nullptr;
}

//---------------------------------------------------------------------------//

inline tapetums::WASAPI::Device tapetums::WASAPI::Manager::GetDevice
(
    UINT index
)
{
    Device dvc;

    if ( index < device_count() )
    {
        device_collection->Item(index, &dvc.m_device);
    }
    else
    {
        device_enumerator->GetDefaultAudioEndpoint
        (
            eRender, eConsole, &dvc.m_device
        );
    }

    return dvc;
}

//---------------------------------------------------------------------------//

inline tapetums::WASAPI::Device tapetums::WASAPI::Manager::GetDevice
(
    LPCWSTR name
)
{
    Device dvc;

    if ( nullptr == name )
    {
        device_enumerator->GetDefaultAudioEndpoint
        (
            eRender, eConsole, &dvc.m_device
        );
    }
    else
    {
        UINT index { 0 };
        for ( const auto& n : device_names )
        {
            if ( n == name )
            {
                device_collection->Item(index, &dvc.m_device);
                break;
            }

            ++index;
        }
    }

    return dvc;
}

//---------------------------------------------------------------------------//
// WASAPI::Device Methods
//---------------------------------------------------------------------------//

inline void tapetums::WASAPI::Device::swap(Device&& rhs) noexcept
{
    if ( this == &rhs ) { return; }

    std::swap(m_device,       rhs.m_device);
    std::swap(m_client,       rhs.m_client);
    std::swap(m_renderer,     rhs.m_renderer);
    std::swap(m_evt_empty,    rhs.m_evt_empty);
    std::swap(m_latency,      rhs.m_latency);
    std::swap(m_frame_count,  rhs.m_frame_count);
    std::swap(m_buf_size,     rhs.m_buf_size);
    std::swap(m_listener,     rhs.m_listener);
    std::swap(m_loop,         rhs.m_loop);
    std::swap(m_lock,         rhs.m_lock);
    std::swap(m_thread_write, rhs.m_thread_write);
    std::swap(m_buffer,       rhs.m_buffer);
    std::swap(m_config,       rhs.m_config);
}

//---------------------------------------------------------------------------//

inline HRESULT tapetums::WASAPI::Device::Open
(
    const Config& cfg
)
{
    HRESULT hr;

    tapetums::WASAPI::LockGuard guard(m_lock);

    if ( is_open() )
    {
        return S_FALSE;
    }

    hr = Activate();
    if ( FAILED(hr) ) { Close(); return hr; }

    hr = GetMixFormat();
    if ( FAILED(hr) ) { Close(); return hr; }

    hr = IsFormatSupported(cfg);
    if ( FAILED(hr) ) { Close(); return hr; }

    hr = GetDevicePeriod(cfg.period);
    if ( FAILED(hr) ) { Close(); return hr; }

    hr = Initialize();
    if ( FAILED(hr) ) { Close(); return hr; }

    hr = SetEventHandle();
    if ( FAILED(hr) ) { Close(); return hr; }

    hr = GetStreamLatency();
    if ( FAILED(hr) ) { Close(); return hr; }

    hr = GetBufferSize();
    if ( FAILED(hr) ) { Close(); return hr; }

    hr = GetService();
    if ( FAILED(hr) ) { Close(); return hr; }

    return hr;
}

//---------------------------------------------------------------------------//

inline HRESULT tapetums::WASAPI::Device::Close()
{
    tapetums::WASAPI::LockGuard guard(m_lock);

    if ( ! is_open() )
    {
        return S_FALSE;
    }

    // 書き出しスレッドを終了させる
    Stop();

    // デバイスを閉じる
    m_renderer    = nullptr;
    m_buf_size    = 0;
    m_frame_count = 0;
    m_latency     = 0;
    m_client      = nullptr;

    // イベントの破棄
    if ( m_evt_empty )
    {
        ::CloseHandle(m_evt_empty);
        m_evt_empty = nullptr;
    }

    return S_OK;
}

//---------------------------------------------------------------------------//

inline HRESULT tapetums::WASAPI::Device::Start
(
    DWORD listener_thread_id
)
{
    tapetums::WASAPI::LockGuard guard(m_lock);

    if ( ! is_open() )
    {
        return E_FAIL;
    }
    if ( is_running() )
    {
        return S_FALSE;
    }

    // コールバック先スレッドのハンドルを記憶
    m_listener = listener_thread_id;

    // 書き出しスレッドの開始
    m_thread_write = std::thread([this]()
    {
        // MMCSSの設定
        DWORD task_index;
        const auto hTask = ::AvSetMmThreadCharacteristics
        (
            TEXT("Pro Audio"), &task_index
        );

        // スレッドの優先順位を上げる
        ::SetThreadPriority
        (
            ::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL
        );

        // メインループ
        MainLoop();

        // MMCSSを解除
        if ( hTask ) { ::AvRevertMmThreadCharacteristics(hTask); }
    });
    m_loop = true;

    return S_ASYNCHRONOUS;
}

//---------------------------------------------------------------------------//

inline HRESULT tapetums::WASAPI::Device::Stop()
{
    tapetums::WASAPI::LockGuard guard(m_lock);

    if ( ! is_running() )
    {
        return S_FALSE;
    }

    // 書き出しスレッドを終了させる
    m_loop = false;
    if ( m_thread_write.joinable() )
    {
        m_thread_write.join();
    }

    return S_OK;
}

//---------------------------------------------------------------------------//
// WASAPI::Device Inner Methods
//---------------------------------------------------------------------------//

inline HRESULT tapetums::WASAPI::Device::Activate()
{
    // デバイスを起動
    const auto hr = m_device->Activate
    (
        __uuidof(IAudioClient), CLSCTX_INPROC_SERVER, nullptr,
        (void**)&m_client
    );

    return hr;
}

//---------------------------------------------------------------------------//

inline HRESULT tapetums::WASAPI::Device::GetMixFormat()
{
    WAVEFORMATEXTENSIBLE* current_format { nullptr };

    // デバイスの現在のフォーマット情報を取得する
    const auto hr = m_client->GetMixFormat
    (
        (WAVEFORMATEX**)&current_format
    );

    if ( current_format )
    {
        ::CoTaskMemFree(current_format);
    }

    return hr;
}

//---------------------------------------------------------------------------//

inline HRESULT tapetums::WASAPI::Device::IsFormatSupported
(
    const Config& cfg
)
{
    WAVEFORMATEXTENSIBLE* closest_format { nullptr };

    // 対応形式かを調べる
    const auto hr = m_client->IsFormatSupported
    (
        cfg.share_mode,
        (WAVEFORMATEX*)&cfg.format,
        (WAVEFORMATEX**)&closest_format
    );

    // 内部変数にコピー
    m_config.share_mode = cfg.share_mode;
    if ( closest_format == nullptr )
    {
        m_config.format = cfg.format;
    }
    else
    {
        m_config.format = *closest_format;

        ::CoTaskMemFree(closest_format);
    }

    return hr;
}

//---------------------------------------------------------------------------//

inline HRESULT tapetums::WASAPI::Device::GetDevicePeriod
(
    REFERENCE_TIME period
)
{
    REFERENCE_TIME def_period;
    REFERENCE_TIME min_period;

    // バッファ処理時間の取得
    const auto hr = m_client->GetDevicePeriod
    (
        &def_period, &min_period
    );

    // 範囲チェック
    m_config.period = period;
    if ( m_config.period == 0 )
    {
        m_config.period = def_period;
    }
    else if ( m_config.period < min_period )
    {
        m_config.period = min_period;
    }
    else if ( m_config.period > 500 * 10 * 1000 )
    {
        m_config.period = 500 * 10 * 1000;
    }

    return hr;
}

//---------------------------------------------------------------------------//

inline HRESULT tapetums::WASAPI::Device::Initialize()
{
    // セッション UUID を生成
    UUID uuid;
    ::CoCreateGuid((GUID*)&uuid);

    // デバイスの初期化
    const auto hr = m_client->Initialize
    (
        m_config.share_mode,
        AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_NOPERSIST,
        m_config.period,
        m_config.share_mode == AUDCLNT_SHAREMODE_EXCLUSIVE ? m_config.period : 0,
        (WAVEFORMATEX*)&m_config.format,
        &uuid
    );

    return hr;
}

//---------------------------------------------------------------------------//

inline HRESULT tapetums::WASAPI::Device::SetEventHandle()
{
    // イベントの生成
    m_evt_empty = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);

    // イベントハンドルの設定
    const auto hr = m_client->SetEventHandle(m_evt_empty);

    return hr;
}

//---------------------------------------------------------------------------//

inline HRESULT tapetums::WASAPI::Device::GetStreamLatency()
{
    // レイテンシの取得
    const auto hr = m_client->GetStreamLatency(&m_latency);

    return hr;
}

//---------------------------------------------------------------------------//

inline HRESULT tapetums::WASAPI::Device::GetBufferSize()
{
    // バッファサイズの取得
    const auto hr = m_client->GetBufferSize(&m_frame_count);

    // 内部バッファを確保
    m_buf_size = m_frame_count * m_config.format.Format.nBlockAlign;
    m_buffer.resize(m_buf_size);

    return hr;
}

//---------------------------------------------------------------------------//

inline HRESULT tapetums::WASAPI::Device::GetService()
{
    // 再生クライアントの取得
    const auto hr = m_client->GetService(IID_PPV_ARGS(&m_renderer));

    return hr;
}

//---------------------------------------------------------------------------//

inline void tapetums::WASAPI::Device::MainLoop()
{
    HRESULT hr;

    // よく使う変数を一時変数にコピー
    const auto renderer_           = m_renderer.Get();
    const auto frame_count_        = m_frame_count;
    const auto evt_empty_          = m_evt_empty;
    const auto buffer_             = m_buffer.data();
    const auto buf_size_           = m_buf_size;
    const auto listener_           = m_listener;
    const auto WM_WASAPI_CALLBACK_ = WM_WASAPI_CALLBACK;

    // デバイスに無音を書き込む
    WriteSilence(renderer_, frame_count_, buf_size_);

    // デバイスの開始
    hr = m_client->Start();
    if ( FAILED(hr) )
    {
        return;
    }

    // デバイスに無音を書き込む
    ::WaitForSingleObject(evt_empty_, INFINITE);
    WriteSilence(renderer_, frame_count_, buf_size_);

    // メインループ
    for ( ; ; )
    {
        // デバイスバッファが空になるのを待つ
        ::WaitForSingleObject(evt_empty_, INFINITE);
        if ( ! m_loop )
        {
            break;
        }

        // デバイスにサウンドデータを書き込む
        hr = WriteBuffer(renderer_, frame_count_, buffer_, buf_size_);
        if ( FAILED(hr) )
        {
            // 共有モードの時はイベントが来ても
            // バッファが取得できない時がある。
            // その時は音を書き出さずにスルー
            continue;
        }

        // 書き込み完了を通知
        ::PostThreadMessage
        (
            listener_, WM_WASAPI_CALLBACK_, WPARAM(buffer_), LPARAM(buf_size_)
        );
    }

    // デバイスの終了
    while ( FAILED(m_client->Stop()) ) { ::Sleep(1); }
}

//---------------------------------------------------------------------------//
// ユーティリティ関数
//---------------------------------------------------------------------------//

inline HRESULT tapetums::WASAPI::WriteSilence
(
    IAudioRenderClient* renderer,
    UINT32              frame_count,
    size_t              size
)
{
    HRESULT hr;
    uint8_t* render_buffer;

    // デバイスに無音を書き込む
    hr = renderer->GetBuffer(frame_count, &render_buffer);
    if ( FAILED(hr) )
    {
        return hr;
    }

    ::memset(render_buffer, 0, size);
    hr = renderer->ReleaseBuffer(frame_count, 0);

    return hr;
}

//---------------------------------------------------------------------------//

inline HRESULT tapetums::WASAPI::WriteBuffer
(
    IAudioRenderClient* renderer,
    UINT32              frame_count,
    const uint8_t*      data,
    size_t              size
)
{
    HRESULT hr;
    uint8_t* render_buffer;

    // デバイスにサウンドデータを書き込む
    hr = renderer->GetBuffer(frame_count, &render_buffer);
    if ( FAILED(hr) )
    {
        return hr;
    }

    ::memcpy(render_buffer, data, size);
    hr = renderer->ReleaseBuffer(frame_count, 0);

    return hr;
}

//---------------------------------------------------------------------------//

// WASAPI.hpp
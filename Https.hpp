#pragma once

//---------------------------------------------------------------------------//
//
// Https.hpp
//  WinHttp による HTTP/2 通信クラス (C++14)
//   Copyright (C) 2018 tapetums
//
//---------------------------------------------------------------------------//

#pragma region USAGE
/******************************************************************************

#include <Https.hpp>

#include <cstdio>  
#include <iostream>

int main()
{
    using namespace tapetums::Https;

    const auto response = SendRequest
    (
        HTTP::METHOD::GET,
        L"https://www.yahoo.co.jp/",
        nullptr, nullptr, 0
    );

    if ( response.header.size() )
    {
        std::wcout << response.header.data() << std::endl;
    }

    if ( response.data.size() )
    {
        FILE* fp;

        if ( 0 == fopen_s(&fp, "index.html", "wb") )
        {
            fwrite(response.data.data(), sizeof(uint8_t), response.data.size(), fp);
            fclose(fp);
        }
    }

    static const auto callback = [](Response&& response, void* usrptr)
    {
        std::wcout << L"HTTP STATUS CODE: " << response.code << std::endl;

        std::wcout << response.url.c_str() << std::endl;

        std::wcout << response.data.size() << L" bytes received." << std::endl;

        ::SetEvent(usrptr);
    };

    const auto event = ::CreateEventW(nullptr, FALSE, FALSE, nullptr);

    PostRequest
    (
        HTTP::METHOD::GET,
        L"https://www.google.com/",
        nullptr, nullptr, 0,
        callback, event
    );

    ::WaitForSingleObject(event, INFINITE);

    ::CloseHandle(event);

    return 0;
}

******************************************************************************/
#pragma endregion

#include <thread>
#include <string>
#include <vector>

#include <windows.h>
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

//---------------------------------------------------------------------------//
// Enumerations
//---------------------------------------------------------------------------//

#undef DELETE

namespace HTTP
{
    enum class METHOD
    {
        GET,
        HEAD,
        POST,
        PUT,
        DELETE,
        CONNECT,
        OPTIONS,
        TRACE,
        PATCH,
    };

    constexpr const wchar_t* METHOD_NAME[9] =
    {
        L"GET",
        L"HEAD",
        L"POST",
        L"PUT",
        L"DELETE",
        L"CONNECT",
        L"OPTIONS",
        L"TRACE",
        L"PATCH",
    };
}

//---------------------------------------------------------------------------//
// Forward Declarations
//---------------------------------------------------------------------------//

namespace tapetums
{
    namespace Https
    {
        struct Response;

        using Callback = void (*)(Response&& response, void* usrptr);

        Response SendRequest
        (
            HTTP::METHOD   method,
            const wchar_t* url,
            const wchar_t* header,
            const void*    data,
            DWORD          size
        );
        void PostRequest
        (
            HTTP::METHOD   method,
            const wchar_t* url,
            const wchar_t* header,
            const void*    data,
            DWORD          size,
            Callback       callback,
            void*          usrptr
        );

        namespace internal
        {
            struct hinternet;

            bool open_session    (hinternet& session);
            bool crack_url       (const wchar_t* url, std::vector<wchar_t>* host, std::vector<wchar_t>* path);
            bool set_option      (hinternet& session);
            bool connect_session (hinternet& session, hinternet& connect, const wchar_t* host);
            bool open_request    (hinternet& connect, hinternet& request, HTTP::METHOD method, const wchar_t* resource);
            bool send_request    (hinternet& request, const wchar_t* header, const void* data, DWORD size);
            bool receive_response(hinternet& request);
            bool query_header    (hinternet& request, DWORD* code, std::vector<wchar_t>* header);
            bool read_data       (hinternet& request, std::vector<uint8_t>* data);
        }
    }
}

//---------------------------------------------------------------------------//
// Structs
//---------------------------------------------------------------------------//

struct tapetums::Https::Response final
{
    DWORD                code;
    std::wstring         url;
    std::vector<wchar_t> header;
    std::vector<uint8_t> data;
};

//---------------------------------------------------------------------------//

struct tapetums::Https::internal::hinternet final
{
    HINTERNET handle { nullptr };

    hinternet() = default;
    ~hinternet() { close(); }

    inline void close()
    {
        if ( handle )
        {
            ::WinHttpCloseHandle(handle);
            handle = nullptr;
        }
    }

    inline void operator =(HINTERNET h)
    {
        close();
        handle = h;
    }

    inline bool operator ==(std::nullptr_t)
    {
        return handle == nullptr;
    }

    inline operator HINTERNET() { return handle; }
};

//---------------------------------------------------------------------------//
// Methods
//---------------------------------------------------------------------------//

inline tapetums::Https::Response tapetums::Https::SendRequest
(
    HTTP::METHOD   method,
    const wchar_t* url,
    const wchar_t* header,
    const void*    data,
    DWORD          size
)
{
    Response response;
    response.code = 0;
    response.url = url;

    for ( auto do_once = true; do_once; do_once = false )
    {
        internal::hinternet session;
        if ( ! internal::open_session(session) )
        {
            break;
        }

        std::vector<wchar_t> host(256);
        std::vector<wchar_t> path(2048);
        if ( ! internal::crack_url(url, &host, &path) )
        {
            response.code = 1;
            break;
        }

        if ( ! internal::set_option(session) )
        {
            response.code = 2;
            break;
        }

        internal::hinternet connect;
        if ( ! internal::connect_session(session, connect, host.data()) )
        {
            response.code = 3;
            break;
        }

        internal::hinternet request;
        if ( ! internal::open_request(connect, request, method, path.data()) )
        {
            response.code = 4;
            break;
        }

        if ( ! internal::send_request(request, header, data, size) )
        {
            response.code = 5;
            break;
        }

        if ( ! internal::receive_response(request) )
        {
            response.code = 6;
            break;
        }

        if ( ! internal::query_header(request, &response.code, &response.header) )
        {
            break;
        }

        if ( ! internal::read_data(request, &response.data) )
        {
            break;
        }
    }

    return response;
}

//---------------------------------------------------------------------------//

inline void tapetums::Https::PostRequest
(
    HTTP::METHOD   method,
    const wchar_t* url,
    const wchar_t* header,
    const void*    data,
    DWORD          size,
    Callback       callback,
    void*          usrptr
)
{
    // Copy Data
    std::wstring u = url ? url : L"";
    std::wstring h = header ? header : L"";

    std::vector<uint8_t> d(size);
    if ( data )
    {
        ::memcpy(d.data(), data, size);
    }

    // Forward Data to Thread
    auto thread = std::thread
    ([method, url = std::move(u), header = std::move(h), data = std::move(d), callback, usrptr]()
    {
        auto response = SendRequest
        (
            method,
            url.c_str(),
            header.c_str(), data.data(), DWORD(data.size())
        );

        if ( callback )
        {
            callback(std::move(response), usrptr);
        }
    });

    thread.detach();
}

//---------------------------------------------------------------------------//
// Internal Methods
//---------------------------------------------------------------------------//

inline bool tapetums::Https::internal::open_session
(
    hinternet& session
)
{
    session = ::WinHttpOpen
    (
        nullptr,
        WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0
    );
    if ( session == nullptr )
    {
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------//

inline bool tapetums::Https::internal::crack_url
(
    const wchar_t*        url,
    std::vector<wchar_t>* host,
    std::vector<wchar_t>* path
)
{
    URL_COMPONENTS url_components { }; // zero clear needed
    url_components.dwStructSize     = sizeof(URL_COMPONENTS);
    url_components.lpszHostName     = host->data();
    url_components.dwHostNameLength = DWORD(host->size());
    url_components.lpszUrlPath      = path->data();
    url_components.dwUrlPathLength  = DWORD(path->size());

    const auto cracked = ::WinHttpCrackUrl
    (
        url, lstrlenW(url), 0, &url_components
    );
    if ( ! cracked )
    {
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------//

inline bool tapetums::Https::internal::set_option
(
    hinternet& session
)
{
    DWORD option { WINHTTP_PROTOCOL_FLAG_HTTP2 };

    const auto http2 = ::WinHttpSetOption
    (
        session,
        WINHTTP_OPTION_ENABLE_HTTP_PROTOCOL,
        &option,
        sizeof(option)
    );
    if ( ! http2 )
    {
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------//

inline bool tapetums::Https::internal::connect_session
(
    hinternet&     session,
    hinternet&     connect,
    const wchar_t* host
)
{
    connect = ::WinHttpConnect
    (
        session,
        host,
        INTERNET_DEFAULT_PORT,
        0
    );
    if ( connect == nullptr )
    {
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------//

inline bool tapetums::Https::internal::open_request
(
    hinternet&     connect,
    hinternet&     request,
    HTTP::METHOD   method,
    const wchar_t* resource
)
{
    request = ::WinHttpOpenRequest
    (
        connect,
        HTTP::METHOD_NAME[size_t(method)],
        resource,
        nullptr,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE
    );
    if ( request == nullptr )
    {
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------//

inline bool tapetums::Https::internal::send_request
(
    hinternet&     request,
    const wchar_t* header,
    const void*    data,
    DWORD          size
)
{
    const auto sent = ::WinHttpSendRequest
    (
        request,
        header,
        lstrlenW(header),
        WINHTTP_NO_REQUEST_DATA,
        0,
        size,
        0
    );
    if ( ! sent )
    {
        return false;
    }

    if ( size )
    {
        const auto written = ::WinHttpWriteData
        (
            request,
            data,
            size,
            nullptr
        );
        if ( ! written )
        {
            return false;
        }
    }

    return true;
}

//---------------------------------------------------------------------------//

inline bool tapetums::Https::internal::receive_response
(
    hinternet& request
)
{
    const auto received = ::WinHttpReceiveResponse
    (
        request,
        nullptr
    );
    if ( ! received )
    {
        return false;
    }

    return true;
}

//---------------------------------------------------------------------------//

inline bool tapetums::Https::internal::query_header
(
    hinternet&            request,
    DWORD*                code,
    std::vector<wchar_t>* header
)
{
    DWORD size;

    size = sizeof(DWORD);
    ::WinHttpQueryHeaders
    (
        request,
        WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX,
        code,
        &size,
        WINHTTP_NO_HEADER_INDEX
    );
    if ( header == nullptr )
    {
        return true;
    }

    size = 0;
    ::WinHttpQueryHeaders
    (
        request,
        WINHTTP_QUERY_RAW_HEADERS_CRLF,
        WINHTTP_HEADER_NAME_BY_INDEX,
        nullptr,
        &size,
        WINHTTP_NO_HEADER_INDEX
    );
    if ( size == 0 )
    {
        return false;
    }

    header->resize(size);
    ::WinHttpQueryHeaders
    (
        request,
        WINHTTP_QUERY_RAW_HEADERS_CRLF,
        WINHTTP_HEADER_NAME_BY_INDEX,
        header->data(),
        &size,
        WINHTTP_NO_HEADER_INDEX
    );

    return true;
}

//---------------------------------------------------------------------------//

inline bool tapetums::Https::internal::read_data
(
    hinternet&            request,
    std::vector<uint8_t>* data
)
{
    if ( data == nullptr )
    {
        return true;
    }

    data->resize(0);

    std::vector<uint8_t> tmp;
    for ( ; ; )
    {
        DWORD tmp_size;
        const auto available = ::WinHttpQueryDataAvailable
        (
            request,
            &tmp_size
        );
        if ( ! available )
        {
            return false;
        }
        if ( tmp_size == 0 )
        {
            break;
        }

        tmp.resize(tmp_size);

        DWORD size;
        ::WinHttpReadData
        (
            request,
            tmp.data(),
            tmp_size,
            &size
        );
        if ( size != tmp_size )
        {
            return false;
        }

        data->insert(data->end(), tmp.begin(), tmp.end());
    }

    return true;
}

//---------------------------------------------------------------------------//

// Https.hpp
#pragma once

//---------------------------------------------------------------------------//
//
// ISocket.hpp
//  通信オブジェクト
//   Copyright (C) 2015-2016 tapetums
//
//---------------------------------------------------------------------------//

#include <cstdint>

#include <array>
#include <thread>

#include <windows.h>

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

//---------------------------------------------------------------------------//

constexpr auto BROADCAST = u8"255.255.255.255";

//---------------------------------------------------------------------------//
// Forward Declarations
//---------------------------------------------------------------------------//

namespace tapetums
{
    struct ISocket;
    class  UDPSocket;
}

//---------------------------------------------------------------------------//
// Classes
//---------------------------------------------------------------------------//

struct tapetums::ISocket
{
    typedef void (__stdcall* Event)
    (
        void* arg, sockaddr_in* addr, uint8_t* data, size_t size
    );

    static ISocket* Builder(IPPROTO protocol);

    ISocket() = default;
    virtual ~ISocket() = 0 { };

    ISocket(const ISocket&) = delete;
    ISocket& operator =(const ISocket&) = delete;

    ISocket(ISocket&&)             noexcept = delete;
    ISocket& operator =(ISocket&&) noexcept = delete;

    virtual bool     is_open()      const = 0;
    virtual bool     is_listening() const = 0;
    virtual SOCKET   socket()       const = 0;
    virtual uint16_t port()         const = 0;
    virtual IPPROTO  protocol()     const = 0;

    virtual bool open(uint16_t port)                                  = 0;
    virtual void close()                                              = 0;
    virtual bool listen(Event on_recv, void* arg = nullptr)           = 0;
    virtual int  sendto(const char* dest, uint8_t* data, size_t size) = 0;
    virtual bool stop()                                               = 0;
};

//---------------------------------------------------------------------------//

class tapetums::UDPSocket : public tapetums::ISocket
{
private:
    SOCKET   socket_ { INVALID_SOCKET };
    uint16_t port_   { 0 };
    bool     loop    { false };

    std::thread thread;

public:
    UDPSocket();
    ~UDPSocket();

public:
    bool     is_open()      const noexcept override { return socket_ != INVALID_SOCKET; }
    bool     is_listening() const noexcept override { return loop;                      }
    SOCKET   socket()       const noexcept override { return socket_;                   }
    uint16_t port()         const noexcept override { return port_;                     }
    IPPROTO  protocol()     const noexcept override { return IPPROTO::IPPROTO_UDP;      }

    bool open(uint16_t port)                                  override;
    void close()                                              override;
    int  sendto(const char* dest, uint8_t* data, size_t size) override;
    bool listen(Event on_receive, void* arg = nullptr)        override;
    bool stop()                                               override;
};

//---------------------------------------------------------------------------//
// ISocket static Method
//---------------------------------------------------------------------------//

inline tapetums::ISocket* tapetums::ISocket::Builder(IPPROTO protocol)
{
    switch ( protocol )
    {
        case IPPROTO::IPPROTO_TCP:
        {
            return nullptr;
        }
        case IPPROTO::IPPROTO_UDP:
        {
            return new UDPSocket();
        }
        default:
        {
            return nullptr;
        }
    }
}

//---------------------------------------------------------------------------//
// UDPSocket Worker Thread
//---------------------------------------------------------------------------//

static inline void udp_thread
(
    tapetums::ISocket* socket, tapetums::ISocket::Event on_recv, void* arg
)
{
    if ( nullptr == socket  ) { return; }
    if ( nullptr == on_recv ) { return; }

    const auto soc  = socket->socket();
    const auto port = socket->port();

    sockaddr_in addr { };
    addr.sin_family           = AF_INET;
    addr.sin_port             = htons(port);
    addr.sin_addr.S_un.S_addr = INADDR_ANY;
    const auto ret = ::bind(soc, (sockaddr*)&addr, (int)sizeof(addr));
    if ( ret != 0 )
    {
        return;
    }

    std::array<uint8_t, 2048> buf;
    auto len = (int)sizeof(sockaddr_in);

    while ( socket->is_listening() )
    {
        const auto size = ::recvfrom
        (
            soc, (char*)buf.data(), (int)buf.size(), 0,
            (sockaddr*)&addr, &len
        );
        if ( size > 0 )
        {
            on_recv(arg, &addr, buf.data(), size);
        }
        else
        {
            // stop command
        }
    }
}

//---------------------------------------------------------------------------//
// UDPSocket ctor / dtor
//---------------------------------------------------------------------------//

inline tapetums::UDPSocket::UDPSocket()
{
    WSADATA wsaData;
    const auto ret = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
    if ( ret != 0 )
    {
        ::MessageBox(nullptr, TEXT("WSAStartup() failed"), TEXT(""), MB_OK);
    }
}

//---------------------------------------------------------------------------//

inline tapetums::UDPSocket::~UDPSocket()
{
    if ( loop )
    {
        stop();
    }
    if ( is_open() )
    {
        close();
    }

    const auto ret = ::WSACleanup();
    if ( ret != 0 )
    {
        ::MessageBox(nullptr, TEXT("WSACleanup() failed"), TEXT(""), MB_OK);
    }
}

//---------------------------------------------------------------------------//
// UDPSocket Methods
//---------------------------------------------------------------------------//

#define do_once for ( auto once__ = true; once__; once__ = false )

inline bool tapetums::UDPSocket::open
(
    uint16_t port
)
{
    if ( socket_ != INVALID_SOCKET ) { return true; }

    int ret = 0;
    int on = 1;

    do_once
    {
        if ( is_open() )
        {
            ret = 0;
            break;
        }

        socket_ = ::socket
        (
            AF_INET, SOCK_DGRAM, protocol()
        );
        if ( socket_ == INVALID_SOCKET )
        {
            ret = ::WSAGetLastError();
            break;
        }

        ret = ::setsockopt
        (
            socket_, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)
        );
        if ( ret == SOCKET_ERROR )
        {
            close();
            break;
        }

        ret = ::setsockopt
        (
            socket_, SOL_SOCKET, SO_BROADCAST, (char*)&on, sizeof(on)
        );
        if ( ret == SOCKET_ERROR )
        {
            close();
            break;
        }

        port_ = port;
    }

    return ret == 0;
}

#undef do_once

//---------------------------------------------------------------------------//

inline void tapetums::UDPSocket::close()
{
    if ( socket_ == INVALID_SOCKET ) { return; }

    port_ = 0;

    ::closesocket(socket_);
    socket_ = INVALID_SOCKET;

    return;
}

//---------------------------------------------------------------------------//

#pragma warning(push)
#pragma warning(disable: 4996)

inline int tapetums::UDPSocket::sendto
(
    const char* dest, uint8_t* data, size_t size
)
{
    if ( socket_ == INVALID_SOCKET )
    {
        return false;
    }

    sockaddr_in addr { };
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    addr.sin_addr.S_un.S_addr = inet_addr(dest);
    if ( 0xffffffff == addr.sin_addr.S_un.S_addr )
    {
        const auto host = ::gethostbyname(dest);
        if ( nullptr == host )
        {
            return 0;
        }
        addr.sin_addr.S_un.S_addr = *(uint32_t*)host->h_addr_list[0];
    }

    const auto ret = ::sendto
    (
        socket_, (char*)data, (int)size, 0, (sockaddr*)&addr, sizeof(addr)
    );

    return ret;
}

#pragma warning(pop)

//---------------------------------------------------------------------------//

inline bool tapetums::UDPSocket::listen
(
    Event on_recv, void* arg
)
{
    if ( loop ) { return false; }

    loop = true;
    thread = std::thread(udp_thread, this, on_recv, arg);

    return true;
}

//---------------------------------------------------------------------------//

inline bool tapetums::UDPSocket::stop()
{
    if ( ! loop ) { return false; }

    loop = false;
    if ( thread.joinable() )
    {
        if ( 0 == sendto("localhost", nullptr, 0) )
        {
            thread.join(); // スレッドの終了を待つ
        }
        else
        {
            thread.detach(); // 強制的に終了
            ::Sleep(100);
        }
    }

    return true;
}

//---------------------------------------------------------------------------//

// ISocket.hpp
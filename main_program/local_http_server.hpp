#pragma once

#if 1

#include <cstdint>          // uint16_t
#include <atomic>           // atomic<bool>
#include <mutex>            // mutex
#include <string>           // string
#include <string_view>      // string_view
#include <thread>           // jthread

class LocalHttpServer {
protected:
    bool is_listening = false;
    bool use_ipv6 = false;
    std::uint16_t port{ 0u };
    int server_fd = -1;
    std::mutex m;
    std::string html_str;
    std::jthread t;
    std::atomic<bool> stop_flag{ false };

    bool Start(std::uint16_t port_wanted = 0u) noexcept;
    void Stop(void) noexcept;
    static void ThreadEntryPoint(LocalHttpServer*);
public:
    void SetFlagToFetchPage(void) noexcept {}
    void DiscardAllPendingConnections(void) noexcept {}
    bool IsListening(void) const noexcept { return is_listening; }
    bool IsUsingIPv6(void) const noexcept { return use_ipv6; }
    std::uint16_t GetListeningPort(void) const noexcept { return port; }

    LocalHttpServer() noexcept;
    ~LocalHttpServer() noexcept;

    void SetHtmlCode(std::string_view) noexcept;
};

#elif 0

#include <cstdint>                  // uint16_t
#include <mutex>                    // mutex
#include <string>                   // string
#include <string_view>              // string_view
#include <wx/event.h>               // wxEvtHandler
#include <wx/socket.h>              // wxSocketServer, wxSocketBase
#include <wx/string.h>              // wxString

class LocalHttpServer : public wxEvtHandler {
public:
    using string      = std::string;
    using string_view = std::string_view;

    LocalHttpServer(void) noexcept;
   ~LocalHttpServer(void) noexcept;

    bool            IsListening(void)     const noexcept { return this->is_listening; }
    bool            IsUsingIPv6(void)           const    { return false; }
    std::uint16_t   GetListeningPort(void) const noexcept { return this->port; }

    void SetHtmlCode(string const &html);
    void DiscardAllPendingConnections(void) noexcept;
    void SetFlagToFetchPage(void) noexcept { /* single-threaded: no-op */ }

private:
    bool Start(unsigned const port = 0u) noexcept;
    void Stop(void) noexcept;

    // wx event handlers (single-threaded, main loop)
    void OnServerEvent(wxSocketEvent &event) noexcept;
    void OnSocketEvent(wxSocketEvent &event) noexcept;

    // helpers
    static bool       ParseHttpGetTarget(string_view const req, string &out_target) noexcept;
    static void       MakeBadRequestResponse(wxSocketBase *const sock) noexcept;
    static void       MakeMethodNotAllowedResponse(wxSocketBase *const sock) noexcept;
    static void       MakeTimeoutResponse(wxSocketBase *const sock) noexcept;
    static void       SendResponse(wxSocketBase *const sock, char const *const content_type, string const &body) noexcept;
    static char const*FindContentTypeFromExtension(string const &ext) noexcept;

    // state
    wxSocketServer *server = nullptr;
    std::uint16_t   port{ 0u };
    bool            is_listening = false;
    string          html_str;
    std::mutex      m;
};

#else

#include <cstdint>                   // uint16_t
#include <atomic>                    // atomic
#include <mutex>                     // mutex
#include <optional>                  // optional
#include <string>                    // string
#include <thread>                    // jthread
#include <boost/asio.hpp>            // io_context, ip::tcp::acceptor

class LocalHttpServer {
protected:
    bool is_listening{ false };
    bool use_ipv6{ false };
    std::uint16_t port{ 0u };
    std::optional<boost::asio::io_context> opt_ioc;
    std::optional<boost::asio::ip::tcp::acceptor> opt_acceptor;
    std::jthread t;
    std::mutex m;
    std::string html_str;
    std::atomic<unsigned> flag{ 0u };
public:
    bool IsListening(void) const noexcept { return this->is_listening; }
    bool IsUsingIPv6(void) const noexcept { return this->use_ipv6; }
    std::uint16_t GetListeningPort(void) const noexcept { return this->port; }
    LocalHttpServer(void) noexcept;
    bool TryServeWebpage(void) noexcept;
    void DiscardAllPendingConnections(void) noexcept;
    void SetHtmlCode(std::string_view const sv) noexcept;
    void SetFlagToFetchPage(void) noexcept
    {
        extern bool wxIsMainThread(void);
        assert( wxIsMainThread() );
        this->flag = 1u;
        this->flag.notify_one();
    }
    ~LocalHttpServer(void) noexcept
    {
        extern bool wxIsMainThread(void);
        assert( wxIsMainThread() );
        this->flag = 666u;
        this->flag.notify_one();
    }
};

#endif


#include "local_http_server.hpp"
#include <utility>
#include "Auto.h"

static constexpr std::pair<char const*, char const*> g_content_types[] = {
    { "html", "text/html"       },
    { "htm" , "text/html"       },
    { "txt" , "text/plain"      },
    { "pdf" , "application/pdf" },
    { "md"  , "text/markdown"   },

    // REVISIT FIX - also needed a few more -- one beginning with 'p' can't remember
};

#if 1

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <string_view>

#if defined(_WIN32) || defined(_WIN64)
  #include <winsock2.h>
  #include <ws2tcpip.h>
  typedef int socklen_t;
#else
  #include <arpa/inet.h>
  #include <netinet/in.h>
  #include <sys/socket.h>
  #include <unistd.h>
  #include <cerrno>
#endif

#include "embedded_archive.hpp"
#include "html_pages_hardcoded.hpp"

using std::string, std::string_view;

static void sockClose(int const fd) noexcept
{
#if defined(_WIN32) || defined(_WIN64)
    ::closesocket(fd);
#else
    ::close(fd);
#endif
}

static void sockShutdown(int const fd) noexcept
{
#if defined(_WIN32) || defined(_WIN64)
    ::shutdown(fd SD_BOTH);
#else
    ::shutdown(fd, SHUT_RDWR);
#endif
}

static ssize_t sockRead(int fd, void *buf, size_t len) noexcept
{
#if defined(_WIN32) || defined(_WIN64)
    int const r = ::recv(fd, (char*)buf, static_cast<int>(len), 0);
    return (r < 0) ? 0 : r;
#else
    return ::read(fd, buf, len);
#endif
}

ssize_t sockWrite(int fd, const void *buf, size_t len) noexcept
{
#if defined(_WIN32) || defined(_WIN64)
    int const r = ::send(fd, (const char*)buf, static_cast<int>(len), 0);
    return (r < 0) ? 0 : r;
#else
    return ::write(fd, buf, len);
#endif
}

void LocalHttpServer::Stop(void) noexcept
{
    std::fprintf(stderr, "Entered LocalHttpServer::Stop\n");
    Auto( std::fprintf(stderr, "Left LocalHttpServer::Stop\n") );

    this->stop_flag = true;

    if ( this->server_fd >= 0 )
    {
        sockShutdown(this->server_fd);  // This causes 'accept' to return
        sockClose(this->server_fd);
    }

    this->server_fd = -1;
    this->is_listening = false;
    this->port = 0u;

    std::fprintf(stderr, "About to join. . .\n");
    if ( this->t.joinable() ) t.join();
    std::fprintf(stderr, "Joined.\n");

    this->stop_flag = false;
}

LocalHttpServer::~LocalHttpServer(void) noexcept
{
    this->Stop();
}

bool LocalHttpServer::Start(std::uint16_t const port_wanted) noexcept
{
    this->Stop();

    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if ( fd >= 0 )
    {
        int opt = 1;
        ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        struct sockaddr_in addr4{};
        addr4.sin_family = AF_INET;
        addr4.sin_addr.s_addr = INADDR_ANY;
        addr4.sin_port = htons(port_wanted);

        if ( ::bind(fd, reinterpret_cast<struct sockaddr*>(&addr4), sizeof(addr4)) < 0 )
        {
            sockClose(fd);
            fd = -1;
        }
    }

    if ( fd < 0 )  // If IPv4 failed, try IPv6
    {
        fd = ::socket(AF_INET6, SOCK_STREAM, 0);
        if ( fd < 0 )
        {
            std::perror("socket");
            return false;
        }

        int opt = 1;
        ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        struct sockaddr_in6 addr6{};
        addr6.sin6_family = AF_INET6;
        addr6.sin6_addr = in6addr_any;
        addr6.sin6_port = htons(port_wanted);

        ::setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt));  // Allow dual-stack if possible

        if ( ::bind(fd, reinterpret_cast<struct sockaddr*>(&addr6), sizeof(addr6)) < 0 )
        {
            sockClose(fd);
            std::perror("bind");
            return false;
        }

        this->use_ipv6 = true;
    }

    // Retrieve actual bound port number
    if ( this->use_ipv6 )
    {
        struct sockaddr_in6 addr6{};
        socklen_t len = sizeof(addr6);
        if ( ::getsockname(fd, reinterpret_cast<struct sockaddr*>(&addr6), &len) < 0 )
        {
            std::perror("getsockname");
            sockClose(fd);
            return false;
        }
        this->port = ntohs(addr6.sin6_port);
    }
    else
    {
        struct sockaddr_in addr4{};
        socklen_t len = sizeof(addr4);
        if ( ::getsockname(fd, reinterpret_cast<struct sockaddr*>(&addr4), &len) < 0 )
        {
            std::perror("getsockname");
            sockClose(fd);
            return false;
        }
        this->port = ntohs(addr4.sin_port);
    }

    if ( ::listen(fd, 16) < 0 )
    {
        std::perror("listen");
        sockClose(fd);
        return false;
    }

    this->server_fd = fd;
    this->is_listening = true;
    this->stop_flag = false;

    // Spawn background thread
    this->t = std::jthread( ThreadEntryPoint, this );

    return true;
}

LocalHttpServer::LocalHttpServer(void) noexcept
{
    this->Start();
}

static bool ParseHttpGetTarget(string_view const req, string &out_target) noexcept
{
    size_t line_end = req.find("\r\n");
    if ( line_end == string_view::npos ) return false;

    string_view line = req.substr(0, line_end);
    constexpr string_view prefix = "GET ";
    if ( false == line.starts_with(prefix) ) return false;

    size_t space_pos = line.find(' ', prefix.size());
    if ( space_pos == string_view::npos ) return false;

    out_target.assign(line.substr(prefix.size(), space_pos - prefix.size()));
    return true;
}

static string FindContentTypeFromExtension(string const &ext) noexcept
{
    for ( auto const &ct : g_content_types )
    {
        if ( ext == ct.first ) return ct.second;
    }
    return "text/plain";
}

static void SendHttpResponse(int sock, const char *content_type, string const &body) noexcept
{
    static thread_local char header[512u];
    std::memset(header, 0, sizeof header);
    int n = std::snprintf(header, sizeof header,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n\r\n",
        content_type, body.size());

    if ( n > 0 ) sockWrite(sock, header, n);

    if ( false == body.empty() ) sockWrite(sock, body.data(), body.size());
}

void LocalHttpServer::SetHtmlCode(std::string_view const sv) noexcept
{
    std::lock_guard<std::mutex> lock(  this->m  );
    this->html_str.assign(sv);
}

/* static */ void LocalHttpServer::ThreadEntryPoint(LocalHttpServer *const pthis)
{
    while ( false == pthis->stop_flag )
    {
        struct sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int const client_fd = ::accept(pthis->server_fd,
                                       reinterpret_cast<struct sockaddr*>(&client_addr),
                                       &client_len);

        std::fprintf(stderr, "accept has returned.\n");

        if ( client_fd < 0 )
        {
#if defined(_WIN32) || defined(_WIN64)
            if ( WSAEINTR == WSAGetLastError() ) continue;
#else
            if ( EINTR == errno ) continue;
#endif
            std::perror("accept");
            break;
        }

        Auto( sockClose(client_fd) );

        static thread_local char buffer[4096U];
        std::memset(buffer, 0, sizeof buffer);
        ssize_t const n = sockRead(client_fd, buffer, sizeof(buffer)-1);
        if ( n <= 0 ) continue;

        string_view request(buffer, n);
        string target;
        if ( !ParseHttpGetTarget(request, target) || target.empty() || (target[0] != '/') )
        {
            SendHttpResponse(client_fd, "text/plain", "Bad Request");
            continue;
        }

        target.erase(0u,1u);  // remove leading /

        string extension, body;
        if ( target == "html" )
        {
            extension = "html";
            std::lock_guard<std::mutex> lock(pthis->m);
            body = pthis->html_str.empty() ? "<html><body>Empty</body></html>" : pthis->html_str;
        }
        else if ( target == "loading_all" )
        {
            body = html_loading_all_papers;
            extension = "html";
        }
        else
        {
            body = ArchiveGetFile(target, extension, true);
        }

        if ( body.empty() || extension.empty() )
        {
            SendHttpResponse(client_fd, "text/plain", "Bad Request");
            continue;
        }

        auto const tmp = FindContentTypeFromExtension(extension);
        char const *const ctype = tmp.c_str();
        SendHttpResponse(client_fd, ctype, body);
    }
}

#elif 0

#include <cstddef>                    // size_t
#include <cstdio>                     // snprintf
#include <cstdlib>                    // malloc, free
#include <cstring>                    // memchr
#include <algorithm>                  // min
#include <new>                        // nothrow
#include <string>                     // string, to_string
#include <string_view>                // string_view
#include <utility>                    // pair
#include <vector>                     // vector
#include <wx/log.h>                   // wxLogMessage, wxLogError
#include <wx/socket.h>                // wxSocket*, wxSOCKET_*
#include "embedded_archive.hpp"       // ArchiveGetFile
#include "Auto.h"                     // Auto
#include "html_pages_hardcoded.hpp"   // html_loading_all_papers

using std::size_t, std::string, std::string_view;

LocalHttpServer::LocalHttpServer(void) noexcept
{
    this->Start();
}

LocalHttpServer::~LocalHttpServer(void) noexcept
{
    this->Stop();
}

bool LocalHttpServer::Start(unsigned const port_wanted) noexcept
{
    this->Stop();

    wxIPV4address addr;
    addr.Service(port_wanted);

    this->server = new(std::nothrow) wxSocketServer(addr, wxSOCKET_NOWAIT);
    if ( nullptr == this->server )
    {
        wxLogError("LocalHttpServer: allocation failed.");
        return false;
    }

    if ( false == this->server->IsOk() )
    {
        wxLogError("LocalHttpServer: failed to bind/listen on port %u", static_cast<unsigned>(port_wanted));
        this->server->Destroy();
        this->server = nullptr;
        return false;
    }

    // Set event handler and bind the server connection event
    this->server->SetEventHandler(*this);
    this->server->SetNotify(wxSOCKET_CONNECTION_FLAG);
    this->server->Notify(true);

    this->Bind(wxEVT_SOCKET, &LocalHttpServer::OnServerEvent, this);

    // Discover actual bound port when 0u was used.
    wxIPV4address bound;
    this->server->GetLocal(bound);
    this->port = static_cast<std::uint16_t>(bound.Service());
    if ( 0u == this->port )
    {
        wxLogError("LocalHttpServer: invalid port (0) after bind.");
        this->Stop();
        return false;
    }

    this->is_listening = true;
    return true;
}

void LocalHttpServer::Stop(void) noexcept
{
    if ( nullptr != this->server )
    {
        this->server->Notify(false);
        this->server->Destroy();
        this->server = nullptr;
    }

    this->is_listening = false;
    this->port = 0u;
}

void LocalHttpServer::SetHtmlCode(string const &html)
{
    std::lock_guard<std::mutex> mylock(this->m);
    this->html_str = html;
}

void LocalHttpServer::DiscardAllPendingConnections(void) noexcept
{
    return;

    if ( nullptr == this->server ) return;

    for (;;)
    {
        wxSocketBase *const s = this->server->Accept(false);
        if ( nullptr == s ) break;
        s->Close();
        s->Destroy();
    }
}

void LocalHttpServer::OnServerEvent(wxSocketEvent &event) noexcept
{
    std::fprintf(stderr, "Entered LocalHttpServer::OnServerEvent\n");
    Auto( std::fprintf(stderr, "Left LocalHttpServer::OnServerEvent\n") );

    if ( wxSOCKET_CONNECTION != event.GetSocketEvent() ) return;

    wxSocketBase *const client = this->server ? this->server->Accept(false) : nullptr;
    if ( nullptr == client ) return;

    client->SetEventHandler(*this);
    client->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
    client->Notify(true);
    server->Notify(true);
}

void LocalHttpServer::OnSocketEvent(wxSocketEvent &event) noexcept
{
    std::fprintf(stderr, "Entered LocalHttpServer::OnSocketEvent\n");
    Auto( std::fprintf(stderr, "Left LocalHttpServer::OnSocketEvent\n") );

    wxSocketBase *const sock = event.GetSocket();
    if ( nullptr == sock ) return;

    switch ( event.GetSocketEvent() )
    {
    case wxSOCKET_INPUT:
    {
        static thread_local unsigned char buf[4096u]{};
        sock->Read(buf, sizeof(buf) - 1u);
        size_t const n = sock->LastCount();

        if ( 0u == n )
        {
            MakeBadRequestResponse(sock);
            sock->Close();
            sock->Destroy();
            return;
        }

        string const request(reinterpret_cast<char const*>(buf), n);

        string target;
        if ( false == ParseHttpGetTarget(request, target) )
        {
            if ( 0u == request.find("GET ") ) MakeBadRequestResponse(sock);
            else                              MakeMethodNotAllowedResponse(sock);
            sock->Close();
            sock->Destroy();
            return;
        }

        if ( true == target.empty() || '/' != target[0] )
        {
            MakeBadRequestResponse(sock);
            sock->Close();
            sock->Destroy();
            return;
        }

        string_view sv = string_view(target).substr(1u);
        if ( sv.empty() )
        {
            MakeBadRequestResponse(sock);
            sock->Close();
            sock->Destroy();
            return;
        }

        string extension;
        string body;
        char const *ctype = "text/plain";

        if ( "html" == sv )
        {
            extension = "html";
            std::lock_guard<std::mutex> mylock(this->m);
            body = this->html_str.empty() ? "<html><body>Empty</body></html>" : this->html_str;
        }
        else if ( "loading_all" == sv )
        {
            body = html_loading_all_papers;
            extension = "html";
        }
        else
        {
            body = ArchiveGetFile(sv, extension, true);
        }

        if ( body.empty() || extension.empty() )
        {
            MakeBadRequestResponse(sock);
            sock->Close();
            sock->Destroy();
            return;
        }

        ctype = FindContentTypeFromExtension(extension);
        SendResponse(sock, ctype, body);
        sock->Close();
        sock->Destroy();
        return;
    }

    case wxSOCKET_LOST:
        sock->Destroy();
        return;

    default:
        return;
    }
}

bool LocalHttpServer::ParseHttpGetTarget(string_view const req, string &out_target) noexcept
{
    // Expect: "GET /path HTTP/1.1\r\n"
    // Find first line
    size_t const line_end = req.find("\r\n");
    if ( string_view::npos == line_end ) return false;

    string_view const line = req.substr(0u, line_end);

    // Method
    char const meth[] = "GET ";
    if ( 0u != line.find(meth) ) return false;

    // After "GET "
    string_view rest = line.substr(sizeof(meth) - 1u);
    size_t const sp = rest.find(' ');
    if ( string_view::npos == sp ) return false;

    string_view const target = rest.substr(0u, sp);
    out_target.assign(target.begin(), target.end());
    return true;
}

void LocalHttpServer::MakeBadRequestResponse(wxSocketBase *const sock) noexcept
{
    static char const body[] = "Bad Request";
    SendResponse(sock, "text/plain", string(body, sizeof(body) - 1u));
}

void LocalHttpServer::MakeMethodNotAllowedResponse(wxSocketBase *const sock) noexcept
{
    // Minimal 405
    char const *const ctype = "text/plain";
    string const body = "Method Not Allowed";
    if ( nullptr == sock ) return;

    // Manual write (short path)
    char header[256u];
    int const n = std::snprintf(header, sizeof(header),
        "HTTP/1.1 405 Method Not Allowed\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n\r\n",
        ctype, body.size());
    if ( n > 0 )
    {
        sock->Write(header, static_cast<size_t>(n));
        sock->Write(body.data(), body.size());
    }
}

void LocalHttpServer::MakeTimeoutResponse(wxSocketBase *const sock) noexcept
{
    static char const body[] = "Timeout";
    SendResponse(sock, "text/plain", string(body, sizeof(body) - 1u));
}

void LocalHttpServer::SendResponse(wxSocketBase *const sock, char const *const content_type, string const &body) noexcept
{
    if ( nullptr == sock ) return;

    // Build minimal HTTP/1.1 response with Connection: close
    char header[256u];
    int const n = std::snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n\r\n",
        content_type ? content_type : "application/octet-stream",
        body.size());

    if ( n > 0 )
    {
        sock->Write(header, static_cast<size_t>(n));
        if ( 0u != body.size() ) sock->Write(body.data(), body.size());
    }
}

char const *LocalHttpServer::FindContentTypeFromExtension(string const &ext) noexcept
{
    for ( auto const &ct : g_content_types )
        if ( 0 == ext.compare(ct.first) )
            return ct.second;

    return "text/plain";
}

#else

#include <cstdio>                    // fprintf, stderr
#include <cstdlib>                   // _Exit
#include <iostream>                  // cerr, endl
#include <stdexcept>                 // runtime_error
#include <string>                    // string
#include <string_view>               // string_view
#include <boost/beast.hpp>           // flat_buffer, http::request, http::response, http::read, http::write
#include "embedded_archive.hpp"      // ArchiveGetFile
#include "_Max.hpp"
#include "Auto.h"

using std::string, std::string_view;
namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

LocalHttpServer::LocalHttpServer(void) noexcept
{
    try
    {
        this->opt_ioc.emplace(1);
        auto &ioc = this->opt_ioc.value();
        this->opt_acceptor.emplace(ioc);
        auto &acceptor = this->opt_acceptor.value();
        try
        {
            // Try IPv4 first
            tcp::endpoint endpoint4(tcp::v4(), 0);
            acceptor.open(endpoint4.protocol());
            acceptor.bind(endpoint4);
        }
        catch(...)
        {
            // Fallback to IPv6
            tcp::endpoint endpoint6(tcp::v6(), 0);
            acceptor.open(endpoint6.protocol());
            acceptor.set_option(net::ip::v6_only(false));
            acceptor.bind(endpoint6);
            this->use_ipv6 = true;
        }
        acceptor.listen();
        this->port = acceptor.local_endpoint().port();   // Get the port actually assigned
        if ( _Max == this->port ) this->port = 0u;
        if ( 0u == this->port ) throw std::runtime_error("invalid TCP listening port number");
        this->is_listening = true;

        t = std::jthread([this]()
          {
                for (; /* ever */ ;)
                {
                    this->flag = 0u;
                    this->flag.wait(0u);
                    if ( 666u == this->flag ) return;
                    auto const timestamp = std::chrono::steady_clock::now();
                    for (; /* ever */ ;)
                    {
                        if ( (std::chrono::steady_clock::now() - timestamp) > std::chrono::seconds(10u) )
                        {
                            std::fputs("Timeout when waiting for incoming HTTP connection.\n", stderr);
                            break;
                        }
                        if ( this->TryServeWebpage() ) break;
                    }
                }
          });
    }
    catch(std::exception const &e)
    {
        std::fprintf( stderr, "LocalHttpServer constructor caught exception: %s\n", e.what() );
    }
    catch(...)
    {
        std::fprintf( stderr, "LocalHttpServer constructor caught unknown exception\n" );
    }
}

bool LocalHttpServer::TryServeWebpage(void) noexcept
{
    std::fprintf(stderr, "Entered LocalHttpServer::TryServeWebpage\n");
    Auto( std::fprintf(stderr, "Left LocalHttpServer::TryServeWebpage\n") );

    struct invalid_url_t {};

    try
    {
        assert( this->is_listening             );
        assert( this->opt_ioc.has_value()      );
        assert( this->opt_acceptor.has_value() );
        auto &ioc      = this->opt_ioc.value();
        auto &acceptor = this->opt_acceptor.value();

        tcp::socket socket{ ioc };
        boost::system::error_code ec;
        acceptor.non_blocking(true);
        acceptor.accept(socket, ec);
        if ( boost::asio::error::would_block == ec )
        {
            std::fprintf(stderr, "  Inside LocalHttpServer::TryServeWebpage - No pending connections.\n");
            return false;
        }

        std::fprintf(stderr, "  Inside LocalHttpServer::TryServeWebpage - Connection Made.\n");

        beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::read(socket, buffer, req);
        try
        {
            std::string s = string( req.target() ) + '.';
            std::fprintf( stderr, "Inside LocalHttpServer::TryServeWebpage, URL is '%s'\n", s.c_str() );
            std::string_view sv = s;
            if ( "." == sv ) throw invalid_url_t();
            if ( false == sv.starts_with('/') ) throw invalid_url_t();
            sv.remove_prefix(1u);  // remove leading forward slash
            if ( sv.empty() ) throw invalid_url_t();
            string extension;
            string file_binary_contents;
            if ( "html." == sv )
            {
                extension = "html";
                std::lock_guard mylock( this->m );
                file_binary_contents = this->html_str;
            }
            else if ( "loading_all." == sv )
            {
                extern std::string const html_loading_all_papers;  /* defined in html_pages_hardcoded.hpp */
                file_binary_contents = html_loading_all_papers;
                extension = "html";
            }
            else
            {
                file_binary_contents = ArchiveGetFile(sv, extension, true);
            }
            if ( extension.empty() || file_binary_contents.empty() ) throw invalid_url_t();
            http::response< http::string_body > res{ http::status::ok, req.version() };
            res.set(http::field::server, "PaperKernelCxx.Boost.Beast");
            char const *content_type = "text/plain";
            for ( auto const &ct : g_content_types )
            {
                if ( ct.first == extension )
                {
                    content_type = ct.second;  // cppcheck-suppress useStlAlgorithm
                    break;
                }
            }
            res.set(http::field::content_type, content_type);
            res.body() = file_binary_contents;
            res.prepare_payload();
            http::write(socket, res);
        }
        catch(invalid_url_t const&)
        {
            http::response<http::string_body> res{ http::status::bad_request, 11 };
            res.set(http::field::server, "PaperKernelCxx.Boost.Beast");
            res.set(http::field::content_type, "text/plain");
            res.body() = "Unable to load paper";
            res.prepare_payload();
            http::write(socket, res);
        }
    }
    catch(...) { return false; }

    return true;
}

void LocalHttpServer::DiscardAllPendingConnections(void) noexcept
{
    try
    {
        assert( this->is_listening );
        assert( this->opt_acceptor.has_value() );
        assert( this->opt_ioc     .has_value() );
        auto &acceptor = this->opt_acceptor.value();
        auto &ioc = this->opt_ioc.value();
        boost::system::error_code ec;
        for (; /* ever */ ;)
        {
            tcp::socket socket(ioc);
            acceptor.non_blocking(true);
            acceptor.accept(socket, ec);
            if ( boost::asio::error::would_block == ec ) break;
        }
    }
    catch(...)
    {
        std::fprintf(stderr, "LocalHttpServer::DiscardAllPendingConnections threw and caught an exception\n");
    }
}

void LocalHttpServer::SetHtmlCode(std::string_view const sv) noexcept
{
    std::lock_guard mylock( this->m );
    this->html_str = sv;
}

#endif

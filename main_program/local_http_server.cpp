#include "local_http_server.hpp"
#include <cstdio>                    // fprintf, stderr
#include <cstdlib>                   // _Exit
#include <iostream>                  // cerr, endl
#include <stdexcept>                 // runtime_error
#include <string>                    // string
#include <string_view>               // string_view
#include <boost/beast.hpp>           // flat_buffer, http::request, http::response, http::read, http::write
#include "embedded_archive.hpp"      // ArchiveGetFile

static constexpr std::pair<char const*, char const*> g_content_types[] = {
    { "html", "text/html"       },
    { "htm" , "text/html"       },
    { "txt" , "text/plain"      },
    { "pdf" , "application/pdf" },
    { "md"  , "text/markdown"   },
};

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
        if ( 0xFFFF == this->port ) this->port = 0u;
        if ( 0u == this->port ) throw std::runtime_error("invalid TCP listening port number");
        this->is_listening = true;
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

void LocalHttpServer::ThreadEntryPoint_NotEternal(void) noexcept(false)
{
    struct invalid_url_t {};

    assert( this->is_listening             );
    assert( this->opt_ioc.has_value()      );
    assert( this->opt_acceptor.has_value() );
    auto &ioc      = this->opt_ioc.value();
    auto &acceptor = this->opt_acceptor.value();

    while ( false == this->death_warrant )
    {
        tcp::socket socket{ ioc };
        acceptor.accept(socket);
        if ( this->death_warrant ) return;
        beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::read(socket, buffer, req);
        try
        {
            std::string s = string( req.target() ) + '.';
            std::string_view sv = s;
            if ( sv.empty() ) throw invalid_url_t();
            if ( false == sv.starts_with('/') ) throw invalid_url_t();
            sv.remove_prefix(1u);  // remove leading forward slash
            if ( sv.empty() ) throw invalid_url_t();
            string extension;
            string const file_binary_contents = ArchiveGetFile(sv, extension, true);
            if ( extension.empty() || file_binary_contents.empty() ) throw invalid_url_t();
            http::response< http::string_body > res{ http::status::ok, req.version() };
            res.set(http::field::server, "PaperKernelCxx.Boost.Beast");
            char const *content_type = "text/plain";
            for ( auto const &ct : g_content_types )
            {
                if ( ct.first == extension )
                {
                    content_type = ct.second;
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
}

void LocalHttpServer::ThreadEntryPoint(void) noexcept
{
    while ( false == this->death_warrant )
    {
        try
        {
            this->ThreadEntryPoint_NotEternal();
        }
        catch (std::exception const &e)
        {
            std::fprintf( stderr, "LocalHttpServer caught exception: %s\n", e.what() );
        }
        catch(...)
        {
            std::fprintf( stderr, "LocalHttpServer caught unknown exception\n" );
        }
    }

    this->death_warrant = false;
}

std::uint16_t LocalHttpServer::StartAccepting(void) noexcept
{
    if ( (false==this->is_listening) || death_warrant ) return 0u;
    if ( this->server_thread.joinable() ) return this->port;
    this->server_thread = std::jthread( [this]{ this->ThreadEntryPoint(); } );
    return this->port;
}

LocalHttpServer::~LocalHttpServer(void) noexcept
{
    std::puts("First line in destructor of LocalHttpServer");
    this->death_warrant = true;
    if ( (false == this->is_listening) || (0u == this->port) || (0xFFFF == this->port) || (false == this->server_thread.joinable()) ) return;
    try
    {
        // Connect to the listening socket to unblock the 'accept()' invocation
        using namespace boost::asio;
        io_context ioc;
        ip::tcp::endpoint ep(this->use_ipv6 ? ip::tcp::v6() : ip::tcp::v4(), this->port);
        ip::tcp::socket sock(ioc);
        sock.connect(ep);
    }
    catch(...){}
    for ( unsigned i = 0u; i < 10u; ++i )
    {
        if ( this->death_warrant ) std::this_thread::sleep_for(std::chrono::milliseconds(100u));
    }
    if ( this->death_warrant )
    {
        std::puts("LocalHttpServer thread didn't end within a second -- killing the process now");
        std::_Exit(EXIT_FAILURE);
    }
    std::puts("Last line in destructor of LocalHttpServer");
}

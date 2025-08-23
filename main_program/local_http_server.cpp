#include "local_http_server.hpp"
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

static constexpr std::pair<char const*, char const*> g_content_types[] = {
    { "html", "text/html"       },
    { "htm" , "text/html"       },
    { "txt" , "text/plain"      },
    { "pdf" , "application/pdf" },
    { "md"  , "text/markdown"   },

    // REVISIT FIX - also needed a few more -- one beginning with 'p' can't remember
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
        if ( _Max == this->port ) this->port = 0u;
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
                file_binary_contents = this->html_str;
                extension = "html";
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

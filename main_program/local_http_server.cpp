#include "local_http_server.hpp"
#include <cstdio>                    // fprintf, stderr
#include <iostream>                  // cerr, endl
#include <stdexcept>                 // runtime_error
#include <string>                    // string
#include <string_view>               // string_view
#include <boost/asio.hpp>            // io_context, ip::tcp::acceptor, ip::tcp::socket
#include <boost/beast.hpp>           // flat_buffer, http::request, http::response, http::read, http::write
#include "embedded_archive.hpp"      // ArchiveGetFile

using std::string, std::string_view;

static constexpr std::pair<char const*, char const*> g_content_types[] = {
    { "html", "text/html"       },
    { "htm" , "text/html"       },
    { "txt" , "text/plain"      },
    { "pdf" , "application/pdf" },
    { "md"  , "text/markdown"   },
};

void LocalHttpServer::ThreadEntryPoint(void) noexcept
{
    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace net = boost::asio;
    using tcp = net::ip::tcp;

    try
    {
        net::io_context ioc{ 1 };
        tcp::acceptor acceptor{ ioc };
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
        if ( (0 == this->port.load()) || (0xFFFF == this->port.load()) ) throw std::runtime_error("invalid TCP port number");
        this->port.notify_one();

        for (; /* ever */;)
        {
            tcp::socket socket{ ioc };
            acceptor.accept(socket);

            beast::flat_buffer buffer;
            http::request<http::string_body> req;
            http::read(socket, buffer, req);

            std::string s = string(  req.target()  ) + '.';
            std::string_view sv = s;
            try
            {
				if ( sv.empty() ) throw - 1;
                if ( false == sv.starts_with('/') ) throw -1;
				sv.remove_prefix(1u);  // remove leading forward slash
                if ( sv.empty() ) throw - 1;
                string extension;
                string const file_binary_contents = ArchiveGetFile( sv, extension, true );
                if ( extension.empty() || file_binary_contents.empty() ) throw -1;
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
				continue;  //  successfully handled the request
            }
            catch (std::exception const &e)
            {
                std::fprintf(stderr, "LocalHttpServer exception thrown: %s\n", e.what());
            }
            catch (...) {}

            http::response<http::string_body> res{ http::status::not_found, req.version() };
            res.set(http::field::server, "PaperKernelCxx.Boost.Beast");
            res.set(http::field::content_type, "text/plain");
            res.body() = "Paper not found";
            res.prepare_payload();
            http::write(socket, res);
        }
    }
    catch (std::exception const &e)
    {
        std::fprintf(stderr, "LocalHttpServer exception thrown: %s\n", e.what() );
    }
    catch (...) {}

    this->port = -1;
    this->port.notify_one();
}

std::uint16_t LocalHttpServer::StartServer(void) noexcept
{
    if ( this->server_thread.joinable() ) return port;
    this->server_thread = std::jthread( [this]{ this->ThreadEntryPoint(); } );
    this->port.wait(0u);
    if ( 0xFFFF == this->port.load() ) this->port = 0u;
    return port;
}

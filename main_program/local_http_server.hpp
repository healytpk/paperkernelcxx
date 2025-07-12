#include <cstdint>                   // uint16_t
#include <atomic>                    // atomic
#include <chrono>                    // milliseconds
#include <functional>                // ref
#include <thread>                    // this_thread::sleep_for, jthread
#include <vector>                    // vector
#include <iostream>                  // cerr, endl
#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace local_http_server_detail {

namespace beast = boost::beast;
namespace http  = beast::http;
namespace net   = boost::asio;
using tcp = net::ip::tcp;

// Example PDF data (replace with your own)
inline std::vector<char> pdfData = {/* ... fill with your PDF bytes ... */ };

inline void thread_entry_point(std::atomic<std::uint16_t> &out_port)
{
    try
    {
        net::io_context ioc{ 1 };

        // Create an IPv6 endpoint on any address, port 0 (let OS choose)
        tcp::endpoint endpoint(tcp::v6(), 0);
        tcp::acceptor acceptor{ ioc };

        // Open the acceptor with IPv6 protocol
        acceptor.open(endpoint.protocol());

        // Allow both IPv4 and IPv6 (dual-stack) if supported
        acceptor.set_option(net::ip::v6_only(false));

        // Bind to the endpoint
        acceptor.bind(endpoint);

        // Start listening for connections
        acceptor.listen();

        // Get the port actually assigned
        out_port = acceptor.local_endpoint().port();

        for (; /* ever */;)
        {
            tcp::socket socket{ ioc };
            acceptor.accept(socket);

            beast::flat_buffer buffer;
            http::request<http::string_body> req;
            http::read(socket, buffer, req);

            if ( req.target() == "/file.pdf" )
            {
                http::response< http::vector_body<char> > res{ http::status::ok, req.version() };
                res.set(http::field::server, "Boost.Beast");
                res.set(http::field::content_type, "application/pdf");
                res.body() = pdfData;
                res.prepare_payload();
                http::write(socket, res);
            }
            else
            {
                http::response<http::string_body> res{ http::status::not_found, req.version() };
                res.set(http::field::server, "Boost.Beast");
                res.set(http::field::content_type, "text/plain");
                res.body() = "Not found";
                res.prepare_payload();
                http::write(socket, res);
            }
        }
    }
    catch (std::exception const &e)
    {
        std::cerr << "HTTP server error: " << e.what() << std::endl;
    }
}


inline std::jthread server_thread;

}  // close namespace local_http_server

inline std::uint16_t start_http_server(void)
{
    using namespace local_http_server_detail;
    static std::atomic<std::uint16_t> port{ 0u };
    if ( server_thread.joinable() ) return port;
    server_thread = std::jthread( thread_entry_point, std::ref(port) );
    while ( 0u == port ) std::this_thread::sleep_for( std::chrono::milliseconds(10) );
    return port;
}

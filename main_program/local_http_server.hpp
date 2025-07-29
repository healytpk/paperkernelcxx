#pragma once
#include <cstdint>                   // uint16_t
#include <atomic>                    // atomic
#include <optional>                  // optional
#include <thread>                    // jthread
#include <boost/asio.hpp>            // io_context, ip::tcp::acceptor

class LocalHttpServer {
protected:
    bool is_listening{ false };
    bool use_ipv6{ false };
    std::uint16_t port{ 0u };
    std::optional<boost::asio::io_context> opt_ioc;
    std::optional<boost::asio::ip::tcp::acceptor> opt_acceptor;
public:
    bool IsListening(void) const noexcept { return this->is_listening; }
    bool IsUsingIPv6(void) const noexcept { return this->use_ipv6; }
    std::uint16_t GetListeningPort(void) const noexcept { return this->port; }
    LocalHttpServer(void) noexcept;
    bool TryServeWebpage(void) noexcept;
    void DiscardAllPendingConnections(void) noexcept;
};

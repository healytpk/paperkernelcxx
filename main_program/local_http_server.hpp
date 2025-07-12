#pragma once
#include <cstdint>                   // uint16_t
#include <atomic>                    // atomic
#include <thread>                    // jthread

class LocalHttpServer {
protected:
    bool use_ipv6 = false;
    std::atomic<std::uint16_t> port{ 0u };
    std::jthread server_thread;
    void ThreadEntryPoint(void) noexcept;
public:
    bool IsUsingIPv6(void) const noexcept { return this->use_ipv6; }
    std::uint16_t StartServer(void) noexcept;
    std::uint16_t GetListeningPort(void) const noexcept { return this->port; }
};

#pragma once
#include <cstdint>                   // uint16_t
#include <atomic>                    // atomic
#include <thread>                    // jthread

class LocalHttpServer {
protected:
    std::atomic<std::uint16_t> port{ 0u };
    std::jthread server_thread;
    void ThreadEntryPoint(void) noexcept;
public:
    std::uint16_t StartServer(void) noexcept;
    std::uint16_t GetListeningPort(void) noexcept { return this->port; }
};

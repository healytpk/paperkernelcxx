#ifndef HEADER_INCLUSION_GUARD_6986124654685656549598674325825449489
#define HEADER_INCLUSION_GUARD_6986124654685656549598674325825449489

#include <cstdint>                     // int32_t
#include <mutex>                       // mutex
#include <string>                      // string
#include <string_view>                 // string_view
#include <vector>                      // vector

struct llama_context;
struct llama_model;
struct llama_sampler;
struct llama_vocab;

class AImanager {
protected:
    static constexpr int n_predict = 32;

    llama_model       *model = nullptr;
    llama_sampler     *smpl  = nullptr;
    llama_context     *ctx   = nullptr;
    llama_vocab const *vocab = nullptr;

    int m_ngl   =   99;
    int m_n_ctx = 2048;

    std::recursive_mutex mtx;

public:

    void Reset(void);
    ~AImanager(void) noexcept;

    void Init(void) noexcept(false);

    void DiscardContext(void) noexcept;
    void NewContext(std::size_t) noexcept(false);
    void LoadInPaper(std::vector<std::int32_t> const *) noexcept(false);
    std::string PutInAndGetOut( std::vector<std::int32_t> const & ) noexcept(false);
    std::string PutInAndGetOut( std::string_view ) noexcept(false);
};

#endif

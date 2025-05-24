#include "ai.hpp"
#include <cassert>                  // assert
#include <cstdio>
#include <cstdlib>                  // EXIT_FAILURE
#include <cstring>
#include <iostream>                 // cin   ------------------------------------- REMOVE THIS
#include <string>                   // string
#include <type_traits>              // is_same
#include <vector>                   // vector
#include "Auto.h"

#ifdef PAPERKERNELCXX_USE_AI
#    include "llama.h"
#endif

using std::lock_guard, std::string, std::string_view, std::vector;

constexpr char model_path[] = "./deepseek.gguf";

void AImanager::Reset(void)
{
    lock_guard mylock( this->mtx );  // might throw if mutex malfunctions

#ifdef PAPERKERNELCXX_USE_AI
    try { if ( smpl  ) llama_sampler_free(smpl ); } catch(...){}
    try { if ( ctx   ) llama_free        (ctx  ); } catch(...){}
    try { if ( model ) llama_model_free  (model); } catch(...){}
#endif

    smpl  = nullptr;
    ctx   = nullptr;
    model = nullptr;
}

AImanager::~AImanager(void) noexcept
{
    try { this->Reset(); } catch(...) {}
}

void AImanager::Init(void) noexcept(false)
{
    lock_guard mylock( this->mtx );

#ifdef PAPERKERNELCXX_USE_AI
    // only print errors
    llama_log_set([](enum ggml_log_level level, char const *const text, void*)
      {
        if ( level >= GGML_LOG_LEVEL_ERROR ) fprintf(stderr, "%s", text);
      }, nullptr);

    ggml_backend_load_all();

    llama_model_params model_params = llama_model_default_params();
    model_params.n_gpu_layers = m_ngl;

    this->model = llama_model_load_from_file(model_path, model_params);
    if ( nullptr == this->model ) throw "unable to load model";

    this->vocab = llama_model_get_vocab(this->model);
    if ( nullptr == this->vocab ) throw "unable to get vocabulary from model";

    // initialize the sampler
    this->smpl = llama_sampler_chain_init( llama_sampler_chain_default_params() );
    if ( nullptr == this->smpl ) throw "failed to create the llama_sampler";

    llama_sampler_chain_add(this->smpl, llama_sampler_init_min_p(0.05f, 1));
    llama_sampler_chain_add(this->smpl, llama_sampler_init_temp(0.8f));
    llama_sampler_chain_add(this->smpl, llama_sampler_init_dist(LLAMA_DEFAULT_SEED));
    llama_sampler_chain_add(this->smpl, llama_sampler_init_greedy());
#endif
}

void AImanager::DiscardContext(void) noexcept
{
    lock_guard mylock( this->mtx );

    assert( nullptr != this->model );

    if ( nullptr == this->ctx ) return;

#ifdef PAPERKERNELCXX_USE_AI
    llama_free( this->ctx );
#endif

    this->ctx = nullptr;
}

void AImanager::NewContext(size_t const token_count) noexcept(false)
{
    lock_guard mylock( this->mtx );

    assert( nullptr != this->model );

    this->DiscardContext();

#ifdef PAPERKERNELCXX_USE_AI
    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_batch = token_count;
    ctx_params.n_ctx   = token_count + n_predict - 1;
    ctx_params.no_perf = false;
    this->ctx = llama_init_from_model(this->model, ctx_params);
#endif

    if ( nullptr == this->ctx ) throw "failed to create the llama_context";
}

string AImanager::PutInAndGetOut( vector<int32_t> const &prompt_tokens ) noexcept(false)
{
    lock_guard mylock( this->mtx );
    assert( nullptr != this->ctx   );
    assert( nullptr != this->smpl  );
    assert( nullptr != this->vocab );

    string response;

#ifdef PAPERKERNELCXX_USE_AI
    static_assert( std::is_same_v<int32_t, llama_token> );

    // prepare a batch for the prompt
    llama_batch batch = llama_batch_get_one( const_cast<int32_t*>(prompt_tokens.data()), prompt_tokens.size() );
    assert( batch.n_tokens == prompt_tokens.size() );
    int32_t new_token_id;
    for (; /* ever */ ;)
    {
        // check if we have enough space in the context to evaluate this batch
        int const n_ctx      = llama_n_ctx(this->ctx);
        int const n_ctx_used = llama_kv_self_used_cells(this->ctx);
        if ( (n_ctx_used + batch.n_tokens) > n_ctx ) throw "context size exceeded";

        if ( llama_decode(this->ctx, batch) ) throw "failed to decode";

        // sample the next token
        new_token_id = llama_sampler_sample(this->smpl, this->ctx, -1);

        // is it an end of generation?
        if ( llama_vocab_is_eog(this->vocab, new_token_id) ) break;

        // convert the token to a string, print it and add it to the response
        char buf[256u];
        int const n = llama_token_to_piece(this->vocab, new_token_id, buf, sizeof buf - 1uz, 0, true);
        if ( n < 0 ) throw "failed to convert token to piece";
        assert( n < sizeof(buf) );
        buf[n] = '\0';
        response += buf;

        // prepare the next batch with the sampled token
        batch = llama_batch_get_one(&new_token_id, 1);
    }
#endif

    return response;
}

string AImanager::PutInAndGetOut(string_view const prompt) noexcept(false)
{
    lock_guard mylock( this->mtx );
    assert( nullptr != this->ctx   );
    assert( nullptr != this->vocab );

#ifdef PAPERKERNELCXX_USE_AI
    bool const is_first = (0 == llama_kv_self_used_cells(this->ctx));

    // Tokenize the prompt - First call it with nullptr to get a negative number of how many tokens
    int const countA = -llama_tokenize(this->vocab, &prompt.front(), prompt.size(), nullptr, 0, is_first, true);
    vector<llama_token> tokens(countA);
    int const countB =  llama_tokenize(this->vocab, &prompt.front(), prompt.size(), &tokens.front(), tokens.size(), is_first, true);

    if ( countA != countB ) throw "Failed to tokenize the text prompt";

    return this->PutInAndGetOut(tokens);
#else
    return {};
#endif
}

void AImanager::LoadInPaper(vector<int32_t> const *const pvec) noexcept(false)
{
    lock_guard mylock( this->mtx );
    assert( nullptr != this->ctx );

    printf( "start - %i tokens\n", (int)pvec->size() );
    this->PutInAndGetOut( *pvec );
    puts("finished");
}

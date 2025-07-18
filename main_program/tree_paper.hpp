#pragma once

#include <cstdint>                          // uint_fast64_t
#include "hash.hpp"                         // Hash, Hash_t
#include "paper.hpp"                        // Paper
#include "stringcraft.hpp"                  // wxStringCharType
#include "_Max.hpp"

struct PaperRevInfo_t;

struct PaperInfo {
    Paper paper;                  // Revision will be set to 0
    PaperRevInfo_t const *prevs;  // points to minus-one-terminated array
};

struct PaperRevInfo_t {
    static constexpr std::uint_fast64_t terminator = _Max;
    std::uint_fast64_t rev;
    Hash_t const *hashes_authors;   // points to null-terminated array
    wxStringCharType const *title;
};

template<Hash_t... Elems>
struct ArrHash_detail {
    static constexpr Hash_t value[sizeof...(Elems) + 1u] = { Elems... , 0u };
};

template<Hash_t... Elems>
consteval Hash_t const *ArrHash(void)
{
    return ArrHash_detail<Elems...>::value;
}

template<std::uint_fast64_t RevNum, Hash_t const *Authors, StringLiteral Title>
struct Rev {
    static constexpr PaperRevInfo_t value = {
        RevNum,
        Authors,
        Title.value
    };
};

template<typename... RevHolders>
struct PaperRevList_detail {
    static constexpr PaperRevInfo_t value[sizeof...(RevHolders) + 1u] = {
        RevHolders::value... ,     // all revs
        { PaperRevInfo_t::terminator, nullptr, nullptr }   // minus-one-terminator
    };
};

template<typename... RevHolders>
consteval PaperRevInfo_t const *RevList(void)
{
    return PaperRevList_detail<RevHolders...>::value;
}

inline constexpr PaperInfo g_map_papers[] =
#    include "AUTO_GENERATED_tree_contents_paper.hpp"
;

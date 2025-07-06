#pragma once

#include <cstdint>                          // uint_fast64_t
#include <utility>                          // pair
#include <vector>                           // vector
#include "paper.hpp"                        // Paper
#include "stringcraft.hpp"                  // Pretender_wxString

template<Paper... RevHolders>
struct PaperList_detail {
    static constexpr Paper value[sizeof...(RevHolders) + 1u] = {
        RevHolders... ,     // all papers
        Paper::Terminator()        // terminator
    };
};

template<Paper... RevHolders>
consteval Paper const *PaperList(void)
{
    return PaperList_detail<RevHolders...>::value;
}

inline constexpr std::pair< std::uint_fast64_t, Paper const * > g_map_authors[] =
#    include "AUTO_GENERATED_tree_contents_author.hpp"
;

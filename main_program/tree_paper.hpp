#pragma once

#include <cstdint>                          // uint_fast64_t, uintptr_t
#include "stringcraft.hpp"                  // wxStringCharType, Hash

struct PaperRevInfo_t;

struct PaperInfo {
    std::uintptr_t num;
    PaperRevInfo_t const *prevs;  // points to minus-one-terminated array
};

struct PaperRevInfo_t {
    static constexpr std::uint_fast64_t terminator = (std::uint_fast64_t)-1;
    std::uint_fast64_t rev;
    std::uint_fast64_t const *hashes_authors;   // points to null-terminated array
    wxStringCharType const *title;
};

template<std::uint_fast64_t... Elems>
struct Arr64_detail {
    static constexpr std::uint_fast64_t value[sizeof...(Elems) + 1u] = { Elems... , 0u };
};

template<std::uint_fast64_t... Elems>
consteval std::uint_fast64_t const *Arr64(void)
{
    return Arr64_detail<Elems...>::value;
}

template<std::uint_fast64_t RevNum, std::uint_fast64_t const *Authors, StringLiteral Title>
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
        { (decltype(PaperRevInfo_t::rev))-1, nullptr, nullptr }   // minus-one-terminator
    };
};

template<typename... RevHolders>
consteval PaperRevInfo_t const *RevList(void)
{
    return PaperRevList_detail<RevHolders...>::value;
}

constexpr PaperInfo g_map_papers[] =
#if wxUSE_UNICODE_WCHAR
#    include "AUTO_GENERATED_tree_contents_paper_wchar_t.hpp"
#else
#    include "AUTO_GENERATED_tree_contents_paper_char.hpp"
#endif
;

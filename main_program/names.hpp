#pragma once
#include <algorithm>          // ranges::lower_bound, ranges::equal_range
#include <array>              // array
#include <vector>             // vector
#include "AUTO_GENERATED_names.hpp"

template<typename T, std::size_t N>
consteval auto container_sorted_by_first(T const (&arg)[N]) -> std::array<T, N>
{
    std::array<T, N> retval{};
    for ( std::size_t i = 0u; i < N; ++i ) retval[i] = arg[i];    // copy_n gives error on g++ Debug
    for ( std::size_t i = 1; i < N; ++i )
    {
        T key = retval[i];
        std::size_t j = i;
        while ( (0u != j) && (std::get<0u>(key) < std::get<0u>(retval[j-1u]) ) )
        {
            retval[j] = retval[j - 1];
            --j;
        }
        retval[j] = key;
    }
    return retval;
}

inline constexpr auto g_primary_names     = container_sorted_by_first(g_primary_names_unsorted    );
inline constexpr auto g_alternative_names = container_sorted_by_first(g_alternative_names_unsorted);

inline constexpr Hash_t PrimaryHash(wxStringCharType const *const name)
{
    Hash_t const h = Hash(name);
    auto const it = std::ranges::lower_bound(
        g_alternative_names, h, {}, [](auto const &e) { return std::get<0u>(e); }
    );
    if ((it != std::end(g_alternative_names)) && (std::get<0u>(*it) == h))
        return std::get<2u>(*it);
    return h;
}

inline constexpr wxStringCharType const *HashToDirectString(Hash_t const h)
{
    auto const it = std::ranges::lower_bound(
        g_primary_names, h, {}, [](auto const &e) { return std::get<0u>(e); }
    );
    if ( (it != std::end(g_primary_names)) && (std::get<0u>(*it) == h) ) return std::get<1u>(*it);

    auto const it2 = std::ranges::lower_bound(
        g_alternative_names, h, {}, [](auto const &e) { return std::get<0u>(e); }
    );
    if ( (it2 != std::end(g_alternative_names)) && (std::get<0u>(*it2) == h) ) return std::get<1u>(*it2);
    return nullptr;
}

inline constexpr wxStringCharType const *Primary(wxStringCharType const *const name)
{
    Hash_t const h = PrimaryHash(name);
    auto const it = std::ranges::lower_bound(
        g_primary_names, h, {}, [](auto const &e) { return std::get<0u>(e); }
    );
    if ((it != std::end(g_primary_names)) && (std::get<0u>(*it) == h))
        return std::get<1u>(*it);
    return nullptr;
}

inline std::vector<wxStringCharType const*> GetAllAlternatives(Hash_t const primary_hash)
{
    std::vector<wxStringCharType const*> result;
    auto const myrange = std::ranges::equal_range(
        g_alternative_names, primary_hash, {}, [](auto const &e) { return std::get<2u>(e); }
    );
    for (auto it = myrange.begin(); it != myrange.end(); ++it)
        result.emplace_back(std::get<1u>(*it));
    return result;
}

inline constexpr wxStringCharType const *PrimaryNameFromHash(Hash_t const h)
{
    auto const it = std::ranges::lower_bound( g_primary_names, h, {}, [](auto const &e) { return std::get<0u>(e); } );
    if ( (it != std::end(g_primary_names)) && (std::get<0u>(*it) == h) ) return std::get<1u>(*it);
    return nullptr;
}

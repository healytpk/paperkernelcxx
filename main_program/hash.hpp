#pragma once
#include <cstddef>                       // size_t
#include <cstdint>                       // uint_fast64_t
#include <string_view>                   // string_view, wstring_view
#include "cctype_constexpr.hpp"          // tolower

inline constexpr std::uint_fast64_t Hash(std::string_view const input) noexcept
{
    std::uint_fast64_t h = 0xcbf29ce484222325;

    //constexpr char letters[] = "abcdefghijklmnopqrstuvwxyz\\0123456789 . -";

    for ( char const cX : input )
    {
        /*
        char const c = cctype_constexpr::tolower(cX);
        bool is_letter = false;

        for ( char const letter : letters )
        {
            if ( letter == c )
            {
                is_letter = true;
                break;
            }
        }

        if ( false == is_letter ) continue;
        */

        h ^= static_cast<std::uint_fast64_t>(cX);
        h *= 0x100000001B3;
    }

    return h;
}

inline constexpr std::uint_fast64_t Hash(std::wstring_view const input) noexcept
{
    std::uint_fast64_t h = 0xcbf29ce484222325;

    //constexpr wchar_t letters[] = L"abcdefghijklmnopqrstuvwxyz\\0123456789";

    for ( wchar_t const cX : input )
    {
        /*
        wchar_t const c = cctype_constexpr::tolower(cX);
        bool is_letter = false;

        for ( wchar_t const letter : letters )
        {
            if ( letter == c )
            {
                is_letter = true;
                break;
            }
        }

        if ( false == is_letter ) continue;
        */

        h ^= static_cast<std::uint_fast64_t>(cX);
        h *= 0x100000001B3;
    }

    return h;
}

static_assert( Hash( "Thomas PK Healy") != Hash( "thomas p. k. healy") );
static_assert( Hash(L"Thomas PK Healy") != Hash(L"thomas p. k. healy") );

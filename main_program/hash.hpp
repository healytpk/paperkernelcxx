#pragma once
#include <cstddef>                       // size_t
#include <cstdint>                       // uint_fast64_t
#include <string_view>                   // string_view, wstring_view

inline constexpr char tolower_consteval_char(char const c) noexcept
{
    switch ( c )
    {
        case 'A': return 'a'; case 'B': return 'b'; case 'C': return 'c'; case 'D': return 'd';
        case 'E': return 'e'; case 'F': return 'f'; case 'G': return 'g'; case 'H': return 'h';
        case 'I': return 'i'; case 'J': return 'j'; case 'K': return 'k'; case 'L': return 'l';
        case 'M': return 'm'; case 'N': return 'n'; case 'O': return 'o'; case 'P': return 'p';
        case 'Q': return 'q'; case 'R': return 'r'; case 'S': return 's'; case 'T': return 't';
        case 'U': return 'u'; case 'V': return 'v'; case 'W': return 'w'; case 'X': return 'x';
        case 'Y': return 'y'; case 'Z': return 'z';
    }

    return c;
}

inline constexpr wchar_t tolower_consteval_wchar_t(wchar_t const c) noexcept
{
    switch ( c )
    {
        case L'A': return L'a'; case L'B': return L'b'; case L'C': return L'c'; case L'D': return L'd';
        case L'E': return L'e'; case L'F': return L'f'; case L'G': return L'g'; case L'H': return L'h';
        case L'I': return L'i'; case L'J': return L'j'; case L'K': return L'k'; case L'L': return L'l';
        case L'M': return L'm'; case L'N': return L'n'; case L'O': return L'o'; case L'P': return L'p';
        case L'Q': return L'q'; case L'R': return L'r'; case L'S': return L's'; case L'T': return L't';
        case L'U': return L'u'; case L'V': return L'v'; case L'W': return L'w'; case L'X': return L'x';
        case L'Y': return L'y'; case L'Z': return L'z';
    }

    return c;
}

inline constexpr std::uint_fast64_t Hash(std::string_view const input) noexcept
{
    std::uint_fast64_t h = 0xcbf29ce484222325;

    constexpr char letters[] = "abcdefghijklmnopqrstuvwxyz";

    for ( char const cX : input )
    {
        char const c = tolower_consteval_char(cX);
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

        h ^= static_cast<std::uint_fast64_t>(c);
        h *= 0x100000001B3;
    }

    return h;
}

inline constexpr std::uint_fast64_t Hash(std::wstring_view const input) noexcept
{
    std::uint_fast64_t h = 0xcbf29ce484222325;

    constexpr wchar_t letters[] = L"abcdefghijklmnopqrstuvwxyz";

    for ( wchar_t const cX : input )
    {
        wchar_t const c = tolower_consteval_wchar_t(cX);
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

        h ^= static_cast<std::uint_fast64_t>(c);
        h *= 0x100000001B3;
    }

    return h;
}

static_assert( Hash( "Thomas PK Healy") == Hash( "thomas p. k. healy") );
static_assert( Hash(L"Thomas PK Healy") == Hash(L"thomas p. k. healy") );

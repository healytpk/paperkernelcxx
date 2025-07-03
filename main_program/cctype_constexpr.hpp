#pragma once

namespace cctype_constexpr {
    
inline constexpr char tolower(char const c) noexcept
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

inline constexpr wchar_t tolower(wchar_t const c) noexcept
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

inline constexpr bool isdigit(char const c) noexcept
{
    switch ( c )
    {
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        return true;
    }

    return false;
}

inline constexpr bool isdigit(wchar_t const c) noexcept
{
    switch ( c )
    {
    case L'0': case L'1': case L'2': case L'3': case L'4':
    case L'5': case L'6': case L'7': case L'8': case L'9':
        return true;
    }

    return false;
}

}  // close namespace 'cctype_constexpr'

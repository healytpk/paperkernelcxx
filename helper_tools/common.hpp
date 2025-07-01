#pragma once

#include <cstddef>                                     // size_t
#include <cstdint>                                     // uint_fast64_t
#include <filesystem>                                  // path
#include <functional>                                  // function
#include <string>                                      // string
#include <string_view>                                 // string_view
#include <vector>                                      // vector

template<typename T>
inline float megabytes(T &&container, size_t s = -1)
{
    if ( -1 == s ) s = container.size();
    float retval = static_cast<float>(s);
    retval *= sizeof( container.front() );
    retval /= 1024.0f;  // kilobytes
    retval /= 1024.0f;  // megabytes
    return retval;
}

extern std::string GetPlainText_PDF (std::filesystem::path const &);
extern std::string GetPlainText_HTML(std::filesystem::path const &);
extern std::string GetPlainText_MD  (std::filesystem::path const &);

extern void IterateThroughFolder(
    std::filesystem::path const &where,
    std::function<bool(std::filesystem::path const &, std::string_view)> const &callback );

extern std::string ExtractTitleFromFileHTML(std::string const &filename);
extern std::string ExtractTitleFromFilePDF (std::string const &filename);

extern std::string ExtractAuthorFromFileHTML(std::string const &filename);
extern std::string ExtractAuthorFromFilePDF (std::string const &filename);

extern void ReplaceHtmlNumericEscapesInPlace(std::string&);

extern void ReplaceInPlace(std::string &text, std::string_view const target, std::string_view replacement);

extern void TrimWhitespace(std::string &s);

extern std::vector<std::string> SplitByNewLines(std::string_view const sv);

extern void EscapeNonAscii(std::string &str, bool escape_wide = false);

extern void Erase(std::string &str, std::string_view const sv);

constexpr char tolower_consteval(char const c) noexcept
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
        default:  return c;
    }
}

constexpr char letters[] = "abcdefghijklmnopqrstuvwxyz";

constexpr std::uint_fast64_t Hash(std::string_view const input) noexcept
{
    std::uint_fast64_t h = 0xcbf29ce484222325;

    for ( char const cX : input )
    {
        char const c = tolower_consteval(cX);
        bool is_letter = false;

        for ( char letter : letters )
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

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

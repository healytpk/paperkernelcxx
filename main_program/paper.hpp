#pragma once

#include <stdexcept>                              // runtime_error
#include <string>                                 // string     , wstring
#include <string_view>                            // string_view, wstring_view

#ifndef PAPERKERNELCXX_MINIMAL_PAPER
    class wxString;
#endif

struct Paper {

    unsigned num, rev;

    Paper(void) noexcept(false)
    {
        throw std::runtime_error("invalid or null paper");
    }

    Paper(std:: string_view) noexcept(false);
    Paper(std::wstring_view) noexcept(false);

    Paper(std:: string const &s) noexcept(false) : Paper( std:: string_view(s) ) {}
    Paper(std::wstring const &s) noexcept(false) : Paper( std::wstring_view(s) ) {}

    Paper( long long unsigned const Anum, long long unsigned const Arev ) noexcept
      : num(static_cast<unsigned>(Anum)), rev(static_cast<unsigned>(Arev)) {}

    bool operator<(Paper const other) const noexcept
    {
        return (num < other.num) || ((num == other.num) && (rev < other.rev));
    }

    bool IsRelatedTo(Paper const other) const noexcept { return num == other.num; }

#ifndef PAPERKERNELCXX_MINIMAL_PAPER
    wxString const &GetTitle (void) const noexcept;
    wxString const &GetAuthor(void) const noexcept;
    wxString const &GetPaper (void) const noexcept;
     char   const * c_str(void) const noexcept;
    wchar_t const *wc_str(void) const noexcept;
#endif
};

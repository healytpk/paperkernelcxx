#pragma once

#include <stdexcept>                              // runtime_error
#include <string>                                 // string
#include <string_view>                            // string_view

struct Paper {

    unsigned num, rev;

    Paper(void) noexcept(false)
    {
        throw std::runtime_error("invalid or null paper");
    }

    Paper(std::string_view) noexcept(false);

    Paper(std::string const &s) noexcept(false)
      : Paper( std::string_view(s) ) {}

    Paper( long long unsigned const Anum, long long unsigned const Arev ) noexcept
      : num(static_cast<unsigned>(Anum)), rev(static_cast<unsigned>(Arev)) {}

    bool operator<(Paper const other) const noexcept
    {
        return (num < other.num) || ((num == other.num) && (rev < other.rev));
    }

    char const *c_str(void) const noexcept;

    bool IsRelatedTo(Paper const other) const noexcept { return num == other.num; }

    char const *GetTitle (void) const noexcept;
    char const *GetAuthor(void) const noexcept;
};

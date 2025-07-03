#pragma once

#include <cassert>                                // assert
#include <stdexcept>                              // runtime_error
#include <string>                                 // string     , wstring
#include <string_view>                            // string_view, wstring_view
#include <type_traits>                            // is_convertible
#include <utility>                                // declval
#include "cctype_constexpr.hpp"                   // isdigit, tolower

#ifndef PAPERKERNELCXX_MINIMAL_PAPER
    class wxString;
#endif

struct Paper {
public:
    char letter;
    unsigned num, rev;
private:
    struct PaperTerminator_t {};
    consteval Paper(PaperTerminator_t) noexcept
        : letter('Z'), num(0), rev(0) {}
public:
    static consteval Paper Terminator(void) noexcept
    {
        return Paper(PaperTerminator_t());
    }
    constexpr bool IsTerminator(void) const noexcept
    {
        return 'Z' == letter;
    }

    template<typename T>
    requires(    std::is_convertible_v<decltype(std::declval<T&&>()[0]),  char  >
              || std::is_convertible_v<decltype(std::declval<T&&>()[0]), wchar_t> )
    static constexpr void Constructor_Paper(Paper *const this_paper, T &&p)
    {
        using namespace cctype_constexpr;  // isdigit, tolower

        auto &letter = this_paper->letter;
        auto &num    = this_paper->num   ;
        auto &rev    = this_paper->rev   ;

        while ( p.size() >= 5u )  // fake loop only iterates once (so we can 'break')
        {
            if ( !isdigit(p[1]) ) break;
            if ( !isdigit(p[2]) ) break;
            if ( !isdigit(p[3]) ) break;
            if ( !isdigit(p[4]) ) break;
            num  = (p[1]-'0') * 1000u;
            num += (p[2]-'0') *  100u;
            num += (p[3]-'0') *   10u;
            num += (p[4]-'0') *    1u;

            switch ( tolower(p[0]) )
            {
            case 'n':
                letter = 'n';
                rev = 0u;
                if ( 5u == p.size() )  // N1234
                {
                    // fall through
                }
                else if ( ('\0' == p[5]) || ('.' == p[5]) )  // length is >= 6
                {
                    // fall through
                }
                else  // length is >= 6
                {
                    break;
                }

                return;

            case 'p':
                letter = 'p';
                if ( p.size() < 7u ) break;
                if ( 'r' != tolower(p[5]) ) break;
                if ( !isdigit(p[6]) ) break;
                rev = (p[6]-'0');

                if ( 7u == p.size() )  // P1234R0
                {
                    // fall through
                }
                else if ( ('\0' == p[7]) || ('.' == p[7]) )  // length is >= 8
                {
                    // fall through
                }
                else if ( isdigit(p[7]) )  // length is >= 8
                {
                    rev *= 10u;
                    rev += (p[7]-'0');
                    if ( (p.size() > 8u) && ('\0' != p[8]) && ('.' != p[8]) ) break;  // P1234R15
                    // fall through
                }
                else  // length is >= 8
                {
                    break;
                }

                return;
            }

            break;  // fake loop only iterates once!
        }
        throw std::runtime_error("invalid paper name string");
    }

    // Constructor takes 'llu' because of 'std::stoull'
    constexpr Paper( char const Aletter, long long unsigned const Anum, long long unsigned const Arev ) noexcept
      : letter( cctype_constexpr::tolower(Aletter) ),
        num(static_cast<unsigned>(Anum)),
        rev(static_cast<unsigned>(Arev))
    {
        assert( ('n' == letter) || ('p' == letter) );
        assert( (num >= 1u) && (num <= 9999u) );
        switch ( letter )
        {
        case 'n': assert( rev ==  0u ); break;
        case 'p': assert( rev <= 99u ); break;
        }
    }

    Paper(void) noexcept(false)
    {
        throw std::runtime_error("null or invalid paper");
    }

    constexpr Paper(std:: string_view const arg) noexcept(false) { Constructor_Paper(this, arg); }
    constexpr Paper(std::wstring_view const arg) noexcept(false) { Constructor_Paper(this, arg); }

    constexpr Paper(   char const *const arg) noexcept(false) : Paper( std:: string_view(arg) ) {}
    constexpr Paper(wchar_t const *const arg) noexcept(false) : Paper( std::wstring_view(arg) ) {}

    constexpr Paper(std:: string const &s) noexcept(false) : Paper( std:: string_view(s) ) {}
    constexpr Paper(std::wstring const &s) noexcept(false) : Paper( std::wstring_view(s) ) {}

    constexpr bool operator<(Paper const other) const noexcept
    {
        if ( letter != other.letter ) return letter < other.letter;
        if (    num != other.num    ) return    num < other.num   ;
        return rev < other.rev;
    }

    constexpr bool IsRelatedTo(Paper const other) const noexcept { return (letter == other.letter) && (num == other.num); }

     char   const * c_str(void) const noexcept;
    wchar_t const *wc_str(void) const noexcept;

#ifndef PAPERKERNELCXX_MINIMAL_PAPER
    wxString const &GetTitle (void) const noexcept;
    wxString const &GetAuthor(void) const noexcept;
    wxString        GetPaper (void) const noexcept;
#endif
};

#ifndef NDEBUG
namespace Paper_detail {
    constexpr Paper my_compile_time_paper("p1234r7");    // just testing that constexpr works
}
#endif

#pragma once

#include <cassert>                                // assert
#include <cstddef>                                // size_t
#include <stdexcept>                              // runtime_error
#include <string>                                 // string     , wstring     , char_traits<T>::length
#include <string_view>                            // string_view, wstring_view
#include <type_traits>                            // is_constant_evaluated, is_convertible
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
    consteval Paper(PaperTerminator_t) noexcept : letter('z'), num(0u), rev(0u) {}
public:
    static consteval Paper Terminator(void) noexcept
    {
        return Paper(PaperTerminator_t());
    }
    constexpr bool IsTerminator(void) const noexcept
    {
        return 'z' == letter;
    }
private:
    struct PaperConstructor_t {};
    template<typename T>
    requires(    std::is_convertible_v<decltype(std::declval<T&&>()[0]),  char  >
              || std::is_convertible_v<decltype(std::declval<T&&>()[0]), wchar_t> )
    constexpr Paper(PaperConstructor_t, T const p, std::size_t const len)
    {
        using namespace cctype_constexpr;  // isdigit, tolower

        auto &letter = this->letter;
        auto &num    = this->num   ;
        auto &rev    = this->rev   ;

        while ( len >= 5u )  // fake loop only iterates once (so we can 'break')
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
                if ( 5u == len )  // N1234
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
                if ( len < 7u ) break;
                if ( 'r' != tolower(p[5]) ) break;
                if ( !isdigit(p[6]) ) break;
                rev = (p[6]-'0');

                if ( 7u == len )  // P1234R0
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
                    if ( (len > 8u) && ('\0' != p[8]) && ('.' != p[8]) ) break;  // P1234R15
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

        // The Microsoft compiler won't allow the throwing
        // of an exception inside a constexpr function (not
        // even if it never gets thrown), so the following
        // 'if' statement is a workaround:
        if ( std::is_constant_evaluated() )
        {
            assert( "invalid paper name string"==0 );
        }
        else
        {
            throw std::runtime_error("invalid paper name string");
        }
    }

public:
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

    constexpr Paper( char   const *const arg) : Paper( PaperConstructor_t(), arg, std::char_traits< char  >::length(arg) ) {}
    constexpr Paper(wchar_t const *const arg) : Paper( PaperConstructor_t(), arg, std::char_traits<wchar_t>::length(arg) ) {}

    constexpr Paper(std:: string_view const arg) : Paper( PaperConstructor_t(), arg.data(), arg.size() ) {}
    constexpr Paper(std::wstring_view const arg) : Paper( PaperConstructor_t(), arg.data(), arg.size() ) {}

    constexpr Paper(std:: string const &s) : Paper( PaperConstructor_t(), s.c_str(), s.size() ) {}
    constexpr Paper(std::wstring const &s) : Paper( PaperConstructor_t(), s.c_str(), s.size() ) {}

    constexpr bool operator<(Paper const other) const noexcept
    {
        // The terminator has letter == 'z', which will come after 'n' and 'p'
        if ( letter != other.letter ) return letter < other.letter;
        if (    num != other.num    ) return    num < other.num   ;
        return rev < other.rev;
    }

    constexpr bool IsRelatedTo(Paper const other) const noexcept
    {
        return (letter == other.letter) && (num == other.num);
    }

     char   const * c_str(void) const noexcept;
    wchar_t const *wc_str(void) const noexcept;
     char   const *PaperNameWithoutRevision (void) const noexcept;
    wchar_t const *PaperNameWithoutRevisionL(void) const noexcept;

#ifndef PAPERKERNELCXX_MINIMAL_PAPER
    wxString const &GetTitle (void) const noexcept;
    wxString const &GetAuthor(void) const noexcept;
    wxString        GetPaper (void) const noexcept;
#endif
};

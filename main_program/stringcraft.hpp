#ifndef INCLUSION_GUARD_57db1f4b_aa9e_4f3a_9050_20a9b04d6bc8
#define INCLUSION_GUARD_57db1f4b_aa9e_4f3a_9050_20a9b04d6bc8

/*
This header file allows you to use any null-terminated string as a
wxString without copying the original string (i.e. no dynamic memory
allocation required).
*/

#include <cstddef>         // ptrdiff_t, size_t
#include <cstdint>         // uint_fast64_t
#include <algorithm>       // max
#include <string>          // basic_string, string, wstring
#include <type_traits>     // is_reference, is_same
#include <wx/string.h>     // wxString, wxStringCharType

#if wxUSE_UNICODE_WCHAR    // See approx. line #150 in wx/chartype.h
    static_assert( std::is_same_v<wchar_t, wxStringCharType> );
    typedef std::wstring wxstring;
#else
    static_assert( std::is_same_v< char  , wxStringCharType> );
    typedef std::string wxstring;
#endif

template<std::size_t N>
struct StringLiteral {
    static constexpr std::size_t len = N;
    wxStringCharType value[N];
    consteval StringLiteral(wxStringCharType const (&str)[N]) noexcept
    {
        for ( std::size_t i = 0; i < N; ++i ) value[i] = str[i];
    }
    consteval wxStringCharType &operator[](std::size_t const arg) noexcept
    {
        return value[arg];
    }
    consteval wxStringCharType const &operator[](std::size_t const arg) const noexcept
    {
        return value[arg];
    }
};

consteval wxStringCharType tolower_consteval(wxStringCharType const c) noexcept
{
    switch ( c )
    {
        case wxS('A'): return wxS('a'); case wxS('B'): return wxS('b'); case wxS('C'): return wxS('c'); case wxS('D'): return wxS('d');
        case wxS('E'): return wxS('e'); case wxS('F'): return wxS('f'); case wxS('G'): return wxS('g'); case wxS('H'): return wxS('h');
        case wxS('I'): return wxS('i'); case wxS('J'): return wxS('j'); case wxS('K'): return wxS('k'); case wxS('L'): return wxS('l');
        case wxS('M'): return wxS('m'); case wxS('N'): return wxS('n'); case wxS('O'): return wxS('o'); case wxS('P'): return wxS('p');
        case wxS('Q'): return wxS('q'); case wxS('R'): return wxS('r'); case wxS('S'): return wxS('s'); case wxS('T'): return wxS('t');
        case wxS('U'): return wxS('u'); case wxS('V'): return wxS('v'); case wxS('W'): return wxS('w'); case wxS('X'): return wxS('x');
        case wxS('Y'): return wxS('y'); case wxS('Z'): return wxS('z');
    }

    return c;
}

template<StringLiteral input>
consteval std::uint_fast64_t Hash(void)
{
    std::uint_fast64_t h = 0xcbf29ce484222325;

    for ( std::size_t i = 0u; i < decltype(input)::len; ++i )
    {
        wxStringCharType const c = tolower_consteval(input[i]);
        bool is_letter = false;
        for ( wxStringCharType const letter : wxS("abcdefghijklmnopqrstuvwxyz") )
        {
            if ( letter == c )
            {
                is_letter = true;
                break;
            }
        }
        if ( false == is_letter ) continue;
        h ^= c;
        h *= 0x100000001B3;
    }

    return h;
}

static_assert( Hash<wxS("Thomas PK Healy")>() == Hash<wxS("thomas p. k. healy")>() );

#ifdef PAPERKERNEL_DONT_USE_STRING_PRETENDER

// The next function is for getting an std::basic_string
// from a wxString without copying anything
inline wxstring const &wxString_inner(wxString const &s)
{
#if defined(_MSC_VER) && defined(_DEBUG) && !defined(NDEBUG)
    constexpr std::ptrdiff_t offset = 8u;
#else
    constexpr std::ptrdiff_t offset = 0u;
#endif

    // We don't need the complication of calling ToStdString or ToStdWstring,
    // we can just assume that the std::basic_string is at offset 0 or 8 inside
    // the wxString object
    return
      *static_cast<wxstring const*>(
         static_cast<void const*>(
           static_cast<char const*>(
             static_cast<void const*>(&s) ) + offset ));
}

// The next POD struct mimics the layout of std::basic_string so that
// it can be configured to refer to any string without copying the string
struct Pretender_wxstring {

#if defined(_MSC_VER) && defined(_DEBUG) && !defined(NDEBUG)
    void *_Myproxy;    // Iterator debugging proxy (Debug mode only)
#endif

    // This layout works for libstdc++ and also the Microsoft compiler
    wxStringCharType const *p;
    std::size_t len, allocd;

    template<std::size_t bytes>
    constexpr Pretender_wxstring(wxStringCharType const (&arg)[bytes]) noexcept
        :
#if defined(_MSC_VER) && defined(_DEBUG) && !defined(NDEBUG)
        _Myproxy(nullptr),    // Iterator debugging proxy (Debug mode only)
#endif
      p(arg), len(bytes - 1u), allocd( std::max(32uz, bytes - 1uz) ) {}

    constexpr operator wxstring const&(void) const noexcept
    {
        return *static_cast<wxstring const*>(static_cast<void const*>(this));
    }

    constexpr operator wxString const&(void) const noexcept
    {
        return *static_cast<wxString const*>(static_cast<void const*>(this));
    }
};

typedef Pretender_wxstring Pretender_wxString;

#else

// The next function is for getting an std::basic_string
// from a wxString without copying anything
inline wxstring const &wxString_inner(wxString const &s)
{
#if wxUSE_UNICODE_WCHAR
    static_assert( std::is_reference_v< decltype(s.ToStdWstring()) > );
    return s.ToStdWstring();
#else
    static_assert( std::is_reference_v< decltype(s.ToStdString()) > );
    return s.ToStdString();
#endif
}

struct Pretender_wxString {

    wxString wxs;

    template<std::size_t bytes>
    constexpr Pretender_wxString(wxStringCharType const (&arg)[bytes]) noexcept
        : wxs(arg) {}

    operator wxString const&(void) const noexcept
    {
        return wxs;
    }
};


#endif

#endif    // ifndef INCLUSION_GUARD

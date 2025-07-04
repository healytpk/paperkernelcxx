#ifndef INCLUSION_GUARD_57db1f4b_aa9e_4f3a_9050_20a9b04d6bc8
#define INCLUSION_GUARD_57db1f4b_aa9e_4f3a_9050_20a9b04d6bc8

/*
This header file allows you to use any null-terminated string as a
wxString without copying the original string (i.e. no dynamic memory
allocation required).
*/

#define PAPERKERNEL_DONT_USE_STRING_PRETENDER

#include <cstddef>         // ptrdiff_t, size_t
#include <cstdint>         // uint_fast64_t
#include <algorithm>       // max
#include <string>          // basic_string     , string     , wstring
#include <string_view>     // basic_string_view, string_view, wstring_view
#include <type_traits>     // is_reference, is_same
#include <wx/string.h>     // wxString, wxStringCharType
#include "hash.hpp"        // Hash (consteval)

#if wxUSE_UNICODE_WCHAR    // See approx. line #150 in wx/chartype.h
    static_assert( std::is_same_v<wchar_t, wxStringCharType> );
    typedef std::wstring wxstring;
    typedef std::wstring_view wxstring_view;
#else
    static_assert( std::is_same_v< char  , wxStringCharType> );
    typedef std::string wxstring;
    typedef std::string_view wxstring_view;
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

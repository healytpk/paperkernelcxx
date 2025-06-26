#ifndef INCLUSION_GUARD_57db1f4b_aa9e_4f3a_9050_20a9b04d6bc8
#define INCLUSION_GUARD_57db1f4b_aa9e_4f3a_9050_20a9b04d6bc8

/*
This header file allows you to use any null-terminated string as a
wxString without copying the original string (i.e. no dynamic memory
allocation required).
*/

#include <cstddef>         // size_t
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

// The next function is for getting an std::basic_string
// from a wxString without copying anything
inline wxstring const &wxString_inner(wxString const &s)
{
    // We don't need the complication of calling ToStdString or ToStdWstring,
    // we can just assume that the std::basic_string is at offset 0 inside
    // the wxString object
    return *static_cast<wxstring const*>(static_cast<void const*>(&s));
}

// The next POD struct mimics the layout of std::basic_string so that
// it can be configured to refer to any string without copying the string
struct Pretender_wxstring {

    // This layout works for libstdc++ and also the Microsoft compiler
    wxStringCharType const *p;
    std::size_t len, allocd;

    template<std::size_t bytes>
    constexpr Pretender_wxstring(wxStringCharType const (&arg)[bytes]) noexcept
      : p(arg), len(bytes - 1u), allocd(bytes - 1u) {}

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

#endif    // ifndef INCLUSION_GUARD

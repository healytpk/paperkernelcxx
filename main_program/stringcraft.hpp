#pragma once
#include <cstddef>         // size_t
#include <map>             // map
#include <string>          // basic_string
#include <type_traits>     // is_reference, is_same
#include <wx/string.h>     // wxString, wxStringCharType

#if wxUSE_UNICODE_WCHAR    // See line #150 in wx/chartype.h
    static_assert( std::is_same_v<wchar_t, wxStringCharType> );
    typedef std::wstring wxstring;
#else
    static_assert( std::is_same_v< char  , wxStringCharType> );
    typedef std::string wxstring;
#endif

inline wxstring const &inner(wxString const &s)
{
    // We don't need the complication of calling ToStdString or ToStdWstring
    //static_assert(  sizeof(wxString) ==  sizeof(wxstring) );
    //static_assert( alignof(wxString) == alignof(wxstring) );
    return *static_cast<wxstring const*>(static_cast<void const*>(&s));
}

struct Pretender_wxstring {

#ifdef _MSC_VER
    wxStringCharType const *p;
    std::size_t len, allocd;
#else
    std::size_t allocd, len;
    wxStringCharType const *p;
#endif

    template<std::size_t bytes>
    constexpr Pretender_wxstring(wxStringCharType const (&arg)[bytes]) noexcept
      : p(arg), len(bytes - 1u), allocd(bytes - 1u) {}

    operator wxstring const&(void) const noexcept
    {
        return *static_cast<wxstring const*>(static_cast<void const*>(this));
    }

    operator wxString const& (void) const noexcept
    {
        return *static_cast<wxString const*>(static_cast<void const*>(this));
    }
};

typedef Pretender_wxstring Pretender_wxString;

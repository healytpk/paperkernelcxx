#pragma once

#include <string_view>      // string_view
#include <type_traits>      // is_same
#include <wx/string.h>      // wxString

static_assert( std::is_same_v< wxChar, wchar_t > || std::is_same_v< wxChar, char > );

class wxStringView : public std::basic_string_view<wxChar> { 
public:
    typedef std::basic_string_view<wxChar> Base;
    using Base::Base;
    wxStringView(wxString const &arg) noexcept
      : Base( static_cast<wxChar const*>(arg.data()), arg.size() ) {}
};

#include "header.hpp"
#include <wx/string.h>     // wxUSE_UNICODE_WCHAR
#include <iostream>        // cout, wcout, endl

using std::cout, std::wcout, std::endl;

#if wxUSE_UNICODE_WCHAR    // See approx. line #150 in wx/chartype.h
#    define wxcout wcout
#else
#    define wxcout  cout
#endif

int main(void)
{
    constexpr auto found = *FindAuthor("TPK Healy");
    wxcout << found.name << endl;
}

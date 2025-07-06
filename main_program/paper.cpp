#include "paper.hpp"

namespace Paper_detail {

template<typename T> requires( std::is_same_v<T,char> || std::is_same_v<T,wchar_t> )
T *null_terminated_string(Paper const *const this_paper)
{
    auto const &letter = this_paper->letter;
    auto const &num    = this_paper->num   ;
    auto const &rev    = this_paper->rev   ;

    static thread_local T s[] = { 'p', '1', '2', '3', '4', 'r', '5', '6', '\0' };

    assert( ('n'==letter) || ('p'==letter) );
    s[0] = letter;
    s[1] = '0' + num / 1000u % 10u;
    s[2] = '0' + num /  100u % 10u;
    s[3] = '0' + num /   10u % 10u;
    s[4] = '0' + num /    1u % 10u;

    switch ( letter )
    {
    case 'n':
        s[5] = '\0';
        break;

    case 'p':
        s[5] = 'r';
        if ( rev < 10u )
        {
            s[6] = '0' + rev;
            s[7] = '\0';
        }
        else
        {
            s[6] = '0' + rev / 10u % 10u;
            s[7] = '0' + rev /  1u % 10u;
            s[8] = '\0';
        }
        break;
    }

    return s;
}

}  // close namespace 'Paper_detail'

char    const *Paper:: c_str(void) const noexcept { return Paper_detail::null_terminated_string< char  >(this); }
wchar_t const *Paper::wc_str(void) const noexcept { return Paper_detail::null_terminated_string<wchar_t>(this); }

char const *Paper::PaperNameWithoutRevision(void) const noexcept
{
    char *const p = Paper_detail::null_terminated_string<char>(this);
    p[5] = '\0';
    return p;
}

wchar_t const *Paper::PaperNameWithoutRevisionL(void) const noexcept
{
    wchar_t *const p = Paper_detail::null_terminated_string<wchar_t>(this);
    p[5] = '\0';
    return p;
}

#ifndef PAPERKERNELCXX_MINIMAL_PAPER

#include <cstdlib>                      // abort
#include <algorithm>                    // lower_bound
#include <iterator>                     // cbegin, cend
#include <type_traits>                  // is_same
#include "tree_paper.hpp"               // g_map_papers

#ifndef NDEBUG
#    include <iostream>                 // cout, endl
#endif

namespace Paper_detail {

wxString const &Paper_GetDatumFromPaperTree(Paper const *const pthis, unsigned const n)
{
    auto const it = std::lower_bound( std::cbegin(g_map_papers), std::cend(g_map_papers),
                                      *pthis,
                                      [](auto &&arg1, auto &&arg2) { return arg1.paper < arg2; } );

    assert( std::cend(g_map_papers) != it );

    for ( PaperRevInfo_t const *p = it->prevs; PaperRevInfo_t::terminator != p->rev; ++p )
    {
        if ( pthis->rev != p->rev ) continue;
        switch ( n )
        {
        case 1u: return *new wxString(p->title);                      // REVISIT - FIX - This is horrible
        case 2u: return *new wxString() << p->hashes_authors[0];      // REVISIT - FIX - This is horrible
        }
    }
#ifndef NDEBUG
    std::cout << "pthis == " << pthis->c_str() << std::endl;
#endif
    assert( nullptr == "invalid paper not listed in tree" );
    std::abort();    // if NDEBUG
    return *new wxString;  // suppress compiler warning
}

}  // close namespace Paper_detail

wxString const &Paper::GetTitle(void) const noexcept
{
    return Paper_detail::Paper_GetDatumFromPaperTree(this,1u);
}

wxString const &Paper::GetAuthor(void) const noexcept
{
    return Paper_detail::Paper_GetDatumFromPaperTree(this,2u);
}

wxString Paper::GetPaper(void) const noexcept
{
    return Paper_detail::null_terminated_string< wxStringCharType >(this);
}

#endif     // ifndef PAPERKERNELCXX_MINIMAL_PAPER

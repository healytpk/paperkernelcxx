#include "paper.hpp"
#include <cassert>                          // assert
#include <cstdlib>                          // abort
#include <cctype>                           // isdigit, tolower
#include <stdexcept>                        // runtime_error
#include <tuple>                            // tuple
#include <vector>                           // vector

#ifndef PAPERKERNELCXX_MINIMAL_PAPER
#    include "papertree.hpp"                    // g_map_papers
#endif

using std::vector, std::tuple;

Paper::Paper(std::string_view const p) noexcept(false)
{
    using std::isdigit, std::tolower;

    while ( p.size() >= 7u )
    {
        if ( 'p' != tolower(p[0]) ) break;
        if ( !isdigit(p[1]) ) break;
        if ( !isdigit(p[2]) ) break;
        if ( !isdigit(p[3]) ) break;
        if ( !isdigit(p[4]) ) break;
        num  = (p[1]-'0') * 1000u;
        num += (p[2]-'0') *  100u;
        num += (p[3]-'0') *   10u;
        num += (p[4]-'0') *    1u;
        if ( 'r' != tolower(p[5]) ) break;
        if ( !isdigit(p[6]) ) break;
        rev = (p[6]-'0');

        if ( 7u == p.size() )  // P1234R0
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
        else if ( ('\0' == p[7]) || ('.' == p[7]) )  // length is >= 8
        {
            // fall through
        }
        else  // length is >= 8
        {
            break;
        }
        
        return;
    }

    throw std::runtime_error("invalid paper name string");
}

char const *Paper::c_str(void) const noexcept
{
    static thread_local char s[] = "p1234r56";

    s[1] = '0' + num / 1000u % 10u;
    s[2] = '0' + num /  100u % 10u;
    s[3] = '0' + num /   10u % 10u;
    s[4] = '0' + num /    1u % 10u;

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

    return s;
}

#ifdef PAPERKERNELCXX_MINIMAL_PAPER

char const *Paper::GetTitle(void) noexcept
{
    return "Title";
}

char const *Paper::GetAuthor(void) noexcept
{
    return "Author";
}

#else

static char const *Paper_GetDatumFromPaperTree(Paper const *const pthis, unsigned const n)
{
    auto const it = g_map_papers.find( pthis->num );
    assert( g_map_papers.end() != it );
    auto const &vec = it->second;
    for ( auto const &e : vec )  // 'e' is a 'tuple<unsigned, char const*, char const* >'
    {
        if ( pthis->rev != std::get<0u>(e) ) continue;
        switch ( n )
        {
        case 1u: return std::get<1u>(e);
        case 2u: return std::get<2u>(e);
        }
    }
    assert( nullptr == "invalid paper not listed in tree" );
    std::abort();    // if NDEBUG
    return nullptr;  // suppress compiler warning
}

char const *Paper::GetTitle(void) noexcept
{
    return Paper_GetDatumFromPaperTree(this,1u);
}

char const *Paper::GetAuthor(void) noexcept
{
    return Paper_GetDatumFromPaperTree(this,2u);
}

#endif

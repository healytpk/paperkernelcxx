#pragma once

#include <map>
#include <tuple>
#include <utility>
#include "paper.hpp"

#ifdef NDEBUG
#   include <boost/container/small_vector.hpp>
    extern std::map< long long unsigned, std::pair< wchar_t const*, boost::container::small_vector<Paper, 4u> > > g_map_authors;
#else
#   include <vector>
    extern std::map< long long unsigned, std::pair< wchar_t const*, std::vector<Paper> > > g_map_authors;
#endif

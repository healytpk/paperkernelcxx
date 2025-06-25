#pragma once

#include <map>
#include <tuple>
#include <utility>
#include "paper.hpp"
#include "stringcraft.hpp"

#if 1
#   include <vector>
    extern std::pair< long long unsigned, std::pair< wxString, std::vector<Paper> > > const g_map_authors[216u];
#elif defined(NDEBUG)
#   include <boost/container/small_vector.hpp>
    extern std::map< long long unsigned, std::pair< Pretender_wxString, boost::container::small_vector<Paper, 4u> > > const g_map_authors;
#elif 0
#   include <vector>
    extern std::map< long long unsigned, std::pair< Pretender_wxString, std::vector<Paper> > > const g_map_authors;
#endif

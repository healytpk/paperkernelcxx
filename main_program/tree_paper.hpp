#pragma once

#include <map>
#include <tuple>
#include "stringcraft.hpp"

#if 1
#   include <vector>
    extern std::pair<unsigned, std::vector< std::tuple<unsigned, wxStringCharType const *, wxStringCharType const *> > > const g_map_papers[3025u];
#elif defined(NDEBUG)
#   include <boost/container/small_vector.hpp>
    extern std::map< unsigned, boost::container::small_vector< std::tuple<unsigned, Pretender_wxString, Pretender_wxString >, 4u > > const g_map_papers;
#elif 0
#   include <vector>
    extern std::map< unsigned, std::vector< std::tuple<unsigned, Pretender_wxString, Pretender_wxString> > > const g_map_papers;
#endif

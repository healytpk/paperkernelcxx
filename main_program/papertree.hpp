#pragma once

#include <map>
#include <tuple>

#ifdef NDEBUG
#   include <boost/container/small_vector.hpp>
    extern std::map< unsigned, boost::container::small_vector< std::tuple<unsigned, char const*, char const* >, 4u > > const g_map_papers;
#else
#   include <vector>
    extern std::map< unsigned, std::vector< std::tuple<unsigned, char const*, char const*> > > const g_map_papers;
#endif

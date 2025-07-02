#pragma once

#include <cstdint>                          // uint_fast64_t
#include <utility>                          // pair
#include <vector>                           // vector
#include "paper.hpp"                        // Paper
#include "stringcraft.hpp"                  // Pretender_wxString

extern std::pair< std::uint_fast64_t, std::pair< Pretender_wxString, std::vector<Paper> > > const g_map_authors[216u] =
#    include "AUTO_GENERATED_tree_contents_author.hpp"
;

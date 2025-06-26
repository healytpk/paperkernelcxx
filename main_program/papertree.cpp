#include "papertree.hpp"

#if 1
extern std::pair<unsigned, std::vector< std::tuple<unsigned, wxStringCharType const*, wxStringCharType const*> > > const g_map_papers[3025] =
#else
decltype(g_map_papers) g_map_papers =
#endif

#if wxUSE_UNICODE_WCHAR
#    include "AUTO_GENERATED_tree_contents_paper_wchar_t.hpp"
#else
#    include "AUTO_GENERATED_tree_contents_paper_char.hpp"
#endif

;

#ifndef HEADER_INCLUSION_GUARD_8A2B4C1D_7E3F_4B2A_9C1D_5F6E7A8B9C0D
#define HEADER_INCLUSION_GUARD_8A2B4C1D_7E3F_4B2A_9C1D_5F6E7A8B9C0D

#include <cctype>                   // isalnum, tolower
#include <cstddef>                  // size_t
#include <cstdint>                  // uint_fast64_t
#include <algorithm>                // ranges::sort, ranges::lower_bound, ranges::equal_range
#include <concepts>                 // input_iterator
#include <fstream>                  // ofstream
#include <map>                      // map
#include <ranges>                   // ranges
#include <string>                   // string
#include <string_view>              // string_view
#include <unordered_map>            // unordered_map
#include <utility>                  // move
#include <vector>                   // vector

class NameManager {
public:
    using size_t = std::size_t;
    using uint_fast64_t = std::uint_fast64_t;
    using string = std::string;
    using string_view = std::string_view;
    template<typename... Ts> using map = std::map<Ts...>;
    template<typename... Ts> using set = std::set<Ts...>;
    template<typename... Ts> using unordered_map = std::unordered_map<Ts...>;
    template<typename... Ts> using vector = std::vector<Ts...>;

private:
    static map<string_view, string_view> const &ExceptionMap(void)
    {
        static const map<string_view, string_view> exceptions = {
            { "Alex"                                                   , "Alex Waffl3x"                                 },
            { "Hana Dusikova"                                          , "Hana Dus\\u00EDkov\\u00E1"                    },
            { "Andrzej Krzemienski"                                    , "Andrzej Krzemie\\u0144ski"                    },
            { "Bengt Gustafsonn"                                       , "Bengt Gustafsson"                             },
            { "Billy O'Neal"                                           , "Billy Robert O'Neal III"                      },
            { "Daniel Krugler"                                         , "Daniel Kr\\u00FCgler"                         },
            { "Dietmar Kuehl"                                          , "Dietmar K\\u00FChl"                           },
            { "Dietmar Kuhl"                                           , "Dietmar K\\u00FChl"                           },
            { "Domagoj Saric"                                          , "Domagoj \\u0160ari\\u0107"                    },
            { "Gonzalo Brito"                                          , "Gonzalo Brito Gadeschi"                       },
            { "J. J\\u4CB6i"                                           , "Jaakko J\\u00e4rvi"                           },
            { "J. Jarvi"                                               , "Jaakko J\\u00e4rvi"                           },
            { "J. Jaarvi"                                              , "Jaakko J\\u00e4rvi"                           },
            { "J. J\\u4CB6i"                                           , "Jaakko J\\u00e4rvi"                           },
            { "J. J\\u00E4rvi"                                         , "Jaakko J\\u00e4rvi"                           },
        };
        return exceptions;
    }

    vector<string     >         m_name_storage;
    vector<string_view>         m_names;
    vector<size_t     > mutable m_parent;

    map<string_view, string_view> mutable m_map_clusters;
    bool mutable m_map_clusters_dirty = true;

    static void ToLowerAndRemovePunct(string &s)
    {
        string result;
        for ( char const c : s )
        {
            if ( std::isalnum(static_cast<unsigned char>(c)) || (c == ' ') )
                result += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            else if ( c == '.' || c == '-' )
                result += ' ';
        }
        s = std::move(result);
    }

    static vector<string> SplitTokens(string_view const s)
    {
        vector<string> tokens;
        size_t pos = 0u;
        while ( pos < s.size() )
        {
            while ( pos < s.size() && s[pos] == ' ' ) ++pos;
            size_t const start = pos;
            while ( pos < s.size() && s[pos] != ' ' ) ++pos;
            if ( start < pos ) tokens.emplace_back(  s.substr(start, pos - start)  );
        }
        return tokens;
    }

    static string GetSurname( vector<string> const &tokens )
    {
        if ( tokens.empty() ) return {};
        return tokens.back();
    }

    static vector<string> GetGivenNames( vector<string> const &tokens )
    {
        if ( tokens.size() <= 1u ) return {};
        return vector<string>( tokens.begin(), tokens.end() - 1 );
    }

    static string ExtractInitials( vector<string> const &tokens )
    {
        string initials;
        for ( auto const &t : tokens ) if ( !t.empty() ) initials += t[0];
        return initials;
    }

    static bool IsSubsequence(string_view const shorter, string_view const longer)
    {
        size_t i = 0u, j = 0u;
        while ( (i < shorter.size()) && (j < longer.size()) )
        {
            if ( shorter[i] == longer[j] ) ++i;
            ++j;
        }
        return i == shorter.size();
    }

    static bool IsOneTypoApart(string_view const a, string_view const b)
    {
        size_t len_a = a.size(), len_b = b.size();
        if ( len_a == len_b )
        {
            unsigned diff = 0u;
            for ( size_t i = 0u; i < len_a; ++i ) if ( a[i] != b[i] ) ++diff;
            return diff == 1u;
        }
        if ( (len_a + 1u) == len_b)
        {
            for (size_t i = 0u, j = 0u; (i < len_a) && (j < len_b); )
            {
                if ( a[i] != b[j] )
                {
                    if ( i != j ) return false;
                    ++j;
                } else
                {
                    ++i; ++j;
                }
            }
            return true;
        }
        if ( (len_b + 1u) == len_a )
        {
            for ( size_t i = 0u, j = 0u; i < len_a && j < len_b; )
            {
                if ( a[i] != b[j] )
                {
                    if ( i != j ) return false;
                    ++i;
                } else
                {
                    ++i; ++j;
                }
            }
            return true;
        }
        return false;
    }

    static bool IsSamePerson(string_view a, string_view b)
    {
        string sa(a), sb(b);
        ToLowerAndRemovePunct(sa);
        ToLowerAndRemovePunct(sb);

        auto const tokensA = SplitTokens(sa);
        auto const tokensB = SplitTokens(sb);

        if ( tokensA.empty() || tokensB.empty() ) return false;

        auto const surnameA = GetSurname(tokensA);
        auto const surnameB = GetSurname(tokensB);

        if ( (surnameA != surnameB) && (false == IsOneTypoApart(surnameA, surnameB)) ) return false;

        auto const givenA = GetGivenNames(tokensA);
        auto const givenB = GetGivenNames(tokensB);

        string const initialsA = ExtractInitials(givenA);
        string const initialsB = ExtractInitials(givenB);

        return IsSubsequence(initialsA, initialsB) || IsSubsequence(initialsB, initialsA) || IsOneTypoApart(sa, sb);
    }

    size_t Find(size_t x) const
    {
        while ( m_parent[x] != x ) x = m_parent[x] = m_parent[m_parent[x]];
        return x;
    }

    void Unite(size_t const a, size_t const b)
    {
        size_t const pa = Find(a);
        size_t const pb = Find(b);
        if ( pa != pb ) m_parent[pa] = pb;
    }

    void SortClusterIndices(vector<size_t>& cluster_indices) const
    {
        std::ranges::sort(cluster_indices,
            [this](size_t a, size_t b)
            {
                auto const tokensA = SplitTokens(m_names[a]);
                auto const tokensB = SplitTokens(m_names[b]);
                if (tokensA.size() != tokensB.size()) return tokensA.size() > tokensB.size();
                return m_names[a].size() > m_names[b].size();
            });
    }

public:
    static constexpr size_t pad_len = 35u;

    template<std::input_iterator Iterator>
    void AddNames(Iterator it_begin, Iterator it_end)
    {
        vector<string_view> new_names;
        for ( auto it = it_begin; it != it_end; ++it ) if ( ExceptionMap().find(*it) == ExceptionMap().end() ) new_names.emplace_back(*it);

        size_t const old_size = m_names.size();
        m_names.insert( m_names.end(), new_names.begin(), new_names.end() );

        m_parent.resize( m_names.size() );
        for ( size_t i = old_size; i < m_parent.size(); ++i ) m_parent[i] = i;

        for ( size_t i = 0u; i < m_names.size(); ++i )
            for (size_t j = i + 1u; j < m_names.size(); ++j )
                if ( IsSamePerson(m_names[i], m_names[j]) )
                    Unite(i, j);

        m_map_clusters_dirty = true;
    }

    string_view GetPrimaryName(string_view const alt) const
    {
        // Check hardcoded exceptions first
        auto it = ExceptionMap().find(alt);
        if ( it != ExceptionMap().end() ) return it->second;

        size_t idx = m_names.size();
        for ( size_t i = 0u; i < m_names.size(); ++i )
        {
            if ( alt == m_names[i] )
            {
                idx = i;
                break;
            }
        }

        if ( idx == m_names.size() ) return alt;

        size_t const root = Find(idx);
        vector<size_t> cluster_indices;
        for ( size_t i = 0u; i < m_names.size(); ++i )
            if ( root == Find(i) )
                cluster_indices.push_back(i);

        SortClusterIndices(cluster_indices);

        return m_names[ cluster_indices.front() ];
    }

    map<string_view, string_view> const &GetMap(void) const
    {
        if ( false == m_map_clusters_dirty ) return m_map_clusters;
        
        m_map_clusters.clear();
        for ( auto const & [alt, primary] : ExceptionMap() ) m_map_clusters[alt] = primary;

        vector<bool> visited(m_names.size(), false);
        for ( size_t i = 0u; i < m_names.size(); ++i )
        {
            size_t const root = Find(i);
            if ( visited[root] ) continue;
            vector<size_t> cluster_indices;
            for ( size_t j = 0u; j < m_names.size(); ++j )
                if ( root == Find(j) )
                    cluster_indices.push_back(j);

            SortClusterIndices(cluster_indices);

            string_view const primary = m_names[ cluster_indices.front() ];
            for ( size_t idx : cluster_indices )
                if ( primary != m_names[idx] )
                    m_map_clusters[ m_names[idx] ] = primary;

            visited[root] = true;
        }

        m_map_clusters_dirty = false;
        return m_map_clusters;
    }

    map< string_view, vector<string_view> > GetPrimaryToAlternativesMap(void) const
    {
        map< string_view, vector<string_view> > result;
        vector<bool> visited( m_names.size(), false );
        for ( size_t i = 0u; i < m_names.size(); ++i )
        {
            size_t const root = Find(i);
            if ( visited[root] ) continue;
            vector<size_t> cluster_indices;
            for ( size_t j = 0u; j < m_names.size(); ++j )
                if ( root == Find(j) )
                    cluster_indices.push_back(j);

            SortClusterIndices(cluster_indices);

            string_view const primary = m_names[ cluster_indices.front() ];
            vector<string_view> alternatives;
            for ( size_t idx : cluster_indices )
                if ( primary != m_names[idx] )
                    alternatives.push_back(m_names[idx]);

            if ( false == alternatives.empty() ) result[primary] = std::move(alternatives);

            visited[root] = true;
        }

        // Add hardcoded exceptions
        for ( auto const & [alt, primary] : ExceptionMap() ) result[primary].push_back(alt);
        return result;
    }

    template<std::input_iterator Iterator>
    void WriteHeaders(Iterator const itBegin, Iterator const itEnd, char const *const output_file) const
    {
        constexpr auto escape_for_hash = [](string_view const s) -> string
          {
            string out;
            out.reserve( s.size() );
            for ( char const c : s )
            {
                out += c;
                if ( '\\' == c ) out += c;
            }
            return out;
          };

        constexpr auto pad_spaces = [](size_t const s, size_t const width = pad_len) -> string
          {
            return (s < width) ? string(width - s, ' ') : string();
          };

        NameManager nm;
        nm.AddNames(itBegin, itEnd);

        auto const groups = nm.GetPrimaryToAlternativesMap();

        // --- Minimal fix: Build set of all alternatives (including exceptions) ---
        set<string> all_alternatives;
        for (const auto& [primary, alts] : groups)
            for (const auto& alt : alts)
                all_alternatives.insert(string(alt));
        for (const auto& [alt, primary] : ExceptionMap())
            all_alternatives.insert(string(alt));
        // ------------------------------------------------------------------------

        set<string> all_primaries;
        for ( auto const & [primary, alts] : groups )
            if (all_alternatives.find(string(primary)) == all_alternatives.end())
                all_primaries.insert( string(primary) );
        for ( Iterator it = itBegin; it != itEnd; ++it )
        {
            auto const &name = *it;
            auto const p = nm.GetPrimaryName(name);
            if ( (p == name) && (groups.find(p) == groups.end()) && (all_alternatives.find(string(p)) == all_alternatives.end()) )
                all_primaries.insert( string(p) );
        }

        struct PrimaryEntry {
            string name;
            uint_fast64_t hash;
        };
        vector<PrimaryEntry> primary_entries;
        for ( auto const &pname : all_primaries )
        {
            auto const hash = Hash(pname);
            primary_entries.emplace_back( pname, hash );
        }

        struct AltEntry {
            string alt_name;
            string primary_name;
            uint_fast64_t alt_hash;
            uint_fast64_t primary_hash;
        };
        vector<AltEntry> alt_entries;
        for ( auto const & [primary, alts] : groups)
        {
            auto const primary_hash = Hash(std::string(primary));
            for ( auto const & alt : alts )
            {
                auto const alt_hash = Hash( string(alt) );
                alt_entries.push_back( {string(alt), string(primary), alt_hash, primary_hash} );
            }
        }
 
        std::ofstream out(output_file);
        out << "#pragma once\n";
        out << "#include <cstdint>            // uint_fast64_t\n";
        out << "#include <algorithm>          // ranges::lower_bound, ranges::equal_range\n";
        out << "#include <array>              // array\n";
        out << "#include <tuple>              // tuple\n";
        out << "#include <utility>            // pair\n";
        out << "#include <vector>             // vector\n";
        out << "#include <wx/string.h>        // wxS, wxStringCharType\n";
        out << "#include \"hash.hpp\"           // Hash\n\n";

        // Primary names sorted alphabetically (not by hash)
        out << "inline constexpr std::pair< std::uint_fast64_t, wxStringCharType const * > g_primary_names_unsorted[] = {\n";
        for ( size_t i = 0u; i < primary_entries.size(); ++i )
        {
            auto const &e = primary_entries[i];
            auto const hash_str = escape_for_hash(e.name);
            out << "    { Hash(\"" << hash_str << "\"" << pad_spaces(hash_str.size()) << ")"
                << ", wxS(\"" << e.name << "\"" << pad_spaces(e.name.size()) << ") }";
            if ( i+1 != primary_entries.size() ) out << ",";
            out << "\n";
        }
        out << "};\n\n";

        // Alternative names sorted alphabetically (not by hash)
        out << "inline constexpr std::tuple<std::uint_fast64_t,wxStringCharType const *,std::uint_fast64_t> g_alternative_names_unsorted[] = {\n";
        for ( size_t i = 0u; i < alt_entries.size(); ++i )
        {
            auto const &e = alt_entries[i];
            auto const alt_hash_str = escape_for_hash(e.alt_name);
            auto const alt_name_str = e.alt_name;
            auto const primary_hash_str = escape_for_hash(e.primary_name);
            out << "    { Hash(\"" << alt_hash_str << "\"" << pad_spaces(alt_hash_str.size()) << ")"
                << ", wxS(\"" << alt_name_str << "\"" << pad_spaces(alt_name_str.size()) << ")"
                << ", Hash(\"" << primary_hash_str << "\"" << pad_spaces(primary_hash_str.size()) << ") }";
            if ( (i+1u) != alt_entries.size() ) out << ",";
            out << "\n";
        }
        out << "};\n\n";

        out << "template<typename T, std::size_t N>\n";
        out << "consteval auto container_sorted_by_first(T const (&arg)[N]) -> std::array<T, N>\n";
        out << "{\n";
        out << "    std::array<T, N> retval{};\n";
        out << "    for ( std::size_t i = 0u; i < N; ++i ) retval[i] = arg[i];    // copy_n gives error on g++ Debug\n";
        out << "    for ( std::size_t i = 1; i < N; ++i )\n";
        out << "    {\n";
        out << "        T key = retval[i];\n";
        out << "        std::size_t j = i;\n";
        out << "        while ( (0u != j) && (std::get<0u>(key) < std::get<0u>(retval[j-1u]) ) )\n";
        out << "        {\n";
        out << "            retval[j] = retval[j - 1];\n";
        out << "            --j;\n";
        out << "        }\n";
        out << "        retval[j] = key;\n";
        out << "    }\n";
        out << "    return retval;\n";
        out << "}\n\n";

        // Arrays sorted by hash
        out << "inline constexpr auto g_primary_names     = container_sorted_by_first(g_primary_names_unsorted    );\n";
        out << "inline constexpr auto g_alternative_names = container_sorted_by_first(g_alternative_names_unsorted);\n\n";

        // Four helper functions
        out << "inline constexpr std::uint_fast64_t PrimaryHash(wxStringCharType const *const name)\n";
        out << "{\n";
        out << "    std::uint_fast64_t const h = Hash(name);\n";
        out << "    auto const it = std::ranges::lower_bound(\n";
        out << "        g_alternative_names, h, {}, [](auto const &e) { return std::get<0u>(e); }\n";
        out << "    );\n";
        out << "    if ((it != std::end(g_alternative_names)) && (std::get<0u>(*it) == h))\n";
        out << "        return std::get<2u>(*it);\n";
        out << "    return h;\n";
        out << "}\n\n";
        out << "inline constexpr wxStringCharType const *HashToDirectString(std::uint_fast64_t const h)\n";
        out << "{\n";
        out << "    auto const it = std::ranges::lower_bound(\n";
        out << "        g_primary_names, h, {}, [](auto const &e) { return std::get<0u>(e); }\n";
        out << "    );\n";
        out << "    if ( (it != std::end(g_primary_names)) && (std::get<0u>(*it) == h) ) return std::get<1u>(*it);\n";
        out << "\n";
        out << "    auto const it2 = std::ranges::lower_bound(\n";
        out << "        g_alternative_names, h, {}, [](auto const &e) { return std::get<0u>(e); }\n";
        out << "    );\n";
        out << "    if ( (it2 != std::end(g_alternative_names)) && (std::get<0u>(*it2) == h) ) return std::get<1u>(*it2);\n";
        out << "    return nullptr;\n";
        out << "}\n\n";
        out << "inline constexpr wxStringCharType const *Primary(wxStringCharType const *const name)\n";
        out << "{\n";
        out << "    std::uint_fast64_t const h = PrimaryHash(name);\n";
        out << "    auto const it = std::ranges::lower_bound(\n";
        out << "        g_primary_names, h, {}, [](auto const &e) { return std::get<0u>(e); }\n";
        out << "    );\n";
        out << "    if ((it != std::end(g_primary_names)) && (std::get<0u>(*it) == h))\n";
        out << "        return std::get<1u>(*it);\n";
        out << "    return nullptr;\n";
        out << "}\n\n";
        out << "inline std::vector<wxStringCharType const*> GetAllAlternatives(std::uint_fast64_t const primary_hash)\n";
        out << "{\n";
        out << "    std::vector<wxStringCharType const*> result;\n";
        out << "    auto const myrange = std::ranges::equal_range(\n";
        out << "        g_alternative_names, primary_hash, {}, [](auto const &e) { return std::get<2u>(e); }\n";
        out << "    );\n";
        out << "    for (auto it = myrange.begin(); it != myrange.end(); ++it)\n";
        out << "        result.emplace_back(std::get<1u>(*it));\n";
        out << "    return result;\n";
        out << "}\n";
    }
};

#endif    // ifndef HEADER_INCLUSION_GUARD_8A2B4C1D_7E3F_4B2A_9C1D_5F6E7A8B9C0D

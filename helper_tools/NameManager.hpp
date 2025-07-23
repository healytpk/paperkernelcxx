#ifndef HEADER_INCLUSION_GUARD_8A2B4C1D_7E3F_4B2A_9C1D_5F6E7A8B9C0D
#define HEADER_INCLUSION_GUARD_8A2B4C1D_7E3F_4B2A_9C1D_5F6E7A8B9C0D

#include <cctype>                   // isalnum, tolower
#include <cstddef>                  // size_t
#include <algorithm>                // find, ranges::sort, ranges::lower_bound, ranges::equal_range
#include <concepts>                 // input_iterator
#include <fstream>                  // ofstream
#include <iterator>                 // cbegin, cend
#include <map>                      // map
#include <ranges>                   // ranges
#include <string>                   // string
#include <string_view>              // string_view
#include <unordered_map>            // unordered_map
#include <utility>                  // move
#include <vector>                   // vector
#include "../main_program/hash.hpp" // Hash_t

class NameManager {
public:
    using size_t = std::size_t;
    using string = std::string;
    using string_view = std::string_view;
    template<typename... Ts> using map = std::map<Ts...>;
    template<typename... Ts> using set = std::set<Ts...>;
    template<typename... Ts> using unordered_map = std::unordered_map<Ts...>;
    template<typename... Ts> using vector = std::vector<Ts...>;

private:
    static constexpr std::pair<unsigned, char> unicode_to_ascii[] = {
        // Lowercase vowels with diacritics
        {0x00E0, 'a'}, // a
        {0x00E1, 'a'}, // a
        {0x00E2, 'a'}, // a
        {0x00E3, 'a'}, // a
        {0x00E4, 'a'}, // a
        {0x00E5, 'a'}, // a
        {0x0101, 'a'}, // a
        {0x0103, 'a'}, // a
        {0x0105, 'a'}, // a

        {0x00E8, 'e'}, // e
        {0x00E9, 'e'}, // e
        {0x00EA, 'e'}, // e
        {0x00EB, 'e'}, // e
        {0x0113, 'e'}, // e
        {0x0115, 'e'}, // e
        {0x0117, 'e'}, // e
        {0x0119, 'e'}, // e
        {0x011B, 'e'}, // e

        {0x00EC, 'i'}, // i
        {0x00ED, 'i'}, // i
        {0x00EE, 'i'}, // i
        {0x00EF, 'i'}, // i
        {0x0129, 'i'}, // i
        {0x012B, 'i'}, // i
        {0x012D, 'i'}, // i
        {0x012F, 'i'}, // i
        {0x0131, 'i'}, // i

        {0x00F2, 'o'}, // o
        {0x00F3, 'o'}, // o
        {0x00F4, 'o'}, // o
        {0x00F5, 'o'}, // o
        {0x00F6, 'o'}, // o
        {0x00F8, 'o'}, // o
        {0x014D, 'o'}, // o
        {0x014F, 'o'}, // o
        {0x0151, 'o'}, // o

        {0x00F9, 'u'}, // u
        {0x00FA, 'u'}, // u
        {0x00FB, 'u'}, // u
        {0x00FC, 'u'}, // u
        {0x0169, 'u'}, // u
        {0x016B, 'u'}, // u
        {0x016D, 'u'}, // u
        {0x016F, 'u'}, // u
        {0x0171, 'u'}, // u
        {0x0173, 'u'}, // u

        {0x00FD, 'y'}, // y
        {0x00FF, 'y'}, // y
        {0x0177, 'y'}, // y

        // Uppercase vowels with diacritics
        {0x00C0, 'A'}, // A
        {0x00C1, 'A'}, // A
        {0x00C2, 'A'}, // A
        {0x00C3, 'A'}, // A
        {0x00C4, 'A'}, // A
        {0x00C5, 'A'}, // A
        {0x0100, 'A'}, // A
        {0x0102, 'A'}, // A
        {0x0104, 'A'}, // A

        {0x00C8, 'E'}, // E
        {0x00C9, 'E'}, // E
        {0x00CA, 'E'}, // E
        {0x00CB, 'E'}, // E
        {0x0112, 'E'}, // E
        {0x0114, 'E'}, // E
        {0x0116, 'E'}, // E
        {0x0118, 'E'}, // E
        {0x011A, 'E'}, // E

        {0x00CC, 'I'}, // I
        {0x00CD, 'I'}, // I
        {0x00CE, 'I'}, // I
        {0x00CF, 'I'}, // I
        {0x0128, 'I'}, // I
        {0x012A, 'I'}, // I
        {0x012C, 'I'}, // I
        {0x012E, 'I'}, // I
        {0x0130, 'I'}, // I

        {0x00D2, 'O'}, // O
        {0x00D3, 'O'}, // O
        {0x00D4, 'O'}, // O
        {0x00D5, 'O'}, // O
        {0x00D6, 'O'}, // O
        {0x00D8, 'O'}, // O
        {0x014C, 'O'}, // O
        {0x014E, 'O'}, // O
        {0x0150, 'O'}, // O

        {0x00D9, 'U'}, // U
        {0x00DA, 'U'}, // U
        {0x00DB, 'U'}, // U
        {0x00DC, 'U'}, // U
        {0x0168, 'U'}, // U
        {0x016A, 'U'}, // U
        {0x016C, 'U'}, // U
        {0x016E, 'U'}, // U
        {0x0170, 'U'}, // U
        {0x0172, 'U'}, // U

        {0x00DD, 'Y'}, // Y
        {0x0176, 'Y'}, // Y
        {0x0178, 'Y'}, // Y

        // Consonants with diacritics
        {0x00E7, 'c'}, // c
        {0x0107, 'c'}, // c
        {0x0109, 'c'}, // c
        {0x010B, 'c'}, // c
        {0x010D, 'c'}, // c

        {0x00C7, 'C'}, // C
        {0x0106, 'C'}, // C
        {0x0108, 'C'}, // C
        {0x010A, 'C'}, // C
        {0x010C, 'C'}, // C

        {0x011F, 'g'}, // g
        {0x0121, 'g'}, // g
        {0x0123, 'g'}, // g
        {0x011E, 'G'}, // G
        {0x0120, 'G'}, // G
        {0x0122, 'G'}, // G

        {0x0144, 'n'}, // n
        {0x0146, 'n'}, // n
        {0x0148, 'n'}, // n
        {0x0149, 'n'}, // n
        {0x00F1, 'n'}, // n
        {0x0143, 'N'}, // N
        {0x0145, 'N'}, // N
        {0x0147, 'N'}, // N
        {0x00D1, 'N'}, // N

        {0x0155, 'r'}, // r
        {0x0157, 'r'}, // r
        {0x0159, 'r'}, // r
        {0x0154, 'R'}, // R
        {0x0156, 'R'}, // R
        {0x0158, 'R'}, // R

        {0x015B, 's'}, // s
        {0x015D, 's'}, // s
        {0x015F, 's'}, // s
        {0x0161, 's'}, // s
        {0x015A, 'S'}, // S
        {0x015C, 'S'}, // S
        {0x015E, 'S'}, // S
        {0x0160, 'S'}, // S

        {0x0163, 't'}, // t
        {0x0165, 't'}, // t
        {0x0162, 'T'}, // T
        {0x0164, 'T'}, // T

        {0x017A, 'z'}, // z
        {0x017C, 'z'}, // z
        {0x017E, 'z'}, // z
        {0x0179, 'Z'}, // Z
        {0x017B, 'Z'}, // Z
        {0x017D, 'Z'}, // Z

        {0x013A, 'l'}, // l
        {0x013C, 'l'}, // l
        {0x013E, 'l'}, // l
        {0x0142, 'l'}, // l
        {0x0139, 'L'}, // L
        {0x013B, 'L'}, // L
        {0x013D, 'L'}, // L
        {0x0141, 'L'}, // L

        // Special cases
        {0x00DF, 's'}, // sharp s
        {0x0153, 'o'}, // oe ligature
        {0x0152, 'O'}, // OE ligature
        {0x0131, 'i'}, // dotless i
        {0x00F8, 'o'}, // o stroke
        {0x00D8, 'O'}, // O stroke
        // Add more as needed
    };

    static void ReplaceUnicodeEscapesWithAscii(std::string &input)
    {
        std::string output;
        for ( size_t i = 0u; i < input.size(); )
        {
            // Handle \uXXXX or \\uXXXX
            if ( input[i] == '\\' && i + 5 < input.size() && input[i+1] == 'u' )
            {
                // \uXXXX
                std::istringstream iss(input.substr(i+2, 4));
                long code = 0;
                iss >> std::hex >> code;
                auto it = std::find_if(std::cbegin(unicode_to_ascii),std::cend(unicode_to_ascii),[code](auto &&arg){ return arg.first == code; });
                if ( it != std::cend(unicode_to_ascii) ) output += it->second;
                else output += '_';
                i += 6;
            }
            else if ( input[i] == '\\' && i + 6 < input.size() && input[i+1] == '\\' && input[i+2] == 'u' )
            {
                // \\uXXXX
                std::istringstream iss(input.substr(i+3, 4));
                long code = 0;
                iss >> std::hex >> code;
                auto it = std::find_if(std::cbegin(unicode_to_ascii),std::cend(unicode_to_ascii),[code](auto &&arg){ return arg.first == code; });
                if ( it != std::cend(unicode_to_ascii) ) output += it->second;
                else output += '_';
                i += 7;
            }
            else
            {
                output += input[i];
                ++i;
            }
        }
        input = std::move(output);
    }

    static constexpr std::string_view hardcoded_primaries[] = {
        "D. S. Hollman",
        "Detlef Vollmann",
        "Hong Hong",
        "Mingxin Wang",
        "Richard Smith",
        "Yihe Li",
        "Yehezkel Bernat",
    };

    inline static std::map<string_view, string_view> const hardcoded_alternatives = {
        { "Alex"                                                   , "Alex Waffl3x"                                 },
        { "Alex (Waffl3x)"                                         , "Alex Waffl3x"                                 },
        { "Arthur O\\u2019Dwyer"                                   , "Arthur O'Dwyer"                               },
        { "Bengt Gustafsonn"                                       , "Bengt Gustafsson"                             },
        { "Billy O'Neal"                                           , "Billy Robert O'Neal III"                      },
        { "Billy O\\u2019Neal"                                     , "Billy Robert O'Neal III"                      },
        { "Carter H. Edwards"                                      , "H. Carter Edwards"                            },
        { "C. Kaeser"                                              , "Christian K\\u00E4ser"                        },
        { "C. K\\u00E4ser"                                         , "Christian K\\u00E4ser"                        },
        { "C. Walker"                                              , "Chris Walker"                                 },
        { "Christian Kaeser"                                       , "Christian K\\u00E4ser"                        },
        { "Daniel Sundernland"                                     , "Daniel Sunderland"                            },
        { "D. Diaz"                                                , "Diego Diaz"                                   },

        { "D.S. Hollman"                                           , "D. S. Hollman"                                },
        { "Daisy Hollman"                                          , "D. S. Hollman"                                },
        { "Daisy S. Hollman"                                       , "D. S. Hollman"                                },
        { "David Hollman"                                          , "D. S. Hollman"                                },
        { "David S. Hollman"                                       , "D. S. Hollman"                                },

        { "D. Vollmann"                                            , "Detlef Vollmann"                              },
        { "Detlef Vollman"                                         , "Detlef Vollmann"                              },
        { "D. Miller"                                              , "David Miller"                                 },
        { "D. Walker"                                              , "Daniel Walker"  /* not David */               },
        { "Ed Catmur"                                              , "Edward Catmur (1982 \\u002D 2024)"            },
        { "H. Br\\uFFFDnnimann"                                    , "Herv\\u00e9 Br\\u00f6nnimann"                 },
        { "Herv\\00e9 Br\\u00f6nnimann" /* 'u' is missing */       , "Herv\\u00e9 Br\\u00f6nnimann"                 },
        { "Herv\\uFFFD Br\\uFFFDnnimann"                           , "Herv\\u00e9 Br\\u00f6nnimann"                 },
        { "Ion Gazta\\U000619E1"                                   , "Ion Gazta\\u00f1aga"                          },
        { "J. J\\u00E4rvi"                                         , "Jaakko J\\u00e4rvi"                           },
        { "J. J\\u4CB6i"                                           , "Jaakko J\\u00e4rvi"                           },
        { "J. Jaarvi"                                              , "Jaakko J\\u00e4rvi"                           },
        { "J. Jarvi"                                               , "Jaakko J\\u00e4rvi"                           },
        { "Johel Guerrero"                                         , "Johel Ernesto Guerrero Pe\\u00F1a"            },
        { "Jonathan Mueller"                                       , "Jonathan M\\u00FCller"                        },
        { "Levi de koning"                                         , "Levi de Koning"                               },
        { "K. Kloepper"                                            , "Kyle Kl\\u00F6pper"                           },
        { "Klemans Morgenstern"                                    , "Klemens Morgenstern"                          },
        { "Kyle Kloepper"                                          , "Kyle Kl\\u00F6pper"                           },
        { "L. Joly"                                                , "Lo\\u00efc Joly"                              },
        { "Lo\\uF8E0Joly"                                          , "Lo\\u00efc Joly"                              },
        { "Loic Joly"                                              , "Lo\\u00efc Joly"                              },
        { "Lo\\uFFFDc Joly"                                        , "Lo\\u00efc Joly"                              },
        { "Lois Goldwaithe"                                        , "Lois Goldthwaite"                             },
        { "M. Diaz"                                                , "Marcos Diaz"                                  },
        { "Marhsall Clow"                                          , "Marshall Clow"                                },
        { "Mingo Gill"                                             , "Mungo Gill"                                   },
        { "P. Talbot"                                              , "Pierre Talbot"                                },
        { "R. Rivera"                                              , "Ren\\u00E9 Ferdinand Rivera Morell"           },
        { "Ren\\u00E9 Rivera"                                      , "Ren\\u00E9 Ferdinand Rivera Morell"           },
        { "Rene Riviera"                                           , "Ren\\u00E9 Ferdinand Rivera Morell"           },
        { "Rene Rivera"                                            , "Ren\\u00E9 Ferdinand Rivera Morell"           },
        { "Robert Klarer 2001"                                     , "Robert Klarer"                                },
        { "Thomas Koeppe"                                          , "Thomas K\\u00F6ppe"                           },
        { "V.  Escriba"                                            , "Vicente J. Botet Escrib\\u00E1"               },
        { "V. Escriba"                                             , "Vicente J. Botet Escrib\\u00E1"               },
        { "Yehezkel Bernart"                                       , "Yehezkel Bernat"                              },
        { "Yoaodan Zhang"                                          , "Yaodan Zhang"                                 },
        { "Yihe Le"                                                , "Yihe Li"                                      },
    };

    vector<string     >         m_name_storage;
    vector<string_view>         m_names;
    vector<size_t     > mutable m_parent;

    map<string_view, string_view> mutable m_map_clusters;
    bool mutable m_map_clusters_dirty = true;

    static constexpr char wildchar = '_';

    static void ReplaceNonAscii(std::string &input)
    {
        std::string output;
        for ( size_t i = 0u; i < input.size(); )
        {
            // Look for \uXXXX or \\uXXXX
            if ( (input[i] == '\\') && (i + 5 < input.size()) && (input[i + 1] == 'u') )
            {
                // Replace \uXXXX with wildchar
                output += wildchar;
                i += 6;
            }
            else if ( (input[i] == '\\') && (i + 6 < input.size()) && (input[i + 1] == '\\') && (input[i + 2] == 'u'))
            {
                // Replace \\uXXXX with wildchar
                output += wildchar;
                i += 7;
            }
            else if ( static_cast<unsigned char>(input[i]) > 0x7F )
            {
                std::abort();
                // Replace any other non-ASCII character with wildchar
                output += wildchar;
                ++i;
            }
            else
            {
                output += input[i];
                ++i;
            }
        }
        input = std::move(output);
    }

    static void ToLowerAndRemovePunct(string &s)
    {
        string result;
        for ( char const c : s )
        {
            if ( std::isalnum(static_cast<unsigned char>(c)) || (c == ' ') || (c == '\\') )
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
        constexpr auto IsHardcodedPrimary = [](string_view const x) -> bool
          {
            return std::cend(hardcoded_primaries) != std::find(std::cbegin(hardcoded_primaries), std::cend(hardcoded_primaries), x);
          };

        if ( IsHardcodedPrimary(a) || IsHardcodedPrimary(b) )  return a == b;

        string sa(a), sb(b);
        ToLowerAndRemovePunct(sa);
        ToLowerAndRemovePunct(sb);

        ReplaceUnicodeEscapesWithAscii(sa);
        ReplaceUnicodeEscapesWithAscii(sb);

        ReplaceNonAscii(sa);
        ReplaceNonAscii(sb);

        // The next line mostly accommodates Spanish names,
        // where the second surname is sometimes ommitted.
        if ( sa.starts_with(sb) || sb.starts_with(sa) ) return true;

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

    static bool HasLowercaseParticle(vector<string> const &tokens)
    {
        static constexpr std::string_view particles[] = { "van", "de", "von", "der", "den", "ter", "ten", "op", "af", "di", "da", "du" };
        // Check all but the first and last token (usually the surname is last)
        for ( size_t i = 1; i + 1 < tokens.size(); ++i )
        {
            for ( auto const &p : particles )
            {
                if ( tokens[i] == p ) return true;
            }
        }
        return false;
    }

    void SortClusterIndices(vector<size_t>& cluster_indices) const
    {
        std::ranges::sort(cluster_indices,
            [this](size_t a, size_t b)
            {
                auto const tokensA = SplitTokens(m_names[a]);
                auto const tokensB = SplitTokens(m_names[b]);
                // Prefer names with lowercase particles
                bool a_has = HasLowercaseParticle(tokensA);
                bool b_has = HasLowercaseParticle(tokensB);
                if (a_has != b_has) return a_has; // true sorts before false
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
        for ( auto it = it_begin; it != it_end; ++it ) if ( hardcoded_alternatives.find(*it) == hardcoded_alternatives.end() ) new_names.emplace_back(*it);

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
        if ( std::cend(hardcoded_primaries) != std::find( std::cbegin(hardcoded_primaries), std::cend(hardcoded_primaries), alt ) ) return alt;

        // Check hardcoded exceptions first
        auto it = hardcoded_alternatives.find(alt);
        if ( it != hardcoded_alternatives.end() ) return it->second;

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
        for ( auto const & [alt, primary] : hardcoded_alternatives ) m_map_clusters[alt] = primary;

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
        for ( auto const & [alt, primary] : hardcoded_alternatives ) result[primary].push_back(alt);
        return result;
    }

    static constexpr string escape_for_hash(string_view const s)
    {
        string out;
        out.reserve( s.size() );
        for ( char const c : s )
        {
            out += c;
            if ( '\\' == c ) out += c;
        }
        return out;
    }

    void WriteHeaders(char const *const output_file) const
    {
        constexpr auto pad_spaces = [](size_t const s, size_t const width = pad_len) -> string
          {
            return (s < width) ? string(width - s, ' ') : string();
          };

        auto const groups = this->GetPrimaryToAlternativesMap();

        // --- Minimal fix: Build set of all alternatives (including exceptions) ---
        set<string> all_alternatives;
        for (const auto& [primary, alts] : groups)
            for (const auto& alt : alts)
                all_alternatives.insert(string(alt));

        for (const auto& [alt, primary] : hardcoded_alternatives)
            all_alternatives.insert(string(alt));
        // ------------------------------------------------------------------------

        set<string> all_primaries;
        for ( auto const & [primary, alts] : groups )
            if (all_alternatives.find(string(primary)) == all_alternatives.end())
                all_primaries.insert( string(primary) );

        for ( auto it = m_names.cbegin(); it != m_names.cend(); ++it )
        {
            auto const &name = *it;
            auto const p = this->GetPrimaryName(name);
            if ( (p == name) && (groups.find(p) == groups.end()) && (all_alternatives.find(string(p)) == all_alternatives.end()) )
                all_primaries.insert( string(p) );
        }

        // Ensure all hardcoded primaries are present
        for ( auto const sv : hardcoded_primaries ) all_primaries.insert( string(sv) );

        struct PrimaryEntry {
            string name;
            Hash_t hash;
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
            Hash_t alt_hash;
            Hash_t primary_hash;
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
        out << "#include <cstddef>            // size_t\n";
        out << "#include <iterator>           // cbegin, cend\n";
        out << "#include <tuple>              // tuple\n";
        out << "#include <utility>            // pair\n";
        out << "#include <wx/string.h>        // wxS, wxStringCharType\n";
        out << "#include \"hash.hpp\"           // Hash, Hash_t\n\n";

        // Primary names sorted alphabetically (not by hash)
        out << "inline constexpr std::pair< Hash_t, wxStringCharType const * > g_primary_names_unsorted[] = {\n";
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
        out << "inline constexpr std::tuple< Hash_t, wxStringCharType const*, Hash_t > g_alternative_names_unsorted[] = {\n";
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

        out << "inline constexpr std::size_t g_primary_names_unsortedSize     = std::cend(g_primary_names_unsorted    ) - std::cbegin(g_primary_names_unsorted    );\n";
        out << "inline constexpr std::size_t g_alternative_names_unsortedSize = std::cend(g_alternative_names_unsorted) - std::cbegin(g_alternative_names_unsorted);\n";
        out << std::endl;
    }
};

#endif    // ifndef HEADER_INCLUSION_GUARD_8A2B4C1D_7E3F_4B2A_9C1D_5F6E7A8B9C0D

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <string_view>
#include <cctype>
#include <algorithm>
#include <unordered_map>
#include <cstdint>
#include <iomanip>  // for setw, setfill
#include "common.hpp"

using std::string;
using std::string_view;
using std::vector;
using std::cin;
using std::cout;
using std::endl;

std::ofstream logfile;

// -- Helper functions --

static void ToLowerAndRemovePunct(string& s)
{
    string result;
    for (char c : s)
    {
        if (std::isalnum(static_cast<unsigned char>(c)) || c == ' ')
            result += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        else if (c == '.' || c == '-')
            result += ' ';
    }
    s = std::move(result);
}

static vector<string> SplitTokens(string_view s)
{
    vector<string> tokens;
    size_t pos = 0;
    while (pos < s.size())
    {
        while (pos < s.size() && s[pos] == ' ')
            ++pos;
        size_t start = pos;
        while (pos < s.size() && s[pos] != ' ')
            ++pos;
        if (start < pos)
            tokens.emplace_back(s.substr(start, pos - start));
    }
    return tokens;
}

static string GetSurname(const vector<string>& tokens)
{
    if (tokens.empty())
        return "";
    return tokens.back();
}

static vector<string> GetGivenNames(const vector<string>& tokens)
{
    if (tokens.size() <= 1)
        return {};
    return vector<string>(tokens.begin(), tokens.end() - 1);
}

static string ExtractInitials(const vector<string>& tokens)
{
    string initials;
    for (auto const& t : tokens)
    {
        if (!t.empty())
            initials += t[0];
    }
    return initials;
}

static bool IsSubsequence(string_view shorter, string_view longer)
{
    size_t i = 0, j = 0;
    while (i < shorter.size() && j < longer.size())
    {
        if (shorter[i] == longer[j])
            ++i;
        ++j;
    }
    return i == shorter.size();
}

bool IsSamePerson(string_view a, string_view b)
{
    string sa{a}, sb{b};
    ToLowerAndRemovePunct(sa);
    ToLowerAndRemovePunct(sb);

    auto tokensA = SplitTokens(sa);
    auto tokensB = SplitTokens(sb);

    if (tokensA.empty() || tokensB.empty())
        return false;

    auto surnameA = GetSurname(tokensA);
    auto surnameB = GetSurname(tokensB);

    if (surnameA != surnameB)
        return false;

    auto givenA = GetGivenNames(tokensA);
    auto givenB = GetGivenNames(tokensB);

    string initialsA = ExtractInitials(givenA);
    string initialsB = ExtractInitials(givenB);

    return IsSubsequence(initialsA, initialsB) || IsSubsequence(initialsB, initialsA);
}

// ----------- Main program ------------

int main()
{
    std::cerr << "Attempting to create 'log.txt' in current directory. . .\n";
    logfile.open("log.txt");
    if (!logfile)
    {
        std::cerr << "Failed to create file 'logfile.txt' in current directory\n";
        return EXIT_FAILURE;
    }
    logfile << "First line in log file\n";

    vector<string> names;
    string line;
    while (std::getline(cin, line))
    {
        if (!line.empty())
            names.push_back(line);
    }

    // Union-Find (Disjoint Set) structure to cluster duplicates
    vector<size_t> parent(names.size());
    for (size_t i = 0; i < parent.size(); ++i)
        parent[i] = i;

    auto find = [&](size_t x) {
        while (parent[x] != x)
            x = parent[x] = parent[parent[x]];
        return x;
    };

    auto unite = [&](size_t a, size_t b) {
        size_t pa = find(a);
        size_t pb = find(b);
        if (pa != pb)
            parent[pa] = pb;
    };

    // Build clusters of duplicates
    for (size_t i = 0; i < names.size(); ++i)
    {
        for (size_t j = i + 1; j < names.size(); ++j)
        {
            if (IsSamePerson(names[i], names[j]))
                unite(i, j);
        }
    }

    // Map root -> cluster indices
    std::unordered_map<size_t, vector<size_t>> clusters;
    for (size_t i = 0; i < names.size(); ++i)
        clusters[find(i)].push_back(i);

    // Structures for output
    struct Author_Primary
    {
        uint_fast64_t hash;
        string name;
    };

    struct AlternativeAuthor
    {
        uint_fast64_t hash;
        string name;
        uint_fast64_t primary;
    };

    vector<Author_Primary> primaryAuthors;
    vector<AlternativeAuthor> alternativeAuthors;

    for (auto const& [root, indices] : clusters)
    {
        if (indices.size() < 2)
            continue; // skip singletons

        // Extract cluster names
        vector<string_view> clusterNames;
        for (auto idx : indices)
            clusterNames.emplace_back(names[idx]);

        // Sort by length descending
        std::sort(clusterNames.begin(), clusterNames.end(),
                  [](string_view a, string_view b) { return a.size() > b.size(); });

        string const primaryName = string(clusterNames.front());
        uint_fast64_t primaryHash = Hash(primaryName);

        primaryAuthors.push_back({primaryHash, primaryName});

        for (size_t i = 1; i < clusterNames.size(); ++i)
        {
            string altName(clusterNames[i]);
            uint_fast64_t altHash = Hash(altName);
            alternativeAuthors.push_back({altHash, altName, primaryHash});
        }
    }

    // Sort primaryAuthors by hash ascending
    std::sort(primaryAuthors.begin(), primaryAuthors.end(),
              [](const Author_Primary& a, const Author_Primary& b) { return a.hash < b.hash; });

    // Sort alternativeAuthors by hash ascending
    std::sort(alternativeAuthors.begin(), alternativeAuthors.end(),
              [](const AlternativeAuthor& a, const AlternativeAuthor& b) { return a.hash < b.hash; });

    // Output the C++ header content
    cout << "#pragma once\n\n";
    cout << "#include <cstdint>         // uint_fast64_t\n"
            "#include <algorithm>       // lower_bound\n"
            "#include <string_view>     // string_view\n\n";

    cout << "namespace Author_details {\n\n";

    cout << "struct Author_Primary {\n"
         << "    std::uint_fast64_t hash;\n"
         << "    char const *name;\n"
         << "};\n\n";

    cout << "inline constexpr Author_Primary names_primary[] = {\n";
    for (size_t i = 0; i < primaryAuthors.size(); ++i)
    {
        cout << "    { 0x" << std::hex << std::setfill('0') << std::setw(16)
             << primaryAuthors[i].hash << std::dec << ", "
             << "\"" << primaryAuthors[i].name << "\" }";
        if (i + 1 != primaryAuthors.size())
            cout << ",";
        cout << "\n";
    }
    cout << "};\n\n";

    cout << "struct Author_Alternative {\n"
         << "    std::uint_fast64_t hash;\n"
         << "    char const *name;\n"
         << "    std::uint_fast64_t primary;\n"
         << "};\n\n";

    cout << "inline constexpr Author_Alternative names_alternative[] = {\n";
    for (size_t i = 0; i < alternativeAuthors.size(); ++i)
    {
        // Find primary name string for comment
        const string* primaryNamePtr = nullptr;
        for (const auto& p : primaryAuthors)
        {
            if (p.hash == alternativeAuthors[i].primary)
            {
                primaryNamePtr = &p.name;
                break;
            }
        }

        cout << "    { 0x" << std::hex << std::setfill('0') << std::setw(16)
             << alternativeAuthors[i].hash << std::dec << ", "
             << "\"" << alternativeAuthors[i].name << "\", "
             << "0x" << std::hex << std::setfill('0') << std::setw(16)
             << alternativeAuthors[i].primary << std::dec << " }";

        if (primaryNamePtr)
            cout << ", // primary = \"" << *primaryNamePtr << "\"";

        cout << "\n";
    }
    cout << "};\n\n";

    // Write helper and hash functions inside an anonymous namespace
    cout <<
R"(
inline constexpr char tolower_consteval(char c) noexcept
{
    switch (c)
    {
        case 'A': return 'a'; case 'B': return 'b'; case 'C': return 'c'; case 'D': return 'd';
        case 'E': return 'e'; case 'F': return 'f'; case 'G': return 'g'; case 'H': return 'h';
        case 'I': return 'i'; case 'J': return 'j'; case 'K': return 'k'; case 'L': return 'l';
        case 'M': return 'm'; case 'N': return 'n'; case 'O': return 'o'; case 'P': return 'p';
        case 'Q': return 'q'; case 'R': return 'r'; case 'S': return 's'; case 'T': return 't';
        case 'U': return 'u'; case 'V': return 'v'; case 'W': return 'w'; case 'X': return 'x';
        case 'Y': return 'y'; case 'Z': return 'z';
        default:  return c;
    }
}

inline constexpr char letters[] = "abcdefghijklmnopqrstuvwxyz";
)";

    cout <<
R"(
inline constexpr std::uint_fast64_t Hash(std::string_view const input) noexcept
{
    std::uint_fast64_t h = 0xcbf29ce484222325;

    for ( char const cX : input )
    {
        char const c = tolower_consteval(cX);
        bool is_letter = false;

        for ( char letter : letters )
        {
            if ( letter == c )
            {
                is_letter = true;
                break;
            }
        }

        if ( false == is_letter ) continue;

        h ^= static_cast<std::uint_fast64_t>(c);
        h *= 0x100000001B3;
    }

    return h;
}
)";

    cout << "\n} // namespace Author_details\n";

    cout <<
R"(
inline constexpr const Author_details::Author_Primary *FindAuthor(char const *const p) noexcept
{
    using namespace Author_details;

    constexpr size_t primary_count = sizeof(names_primary) / sizeof(names_primary[0]);
    constexpr size_t alternative_count = sizeof(names_alternative) / sizeof(names_alternative[0]);

    const std::uint_fast64_t h = Hash(p);

    // Search alternatives with lower_bound
    auto altIt = std::lower_bound(
        names_alternative, names_alternative + alternative_count, h,
        [](const Author_Alternative& a, std::uint_fast64_t value) {
            return a.hash < value;
        });

    if (altIt != names_alternative + alternative_count && altIt->hash == h)
    {
        // Search primary with lower_bound
        auto primIt = std::lower_bound(
            names_primary, names_primary + primary_count, altIt->primary,
            [](const Author_Primary& a, std::uint_fast64_t value) {
                return a.hash < value;
            });

        if (primIt != names_primary + primary_count && primIt->hash == altIt->primary)
            return primIt;

        return nullptr;
    }

    // Search primary with lower_bound
    auto primIt = std::lower_bound(
        names_primary, names_primary + primary_count, h,
        [](const Author_Primary& a, std::uint_fast64_t value) {
            return a.hash < value;
        });

    if (primIt != names_primary + primary_count && primIt->hash == h)
        return primIt;

    return nullptr;
}
)";

    return 0;
}

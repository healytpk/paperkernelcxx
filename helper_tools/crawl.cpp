#include <cassert>
#include <cstring>                           // strchr, strcmp
#include <charconv>                          // from_chars
#include <algorithm>                         // replace
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <string_view>
#include <vector>
#include <set>
#include <utility>
#include <nlohmann/json.hpp>
#include "common.hpp"
#include "../main_program/hash.hpp"
#include "../main_program/paper.hpp"
#include "NameManager.hpp"

using std::size_t, std::string, std::string_view, std::cout, std::cerr, std::endl;
using json = nlohmann::json;

std::ofstream logfile;

std::map< string, std::vector<Paper> > g_names;

std::map< Paper, std::set<string> > ReverseAuthorPaperMap( std::map< string, std::vector<Paper> > const &arg )
{
    std::map< Paper, std::set<string> > retval;

    for ( auto const &[author, vec_papers] : arg )
    {
        for ( Paper const ppr : vec_papers )
        {
            retval[ppr].insert(author);
        }
    }

    return retval;
}

std::map< string, std::map<Paper, std::pair<string /* author */, string /* title */> > > papers;

unsigned GetPaperLatestRev( std::map< Paper, std::pair<string /* author */, string /* title */> >  const &arg)
{
    unsigned retval = 0u;
    for ( size_t i = 0u; i < arg.size(); ++i )
    {
        std::pair< Paper, std::pair<string /* author */, string /* title */> > const &mypair = *std::next(std::cbegin(arg), i);
        retval = std::max(retval, mypair.first.rev);
    }
    return retval;
}

void escape_for_hash(std::string &s)
{
    std::string out;
    out.reserve(s.size());
    for ( char const c : s )
    {
        if ( c == '\\' ) out += "\\\\";
        else out += c;
    }
    s = std::move(out);
}

bool IsNotName(string_view const s) noexcept
{
    static constexpr char const *not_names[] = {
        "dkuhl@bloomberg.net",
        "et al.",
        "thomas.rodgers@woven-planet.global",
        "various SG14 contibutors including Nicolas Fleury (Ubisoft)",
        "Add container pop methods that return the popped value",
        "Woven By Toyota",
        "Bloomberg",
        "others",
        "C.S.",
    };

    for ( char const *const p : not_names )
    {
        if ( s == p ) return true;
    }

    return false;
}

bool ContainsNonASCII(string_view const s) noexcept
{
    for ( char unsigned const c : s )
    {
        if ( c >= 0x7f ) return true;
    }
    return false;
}

void ProcessAuthorSquareFromTable(string author, string_view const doc)
{
    ReplaceHtmlNumericEscapesInPlace(author);

// ============================ Bugs on ISO webpages
    ReplaceInPlace(author, "Alicia KlinvexLi-Ta Lo"                                 , "Alicia Klinvex\nLi-Ta Lo"                               );
    ReplaceInPlace(author, "Berg&eacute "                                           , "Berg&eacute;"                                           );
    ReplaceInPlace(author, "D. AbrahamsJ. SiekT. Witt"                              , "D. Abrahams\nJ. Siek\nT. Witt"                          );
    ReplaceInPlace(author, "D. GregorG. PowellJ. J\u00E4rvi"                        , "D. Gregor\nG. Powell\nJ. J\\u00E4rvi"                   );
    ReplaceInPlace(author, "D. GregorP. Dimov"                                      , "D. Gregor\nP. Dimov"                                    );
    ReplaceInPlace(author, "G. Dos ReisM. Marcus"                                   , "G. Dos Reis\nM. Marcus"                                 );
    ReplaceInPlace(author, "H. Hinnant P. Dimov D. Abrahams"                        , "H. Hinnant\nP. Dimov\nD. Abrahams"                      );
    ReplaceInPlace(author, "H. Sutter T. Plum"                                      , "H. Sutter\nT. Plum"                                     );
    ReplaceInPlace(author, "J. J\\u00E4rviB. StroustrupD. GregorJ. Siek"            , "J. J\\u00E4rvi\nB. Stroustrup\nD. Gregor\nJ. Siek"      );
    ReplaceInPlace(author, "J. J\u00E4rviB. StroustrupD. GregorJ. Siek"             , "J. J\\u00E4rvi\nB. Stroustrup\nD. Gregor\nJ. Siek"      );
    ReplaceInPlace(author, "J. Spicer J. Wiegley"                                   , "J. Spicer\nJ. Wiegley"                                  );
    ReplaceInPlace(author, "Jared Hoberock Lee Howes"                               , "Jared Hoberock\nLee Howes"                              );
    ReplaceInPlace(author, "M. MarcusG. Dos Reis"                                   , "M. Marcus\nG. Dos Reis"                                 );
    ReplaceInPlace(author, "P. DimovB. DawesG. Colvin"                              , "P. Dimov\nB. Dawes\nG. Colvin"                          );
    ReplaceInPlace(author, "P. DimovD. GregorJ. J\\u00E4rviG. Powell"               , "P. Dimov\nD. Gregor\nJ. J\\u00E4rvi\nG. Powell"         );
    ReplaceInPlace(author, "P. DimovD. GregorJ. J\u00E4rviG. Powell"                , "P. Dimov\nD. Gregor\nJ. J\\u00E4rvi\nG. Powell"         );
    ReplaceInPlace(author, "P. Dimov D. Gregor J. J\\u00E4rvi G. Powell"            , "P. Dimov\nD. Gregor\nJ. J\\u00E4rvi\nG. Powell"         );
    ReplaceInPlace(author, "P. Dimov D. Gregor J. J\u00E4rvi G. Powell"             , "P. Dimov\nD. Gregor\nJ. J\\u00E4rvi\nG. Powell"         );
    ReplaceInPlace(author, "P. Dimov B. Dawes G. Colvin"                            , "P. Dimov\nB. Dawes\nG. Colvin"                          );
    ReplaceInPlace(author, "Pablo Halpern Ville Voutilainen"                        , "Pablo Halpern\nVille Voutilainen"                       );
    ReplaceInPlace(author, "R. Klarer J. Maddock"                                   , "R. Klarer\nJ. Maddock"                                  );
    ReplaceInPlace(author, "R.W. Grosse-Kunstleve & D. Abrahams"                    , "Ralf W. Grosse-Kunstleve\nD. Abrahams"                  );
    ReplaceInPlace(author, "Walter Brown & Marc Paterno"                            , "Walter Brown\nMarc Paterno"                             );
// ===================================================

    ReplaceInPlace(author,  "\u00A0", " ");  // Non-breaking space (just one backslash)
    ReplaceInPlace(author, "\\u2019", "'");  // Strange apostrophe (two backslashes)
    ReplaceInPlace(author, "`", "'");
    ReplaceInPlace(author, "&aacute;", "\u00E1");
    ReplaceInPlace(author, "&eacute;", "\u00E9");
    ReplaceInPlace(author, "&Eacute;", "\u00C9");
    ReplaceInPlace(author, "&iacute;", "\u00ED");
    ReplaceInPlace(author, "&oacute;", "\u00F3");
    ReplaceInPlace(author, "&uacute;", "\u00FA");
    ReplaceInPlace(author, "&auml;"  , "\u00E4");
    ReplaceInPlace(author, "&euml;"  , "\u00EB");
    ReplaceInPlace(author, "&iuml;"  , "\u00EF");
    ReplaceInPlace(author, "&ouml;"  , "\u00F6");
    ReplaceInPlace(author, "&uuml;"  , "\u00FC");
    ReplaceInPlace(author, "&ordf;"  , "\u00AA");
    ReplaceInPlace(author, "&ntilde;", "\u00F1");
    ReplaceInPlace(author, "&nbsp;"  , " "     );
    ReplaceInPlace(author, "&amp;"   , "\n"    );
    ReplaceInPlace(author, ";"       , "\n"    );
    ReplaceInPlace(author, " and "   , "\n"    );
    ReplaceInPlace(author, " / "     , "\n"    );
    ReplaceInPlace(author, ","       , "\n"    );
    TrimWhitespace(author);

    std::vector<string_view> authors = SplitByNewLines(author);
    for ( string_view const sv : authors )
    {
        string s(sv);
        TrimWhitespace(s);
        if ( IsNotName(s) ) continue;
        EscapeNonAscii(s, true);
        ReplaceInPlace(s, "a\\u0301", "\\u00E1");  // á
        ReplaceInPlace(s, "e\\u0301", "\\u00E9");  // é
        ReplaceInPlace(s, "i\\u0301", "\\u00ED");  // í
        ReplaceInPlace(s, "o\\u0301", "\\u00F3");  // ó
        ReplaceInPlace(s, "u\\u0301", "\\u00FA");  // ú
        ReplaceInPlace(s, "y\\u0301", "\\u00FD");  // ý
        ReplaceInPlace(s, "n\\u0301", "\\u0144");  // n
        ReplaceInPlace(s, "c\\u0301", "\\u0107");  // c
        ReplaceInPlace(s, "l\\u0301", "\\u013A");  // l
        ReplaceInPlace(s, "r\\u0301", "\\u0155");  // r
        ReplaceInPlace(s, "s\\u0301", "\\u015B");  // s
        ReplaceInPlace(s, "z\\u0301", "\\u017A");  // z
        ReplaceInPlace(s, "A\\u0301", "\\u00C1");  // Á
        ReplaceInPlace(s, "E\\u0301", "\\u00C9");  // É
        ReplaceInPlace(s, "I\\u0301", "\\u00CD");  // Í
        ReplaceInPlace(s, "O\\u0301", "\\u00D3");  // Ó
        ReplaceInPlace(s, "U\\u0301", "\\u00DA");  // Ú
        ReplaceInPlace(s, "Y\\u0301", "\\u00DD");  // Ý
        ReplaceInPlace(s, "N\\u0301", "\\u0143");  // N
        ReplaceInPlace(s, "C\\u0301", "\\u0106");  // C
        ReplaceInPlace(s, "L\\u0301", "\\u0139");  // L
        ReplaceInPlace(s, "R\\u0301", "\\u0154");  // R
        ReplaceInPlace(s, "S\\u0301", "\\u015A");  // S
        ReplaceInPlace(s, "Z\\u0301", "\\u0179");  // Z
        ReplaceInPlace(s, "Joaqu\\uDBA0M\\u00AA L\\U000F097A Mu\\u00F1oz", "Joaqu\\u00EDn M\\u00AA L\\u00F3pez Mu\\u00F1oz");
        ReplaceInPlace(s, "Mat\\u00FA\\u0161 Chochl\\u0131\\u0301k", "Mat\\u00FA\\u0161 Chochl\\u00EDk");
        ReplaceInPlace(s, "Mat\\u00FAs\\u030C Chochl\\u00EDk"      , "Mat\\u00FA\\u0161 Chochl\\u00EDk");
        ReplaceInPlace(s, "R. \"Tim\" Song", "R. Tim Song");
        ReplaceInPlace(s, "_ukasz Mendakiewicz", "Lukasz Mendakiewicz");
        ReplaceInPlace(s, "Tomasz Kami_ski", "Tomasz Kami\\u0144ski");
        ReplaceInPlace(s, "H. Br\\U001AEBA9mann"       , "Herv\\00e9 Br\\u00f6nnimann");
        ReplaceInPlace(s, "Herv\\u9802r\\U001AEBA9mann", "Herv\\00e9 Br\\u00f6nnimann");
        ReplaceInPlace(s, "Joseph S. Berr\\uDBF3", "Joseph S. Berr\\u00edos");
        Erase(s, "(h2 AT fsfe.org)");
        Erase(s, " et al.");
        Erase(s, " (EIDOS)");
        TrimWhitespace(s);
        if ( !s.empty() && ('.' == s.back()) ) s.pop_back();
        if ( s.empty() ) continue;
        if ( ContainsNonASCII(s) )
        {
            cout << "----- " << doc << " --- " << s << " --- ";
            for ( char const c : s )
            {
                cout << std::hex << std::setfill('0') << std::setw(2u) << (unsigned)(char unsigned)c << " ";
            }
            cout << endl;
            std::abort();
        }
        if ( s.empty() ) continue;
        g_names[s].emplace_back( Paper{doc} );
    }
}

void MonkeyString(string &s)
{
    string retval;

    for ( char const c : s )
    {
        if ( std::strchr( "abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ", c ) ) retval +=c;
    }

    s = std::move(retval);
}

void MergeAlternativeNames(std::map< std::string, std::vector<Paper> > &arg_names)
{
    NameManager nm;

    // 1. Add all names to NameManager
    std::vector<std::string> all_names;
    for ( const auto& [name, _] : arg_names ) all_names.push_back(name);
    nm.AddNames( all_names.begin(), all_names.end() );

    // 2. Collect alternative names and their primaries
    std::vector< std::pair<std::string, std::string> > to_merge; // {alt, primary}
    for ( const auto& [name, _] : arg_names )
    {
        auto primary = std::string(nm.GetPrimaryName(name));
        if (primary != name) to_merge.emplace_back(name, primary);
    }

    // 3. Merge and erase
    for ( const auto& [alt, primary] : to_merge )
    {
        auto alt_it = arg_names.find(alt);
        if (alt_it == arg_names.end()) continue;

        auto& alt_vec = alt_it->second;
        auto prim_it = arg_names.find(primary);
        if ( prim_it == arg_names.end() )
        {
            arg_names[primary] = std::move(alt_vec);
        }
        else
        {
            auto& prim_vec = prim_it->second;
            prim_vec.insert(prim_vec.end(),
                            std::make_move_iterator(alt_vec.begin()),
                            std::make_move_iterator(alt_vec.end()));
        }
        arg_names.erase(alt_it);
    }
}

int main(void)
{
    std::cerr << "Attempting to create 'log.txt' in current directory. . .\n";
    logfile.open("log.txt");
    if ( false == logfile.is_open() )
    {
        std::cerr << "Failed to create file 'logfile.txt' in current directory\n";
        return EXIT_FAILURE;
    }
    logfile << "First line in log file\n";

    // Read and parse index.json
    std::ifstream json_file("index.json");
    if ( !json_file.is_open() )
    {
        std::cerr << "Failed to open index.json\n";
        return EXIT_FAILURE;
    }
    json j;
    json_file >> j;

    // Populate names and papers from JSON using ProcessAuthorSquareFromTable
    for ( auto it = j.begin(); it != j.end(); ++it )
    {
        auto const &entry = *it;
        string paper_id = it.key();
        if ( paper_id.empty() ) continue;
        string type = entry.value("type", "");
        if ( "paper" != type ) continue;
        switch ( std::tolower(paper_id[0]) )
        {
        case 'd':
            continue;
        case 'n':
        case 'p':
            break;
        case 's':
            if ( paper_id == "standard" ) continue;
        default:
            std::cout << paper_id << std::endl;
            std::abort();
        }
        if ( "P04116R0" == paper_id ) continue;
        if ( "P1343" == paper_id ) paper_id += "r0";
        if ( "P3141" == paper_id ) paper_id += "r0";
        std::string author = entry.value("author", "");
        std::string title = entry.value("title", "");
        if ( author.empty() || title.empty() ) continue;
        // Use the same logic as the HTML path: process author string for names
        ProcessAuthorSquareFromTable(author, paper_id);
        Paper ppr(paper_id);
        papers[ppr.PaperNameWithoutRevision()][ppr] = {author, title};
    }

    std::vector<string> names2;
    for ( auto const &mypair : g_names ) names2.emplace_back( mypair.first );

    NameManager nm;
    nm.AddNames( names2.cbegin(), names2.cend() );
    nm.WriteHeaders( "../main_program/AUTO_GENERATED_names.hpp" );

    MergeAlternativeNames(g_names);

    std::cerr << "==================== " << g_names.size() << " unique names ==================\n";

    std::ofstream fnames_papers("../main_program/AUTO_GENERATED_tree_contents_author.hpp");
    if ( fnames_papers.is_open() )
    {
        fnames_papers << "{\n";
        for ( std::size_t i = 0u; i < g_names.size(); ++i )
        {
            auto const &e = *std::next(std::cbegin(g_names), i);
            string escaped = e.first;
            escape_for_hash(escaped);
            fnames_papers << "    { Hash(\"" << escaped << "\"), PaperList< ";
            bool already_got_one = false;
            for ( auto const &e2 : e.second )
            {
                if ( already_got_one ) fnames_papers << ", ";
                already_got_one = true;
                fnames_papers << "\"" << e2 << "\"";
            }
            fnames_papers << " >() },\n";
        }
    }
    fnames_papers << "};\n";

    std::map< Paper, std::set<string> > paper_authors = ReverseAuthorPaperMap(g_names);

    std::ofstream fpapers("../main_program/AUTO_GENERATED_tree_contents_paper.hpp");
    if ( fpapers.is_open() )
    {
        fpapers << "{\n";
        for ( std::size_t i = 0u; i < papers.size(); ++i )
        {
            std::pair< string, std::map<Paper, std::pair<string /* author */, string /* title */> > > const &e = *std::next(std::cbegin(papers), i);
            std::map<Paper, std::pair<string /* author */, string /* title */> > const &mymap = e.second;
            unsigned const latest_revision = GetPaperLatestRev(mymap);
            fpapers << "    { \"" << e.first;
            if ( !e.first.empty() && 'p' == e.first[0] ) fpapers << "r0";
            fpapers << "\", " << GetPaperLatestRev(mymap) << "u, RevList< ";

            bool already_got_one = false;
            for ( std::size_t j = 0u; j < mymap.size(); ++j )
            {
                std::pair<Paper, std::pair<string /* author */, string /* title */> > const &mypair = *std::next(std::cbegin(mymap), j);
                if ( already_got_one ) fpapers << ", ";
                already_got_one = true;
                fpapers << "Rev< " << mypair.first.rev << "u, ArrHash<";

                std::set<string> const &set_names = paper_authors.at(mypair.first);
                for ( std::size_t k = 0u; k < set_names.size(); ++k )
                {
                    string const &x = *std::next(std::cbegin(set_names), k);
                    fpapers << " Hash(\"" << x << "\")";
                    if ( k != (set_names.size()-1u) ) fpapers << ",";
                }

                fpapers << " >(), wxS(\"";
                string title = mypair.second.second;
                MonkeyString(title);
                fpapers << title << "\") >";
            }
            fpapers << " >() },\n";
        }
        // Don't uncomment the next line -- don't put a null terminator at the end of the papers array
        // fpapers << "    { Paper::Terminator(), RevList< Rev< 0u, Arr64< 0u >(), wxS(\"\") > >() },\n";
        fpapers << "};\n";
    }

    std::ofstream fpaperlist("./complete_paper_list.txt");
    if ( fpaperlist.is_open() )
    {
        for ( auto const &mymap : papers )
        {
            for ( auto const &mypair : mymap.second )
            {
                fpaperlist << mypair.first << endl;
            }
        }
    }

    return EXIT_SUCCESS;
}

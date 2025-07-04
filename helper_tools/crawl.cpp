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
#include <regex>
#include <set>
#include <utility>
#include <curl/curl.h>
#include "common.hpp"
#include "../main_program/hash.hpp"
#include "../main_program/paper.hpp"

using std::size_t, std::string, std::string_view, std::cout, std::cerr, std::endl;

std::ofstream logfile;

std::map< string, std::vector<string> > names;

std::map< string, std::map<string, std::pair<string, string> > > papers;

bool IsNotName(string_view const s) noexcept
{
    static constexpr char const *not_names[] = {
        "dkuhl@bloomberg.net",
        "et al.",
        "thomas.rodgers@woven-planet.global",
        "various SG14 contibutors including Nicolas Fleury (Ubisoft)",
        "(h2 AT fsfe.org)",
        "(EIDOS)",
        "Add container pop methods that return the popped value",
        "Woven By Toyota",
        "others",
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

size_t WriteCallback(void *const contents, size_t const size, size_t const nmemb, void *const userp)
{
    string *const data = static_cast<string*>(userp);
    if ( nullptr == data ) return 0u;
    size_t totalSize = size * nmemb;
    if ( 0u == totalSize ) return 0u;
    data->append( static_cast<char*>(contents), totalSize );
    return totalSize;
}

bool DownloadPage(char const *const arg_url, string &outContent)
{
    std::string_view url(arg_url);

    // Generate a filename from the URL
    string filename;

    // Extract the last non-empty path segment (e.g., "2024/")
    std::regex regex_filename_from_url(R"(/(\d{4})/?)");
    std::match_results< std::string_view::const_iterator > match;
    if ( std::regex_search( url.cbegin(), url.cend(), match, regex_filename_from_url ) )
    {
        filename = match[1].str() + ".html";
    }

    if ( false == filename.empty() )
    {
        // Try reading from local cache if filename is known
        std::ifstream cached(filename);
        if ( cached.is_open() )
        {
            std::cerr << "Reading cached file: " << filename << "\n";
            outContent.assign(std::istreambuf_iterator<char>(cached),
                              std::istreambuf_iterator<char>());
            return true;
        }
    }

    // No cache or cache file not found — proceed with download
    CURL *const curl = curl_easy_init();
    if ( nullptr == curl )
    {
        std::cerr << "curl_easy_init failed\n";
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, arg_url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outContent);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if ( res != CURLE_OK )
    {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << "\n";
        return false;
    }

    if ( false == filename.empty() )
    {
        // Write to cache file
        std::ofstream out(filename);
        if ( out.is_open() ) out << outContent;
    }

    return true;
}

void ExtractYearLinks(std::string_view const html, std::set<unsigned> &years)
{
    std::regex year_link(R"((\d{4})/)");
    std::match_results<std::string_view::const_iterator> match;

    auto begin = html.cbegin();
    auto end = html.cend();

    while ( std::regex_search(begin, end, match, year_link) )
    {
        // match[1] contains the year as a std::sub_match, which is a string_view
        auto year = match[1];  // This is a std::sub_match

        unsigned result = 0u;

        // Use the iterators of the sub_match
        auto [ptr, ec] = std::from_chars(&*year.first, &*year.second, result);

        if ( ec == std::errc() )
        {
            years.insert(result);
        }

        // Move `begin` to the suffix after the matched year
        begin = match.suffix().first;
    }
}

void ProcessAuthorSquareFromTable(string author, string_view const doc)
{
    ReplaceHtmlNumericEscapesInPlace(author);

// ============================ Bugs on ISO webpages
    ReplaceInPlace(author, "Berg&eacute ", "Berg&eacute;");
    ReplaceInPlace(author, "Pablo Halpern Ville Voutilainen", "Pablo Halpern\nVille Voutilainen");
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
        ReplaceInPlace(s, "n\\u0301", "\\u0144");  // ń
        ReplaceInPlace(s, "c\\u0301", "\\u0107");  // ć
        ReplaceInPlace(s, "l\\u0301", "\\u013A");  // ĺ
        ReplaceInPlace(s, "r\\u0301", "\\u0155");  // ŕ
        ReplaceInPlace(s, "s\\u0301", "\\u015B");  // ś
        ReplaceInPlace(s, "z\\u0301", "\\u017A");  // ź
        ReplaceInPlace(s, "A\\u0301", "\\u00C1");  // Á
        ReplaceInPlace(s, "E\\u0301", "\\u00C9");  // É
        ReplaceInPlace(s, "I\\u0301", "\\u00CD");  // Í
        ReplaceInPlace(s, "O\\u0301", "\\u00D3");  // Ó
        ReplaceInPlace(s, "U\\u0301", "\\u00DA");  // Ú
        ReplaceInPlace(s, "Y\\u0301", "\\u00DD");  // Ý
        ReplaceInPlace(s, "N\\u0301", "\\u0143");  // Ń
        ReplaceInPlace(s, "C\\u0301", "\\u0106");  // Ć
        ReplaceInPlace(s, "L\\u0301", "\\u0139");  // Ĺ
        ReplaceInPlace(s, "R\\u0301", "\\u0154");  // Ŕ
        ReplaceInPlace(s, "S\\u0301", "\\u015A");  // Ś
        ReplaceInPlace(s, "Z\\u0301", "\\u0179");  // Ź
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
        if ( !s.empty() && ('.' == s.back()) ) s.pop_back();
        if ( ContainsNonASCII(s) )
        {
            cout << "----- " << doc << " --- " << s << " --- ";
            for ( char const c : s )
            {
                cout << std::hex << std::setfill('0') << std::setw(2u) << (unsigned)(char unsigned)c << " ";
            }
            cout << endl;
        }
        if ( s.empty() ) continue;
        names[s].emplace_back(doc);
    }
}

void ParseYearTable(string_view const html, unsigned const year)
{
    std::regex row_regex(R"(<tr[^>]*>\s*<td[^>]*>\s*<a[^>]*>([^<]+)</a>\s*</td>\s*<td[^>]*>(.*?)</td>\s*<td[^>]*>(.*?)</td>\s*<td[^>]*>(.*?)</td>)", std::regex::icase);

    auto begin = html.cbegin();
    auto end   = html.cend();
    std::match_results< std::string_view::const_iterator > match;

    while ( std::regex_search(begin, end, match, row_regex) )
    {
        string wg21_number;
        string some_number;
        string title;
        string author;
        if ( year >= 2013u )
        {
            wg21_number  = match[1];
            title        = match[2];
            author       = match[3];
        }
        else
        {
            wg21_number  = match[1];
            some_number  = match[2];            
            title        = match[3];
            author       = match[4];
        }

        try
        {
            Paper paper{wg21_number};  // just to see if it throws because of invalid paper (e.g. SD-1)
            auto &map = papers[ paper.PaperNameWithoutRevision() ];
            map[ paper.c_str() ] = { author, title };
            ProcessAuthorSquareFromTable(author, wg21_number);
        }
        catch(...){}

        begin = match.suffix().first;
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

    string index_html;

    constexpr char index_url[] = "https://www.open-std.org/jtc1/sc22/wg21/docs/papers/";

    if ( false == DownloadPage(index_url, index_html) )
    {
        std::cerr << "DownloadPage failed.\n";
        return EXIT_FAILURE;
    }

    std::set<unsigned> years;
    ExtractYearLinks(index_html, years);

    for ( auto const &year : years )
    {
        string year_html;

        std::cerr << "Fetching year: " << year << "\n";

        if ( false == DownloadPage( (index_url + std::to_string(year) + "/").c_str(), year_html ) )
        {
            std::cerr << "Failed to fetch year page: " << year << "\n";
            continue;
        }

        ParseYearTable(year_html, year);
    }

    std::cerr << "==================== " << names.size() << " unique names ==================\n";

    std::ofstream fnames("names.txt");
    if ( fnames.is_open() )
    {
        for ( std::size_t i = 0u; i < names.size(); ++i )
        {
            string const &s = std::next(std::cbegin(names), i)->first;
            fnames << s << endl;
        }
    }

    std::ofstream fnames_papers("../main_program/AUTO_GENERATED_tree_contents_author.hpp");
    if ( fnames_papers.is_open() )
    {
        fnames_papers << "{\n";
        for ( std::size_t i = 0u; i < names.size(); ++i )
        {
            auto const &e = *std::next(std::cbegin(names), i);
            fnames_papers << "    { 0x"
                          << std::hex << std::setfill('0') << std::setw(16)
                          << Hash(e.first) << std::dec << ", { wxS(\"" << e.first << "\"), PaperList< ";
            bool already_got_one = false;
            for ( auto const &e2 : e.second )
            {
                if ( already_got_one ) fnames_papers << ", ";
                already_got_one = true;
                fnames_papers << "\"" << Paper(e2).c_str() << "\"";
            }
            fnames_papers << " >() } },\n";
        }
    }
    fnames_papers << "};\n";

    std::ofstream fpapers("../main_program/AUTO_GENERATED_tree_contents_paper.hpp");
    if ( fpapers.is_open() )
    {
        fpapers << "{\n";
        for ( std::size_t i = 0u; i < papers.size(); ++i )
        {
            auto const &e = *std::next(std::cbegin(papers), i);
            fpapers << "    { \"" << e.first;
            if ( 'p' == e.first[0] ) fpapers << "r0";
            fpapers << "\", RevList< ";

            auto const &mymap = e.second;
            bool already_got_one = false;
            for ( std::size_t j = 0u; j < mymap.size(); ++j )
            {
                auto const &mypair = *std::next(std::cbegin(mymap), j);
                if ( already_got_one ) fpapers << ", ";
                already_got_one = true;
                fpapers << "Rev< " << Paper(mypair.first).rev << "u, Arr64< Hash(wxS(\"";
                string author = mypair.second.first;
                MonkeyString(author);
                fpapers << author;
                fpapers << "\")) >(), wxS(\"";
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

    return EXIT_SUCCESS;
}

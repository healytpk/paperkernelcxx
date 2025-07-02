#include <cassert>
#include <cstring>                           // strcmp
#include <algorithm>                         // replace
#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <regex>
#include <set>
#include <curl/curl.h>
#include "common.hpp"

using std::cout, std::cerr, std::endl;

using std::string_view;

std::ofstream logfile;

std::set< std::string > names;

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
        nullptr
    };

    for ( char const *const *pp = not_names; nullptr != *pp; ++pp )
    {
        if ( s == *pp ) return true;
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

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t totalSize = size * nmemb;
    std::string *data = static_cast<std::string *>(userp);
    data->append(static_cast<char *>(contents), totalSize);
    return totalSize;
}

bool DownloadPage(std::string const &url, std::string &outContent)
{
    // Generate a filename from the URL
    std::string filename;
    {
        // Extract the last non-empty path segment (e.g., "2024/")
        std::regex filename_from_url(R"(/(\d{4})/?)");
        std::smatch match;
        if ( std::regex_search(url, match, filename_from_url) )
        {
            filename = match[1].str() + ".html";
        }
    }

    // Try reading from local cache if filename is known
    if ( !filename.empty() )
    {
        std::ifstream cached(filename);
        if ( cached )
        {
            std::cerr << "Reading cached file: " << filename << "\n";
            outContent.assign(std::istreambuf_iterator<char>(cached),
                              std::istreambuf_iterator<char>());
            return true;
        }
    }

    // No cache or cache file not found — proceed with download
    CURL *curl = curl_easy_init();
    if ( !curl )
    {
        std::cerr << "curl_easy_init failed\n";
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
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

    // Write to cache file
    if ( !filename.empty() )
    {
        std::ofstream out(filename);
        if ( out ) out << outContent;
    }

    return true;
}

void ExtractYearLinks(std::string const &html, std::set<std::string> &years)
{
    std::regex year_link(R"((\d{4})/)");
    std::smatch match;

    auto begin = html.cbegin();
    auto end = html.cend();

    while ( std::regex_search(begin, end, match, year_link) )
    {
        years.insert(match[1]);
        begin = match.suffix().first;
    }
}

void ParseYearTable(std::string const &html, std::string const &year)
{
#if 0
    std::regex row_regex(R"(<tr>\s*<td[^>]*>\s*<a[^>]*>([^<]+)</a>\s*</td>\s*<td[^>]*>([^<]*)</td>\s*<td[^>]*>([^<]*)</td>\s*<td[^>]*>([^<]*)</td>)", std::regex::icase);
#else
    std::regex row_regex(R"(<tr[^>]*>\s*<td[^>]*>\s*<a[^>]*>([^<]+)</a>\s*</td>\s*<td[^>]*>(.*?)</td>\s*<td[^>]*>(.*?)</td>\s*<td[^>]*>(.*?)</td>)", std::regex::icase);
#endif
    auto begin = html.cbegin();
    auto end = html.cend();
    std::smatch match;

    while ( std::regex_search(begin, end, match, row_regex) )
    {
        std::string wg21_number;
        std::string some_number;
        std::string title;
        std::string author;
        if ( std::stoul(year) >= 2013u )
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

#if 0
        cout << wg21_number << " | " << title << " | " << author << " | " << date << "\n";
#else
        std::replace(author.begin(), author.end(), ',', '\n');
        ReplaceHtmlNumericEscapesInPlace(author);
        ReplaceInPlace(author, "\u00A0"  , " "     );  // Non-breaking space

        ReplaceInPlace(author, "Pablo Halpern Ville Voutilainen", "Pablo Halpern\nVille Voutilainen");
// ============================ Bugs on ISO webpages
        ReplaceInPlace(author, "Berg&eacute ", "Berg&eacute;");
// ===================================================
        ReplaceInPlace(author, "\\u2019", "'");
        ReplaceInPlace(author, "`", "'");
        ReplaceInPlace(author, "&aacute;", "\u00E1");
        ReplaceInPlace(author, "&eacute;", "\u00E9");
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
        ReplaceInPlace(author, "&Eacute;", "\u00C9");
        ReplaceInPlace(author, ";"       , "\n"    );
        ReplaceInPlace(author, " and "   , "\n"    );
        ReplaceInPlace(author, " / "     ,  "\n"   );
        TrimWhitespace(author);
        
        std::vector<std::string> authors = SplitByNewLines(author);
        for ( std::string &s : authors )
        {
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
            Erase(s, "(h2 AT fsfe.org)");
            Erase(s, " et al.");
            if ( ContainsNonASCII(s) )
            {
                cout << "----- " << year << " --- " << s << " --- ";
                for ( char const c : s )
                {
                    cout << std::hex << (unsigned)(char unsigned)c << " ";
                }
                cout << endl;
            }
            if ( s.empty() ) continue;
            names.insert(s);
        }

#endif

        begin = match.suffix().first;
    }
}

int main()
{
    std::cerr << "Attempting to create 'log.txt' in current directory. . .\n";
    logfile.open("log.txt");
    if ( ! logfile )
    {
        std::cerr << "Failed to create file 'logfile.txt' in current directory\n";
        return EXIT_FAILURE;
    }
    logfile << "First line in log file\n";

    std::string index_url = "https://www.open-std.org/jtc1/sc22/wg21/docs/papers/";
    std::string index_html;

    if ( !DownloadPage(index_url, index_html) )
    {
        return EXIT_FAILURE;
    }

    std::set<std::string> years;
    ExtractYearLinks(index_html, years);

    for ( auto const &year : years )
    {
        std::string year_url = index_url + year + "/";
        std::string year_html;

        std::cerr << "Fetching year: " << year_url << "\n";

        if ( !DownloadPage(year_url, year_html) )
        {
            std::cerr << "Failed to fetch year page: " << year << "\n";
            continue;
        }

        ParseYearTable(year_html, year);
    }

    std::cerr << "==================== " << names.size() << " unique names ==================\n";
    for ( std::size_t i = 0u; i < names.size(); ++i )
    {
        std::string const &s = *std::next(std::begin(names), i);
        cout << s << endl;
    }

    return EXIT_SUCCESS;
}

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <set>
#include <curl/curl.h>

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t totalSize = size * nmemb;
    std::string *data = static_cast<std::string *>(userp);
    data->append(static_cast<char *>(contents), totalSize);
    return totalSize;
}

bool DownloadPage(std::string const &url, std::string &outContent)
{
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
    std::regex row_regex(R"(<tr>\s*<td[^>]*>\s*<a[^>]*>([^<]+)</a>\s*</td>\s*<td[^>]*>([^<]*)</td>\s*<td[^>]*>([^<]*)</td>\s*<td[^>]*>([^<]*)</td>)", std::regex::icase);
    auto begin = html.cbegin();
    auto end = html.cend();
    std::smatch match;

    while ( std::regex_search(begin, end, match, row_regex) )
    {
        std::string wg21_number = match[1];
        std::string title        = match[2];
        std::string author       = match[3];
        std::string date         = match[4];

        std::cout << wg21_number << " | " << title << " | " << author << " | " << date << "\n";

        begin = match.suffix().first;
    }
}

int main()
{
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

    return EXIT_SUCCESS;
}

#include <cstddef>                  // size_t
#include <cstdlib>                  // EXIT_FAILURE, EXIT_SUCCESS
#include <filesystem>               // filesystem, directory_iterator
#include <fstream>                  // ifstream, ofstream
#include <iostream>                 // cout, cerr, endl
#include <regex>                    // regex, smatch, regex_search
#include <string>                   // string, to_string
#include <vector>                   // vector
#include <curl/curl.h>              // CURL, curl_easy_*, etc.

using std::size_t;
using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::cout;
using std::cerr;
using std::endl;
namespace fs = std::filesystem;

size_t WriteDiscard(char const *const ptr, size_t const size, size_t const nmemb, void *const userdata) noexcept
{
    // Discard all data
    return size * nmemb;
}

size_t WriteFile(void *const ptr, size_t const size, size_t const nmemb, void *const stream) noexcept
{
    ofstream *const outfile = static_cast<ofstream*>(stream);
    outfile->write(static_cast<char const *>(ptr), size * nmemb);
    return size * nmemb;
}

string GetFinalUrl(CURL *const curl, string const &url)
{
    curl_easy_reset(curl);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteDiscard);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, nullptr);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.68.0");

    CURLcode const res = curl_easy_perform(curl);
    if ( CURLE_OK != res )
        return {};

    char *final_url = nullptr;
    CURLcode const info_res = curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &final_url);
    if ( (CURLE_OK == info_res) && (nullptr != final_url) )
        return string(final_url);
    return {};
}

string GetFilenameFromUrl(string const &url)
{
    size_t const last_slash = url.find_last_of('/');
    if ( string::npos == last_slash ) return {};
    return url.substr(last_slash + 1u);
}

int main(void)
{
    ifstream infile("../complete_paper_list.txt");
    if ( false == infile.is_open() )
    {
        cerr << "Could not open ../complete_paper_list.txt\n";
        return EXIT_FAILURE;
    }

    string line;
    vector<string> missing;

    while ( std::getline(infile, line) )
    {
        if ( 0u == line.size() ) continue;
        cout << "Processing: " << line << " . . . ";
        bool found = false;
        for ( auto const &entry : fs::directory_iterator("../downloaded_papers") )
        {
            if ( false == entry.is_regular_file() ) continue;
            string const fname = entry.path().filename().string();
            if ( (0u == fname.rfind(line, 0u)) && ('.' == fname[line.size()]) )
            {
                found = true;
                cout << " FOUND.\n";
                break;
            }
        }
        if ( false == found )
        {
            missing.push_back(line);
            cout << " -------------------------------------------------- missing\n";
        }
    }

    CURL *const curl = curl_easy_init();
    if ( nullptr == curl )
    {
        cerr << "Could not initialize libcurl\n";
        return EXIT_FAILURE;
    }

    for ( string const &paper : missing )
    {
        string const url = "http://wg21.link/" + paper;
        cout << "Resolving final URL for: " << url << endl;

        string const final_url = GetFinalUrl(curl, url);
        if ( 0u == final_url.size() )
        {
            cerr << "ERROR: Could not resolve final URL for " << paper << endl;
            continue;
        }
        cout << "Final URL: " << final_url << endl;

        string const filename = GetFilenameFromUrl(final_url);
        if ( 0u == filename.size() )
        {
            cerr << "ERROR: Could not extract filename from URL: " << final_url << endl;
            continue;
        }
        cout << "Filename: " << filename << endl;

        string const outpath = "./freshly_downloaded_papers/" + filename;
        ofstream outfile(outpath, std::ios::binary);
        if ( false == outfile.is_open() )
        {
            cerr << "ERROR: Could not open output file: " << outpath << endl;
            continue;
        }

        curl_easy_reset(curl);
        curl_easy_setopt(curl, CURLOPT_URL, final_url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0L); // No need to follow, already resolved
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFile);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outfile);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.68.0");

        CURLcode const res = curl_easy_perform(curl);
        if ( CURLE_OK != res )
        {
            cerr << "Download failed: " << curl_easy_strerror(res) << endl;
            outfile.close();
            fs::remove(outpath);
        }
        else
        {
            outfile.close();
            cout << "Download complete: " << outpath << endl;
        }
    }

    curl_easy_cleanup(curl);
    return EXIT_SUCCESS;
}

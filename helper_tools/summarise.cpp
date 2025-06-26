#include <cassert>                    // assert
#include <cctype>                     // isspace, tolower
#include <cstdlib>                    // EXIT_FAILURE
#include <algorithm>                  // transform
#include <filesystem>                 // directory_iterator, path
#include <fstream>                    // ifstream, ofstream
#include <iostream>                   // endl
#include <map>                        // map
#include <regex>                      // regex, smatch
#include <sstream>                    // ostringstream
#include <string>                     // string
#include <string_view>                // string_view
#include <set>                        // set
#include <gumbo.h>
#include "common.hpp"                 // ExtractTitleFromFileHTML/PDF
#include "hasher.hpp"
#include "../main_program/paper.hpp"  // Paper
#include "escape_hex.hpp"

namespace fs = std::filesystem;
using std::string;
using std::ifstream;
using std::cout;
using std::cerr;
using std::endl;
using std::int32_t;
using std::ifstream;
using std::ofstream;
using std::ostringstream;
using std::size_t;
using std::string;
using std::string_view;
using std::unique_ptr;
using std::vector;

std::ofstream logfile;

unsigned count_html  = 0u;
    unsigned count_bikeshed = 0u;
unsigned count_pdf   = 0u;
unsigned count_md    = 0u;
unsigned count_txt   = 0u;
unsigned count_other = 0u;

size_t ifind(string_view haystack, string_view needle, size_t const i = 0u)
{
    // Assume needle to already be lowercase
    assert( haystack.size() >= i );
    string lowercase(haystack);
    for ( char &c : lowercase ) c = std::tolower( (char unsigned)c );
    return lowercase.find(needle,i);
}

size_t ifind(std::string const &haystack, std::string const &needle, size_t const i = 0u)
{
    return ifind(string_view(haystack), string_view(needle), i);
}

string_view &Trim(string_view &sv)
{
    while ( !sv.empty() && std::isspace(static_cast<unsigned char>(sv.front())) )
    {
        sv.remove_prefix(1);
    }
    while ( !sv.empty() && std::isspace(static_cast<unsigned char>(sv.back())) )
    {
        sv.remove_suffix(1);
    }

    return sv;
}

string &Trim(string &s)
{
    while ( !s.empty() && std::isspace(static_cast<unsigned char>(s.front())) )
    {
        s.erase(0,1);
    }
    while ( !s.empty() && std::isspace(static_cast<unsigned char>(s.back())) )
    {
        s.pop_back();
    }

    return s;
}

struct Author {
    string name;
    std::vector<string> files;
    bool operator==(Author const &rhs) { return name == rhs.name; }
    bool operator< (Author const &rhs) { return name  < rhs.name; }
};

std::map<long long unsigned, Author> g_all_authors;

void normalize_name(string &name)
{
    name.erase(std::remove_if(name.begin(), name.end(), [](unsigned char c) {
        return !std::isalpha(c); // Keep only letters
    }), name.end());

    std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c) {
        return std::tolower(c);
    });

    Trim(name);
}

void RecordAuthor(string_view arg, string_view filename)
{
    cerr << arg << " ------ ";

    string s2(arg);
    Trim(s2);
    size_t pos = 0;
    while ( -1 != (pos = s2.find("\xE2\x80\x94", pos)) )
    {
        s2.replace(pos, 3, "-");
        ++pos;
    }
    if ( -1 != (pos = s2.find(" - " )) ) s2.resize(pos);
    if ( -1 != (pos = s2.find("&lt;")) ) s2.resize(pos);
    if ( -1 != (pos = s2.find("("   )) ) s2.resize(pos);
    Trim(s2);

    cerr << s2 << " ------ ";
    string s(s2);
    normalize_name(s);
    cerr << s << " ---- ";

    long long unsigned digest = GetHash(s.c_str());
    cerr << "0x" << std::hex << std::setfill('0') << std::setw(16) << digest << endl;
    cerr << std::dec;
    Author &a = g_all_authors[digest];
    if ( false == a.name.empty() )
    {
        if ( (a.name == "H. Carter Edwards") && (s2 == "H Carter Edwards") ) {}
        else if ( (a.name == "D. S. Hollman") && (s2 == "D.S. Hollman") ) {}
        else if ( false == std::ranges::equal(a.name, s2, [](char a, char b){ return std::tolower(a) == std::tolower(b); } ) )
        {
            cerr << "======== ERROR: " << a.name << " != " << s2 << endl;
            cerr << "size = " << a.name.size() << ", size = " <<  s2.size() << endl;
            std::abort();
        }
    }
    else
    {
        a.name = s2;
    }
    a.files.emplace_back(filename);
}

int main(void)
{
    cerr << "Attempting to create 'log.txt' in current directory. . .\n";
    logfile.open("log.txt");
    if ( ! logfile )
    {
        cerr << "Failed to create file 'logfile.txt' in current directory\n";
        return EXIT_FAILURE;
    }
    logfile << "First line in log file\n";

    // Path to the directory containing the .tokens files
    string const path = "../papers/";

    // Regex pattern to match files in the form "PxxxxRx.*"
    std::regex const pattern(R"(p(\d+)r(\d+)\.[A-z]*)", std::regex::icase);

    // Iterate through all files in the specified directory
    for ( auto const &entry : fs::directory_iterator(path) )
    {
        auto const &filename = entry.path().filename().string();

        // Check if the filename matches the pattern
        if ( false == std::regex_match(filename, pattern) ) continue;

        std::smatch match;
        if ( false == std::regex_search(filename, match, pattern) ) continue;

        // Extract the paper number and revision number
        unsigned const paper_number    = std::stoul(match[1].str());
        unsigned const revision_number = std::stoul(match[2].str());

        if ( filename.contains(".htm") )
        {
            ++count_html;
            std::ifstream f(entry.path());
            assert( f.is_open() );
            std::stringstream ss;
            ss << f.rdbuf();
            string s = std::move(ss).str();
            assert( false == s.empty() );
            //for ( char &c : s ) c = std::tolower( (char unsigned)c );
            if ( -1 == ifind(s,"content=\"bikeshed") ) continue;
            ++count_bikeshed;
            size_t n = -1;
            if ( -1 != (n=ifind(s,"<dt>author")) || -1 != (n=ifind(s,"<dt class=\"editor\">author")) )
            {
                size_t n2 = ifind(s,"<dt", n + 3u);
                if ( -1 == n2 )
                {
                    cerr << "---------- missing another \"<dt\" after Author ----------\n";
                    std::abort();
                }
                std::string_view sv( &s[n], n2 - n );
                assert( '<' != sv.back() );
                //cerr << "====== " << sv << " ======\n";
                size_t n3 = 0u;
                size_t n4 = -1;
                while ( -1 != (n4 = ifind(sv,"<dd",n3)) )
                {
                    n3 = ifind(sv, ">",n4 + 3);
                    if ( -1 == n3 )
                    {
                        cerr << "---------- missing a close angle bracket after \"<dd\" ----------\n";
                        std::abort();
                    }
                    ++n3;  // move forward after the close angle bracket
                    size_t n5 = ifind(sv, "<dd",n3);
                    if ( -1 == n5 ) n5 = sv.size();
                    string_view sv_author(&sv[n3], n5 - n3);
                    Trim(sv_author);
                    if ( sv_author.empty() ) continue;
                    if ( sv_author.ends_with("</dd>") ) continue;
                    bool is_just_whitespace = true;
                    for ( char const ch : sv_author )
                    {
                        if ( false == std::isspace( static_cast<unsigned char>(ch)) )
                        {
                            is_just_whitespace = false;
                            break;
                        }
                    }
                    if ( is_just_whitespace ) continue;
                    //cerr << "Author len=" << sv_author.size() << " : " << sv_author << " ::: " << filename << endl;

                    Author author;
                    std::regex simple_name_regex(R"(<span class="p-name fn">(.*?)</span>\s*((.+))?)"); // Extracts name from <span>
                    //std::regex full_regex("<a class=\\\"p-name fn u-email email\\\" href=\\\"(.+)\\\">(.+)\\s*\\((.+)\\)</a>(.*)");
                    //std::regex full_regex("<a class=\\\"p-name fn u-email email\\\" href=\\\"(.+?)\\\">(.+?)\\s*\\((.+?)\\)</a>\\s*(.*)");
                    std::regex full_regex_E ("<a class=\\\"p-name fn u-email email\\\" href=\\\"mailto:(.+?)\\\">([^<]+)</a>\\s*(\\(<span[^<]([^<]+)</span>\\))?");
                    std::regex full_regex_E2("<a class=\\\"p-name fn u-email email\\\" href=\\\"mailto:(.+?)\\\">([^<]+)</a>.*");
                    std::regex full_regex_U("<a class=\\\"p-name fn u-url url\\\" href=\\\"(.+?)\\\">([^<]+)</a>\\s*(\\(<span[^<]([^<]+)</span>\\))?");
                    string s_author(sv_author);
                    //s_author = "<a class=\"p-name fn u-email email\" href=\"mailto:phdofthehouse@gmail.com\">jeanheyd meneide (https://thephd.dev)</a>";
                    if ( std::regex_match( s_author, match, simple_name_regex) )
                    {
                        author.name = match[1].str();
                    }
                    else if ( std::regex_match( s_author, match, full_regex_E) )
                    {
                        //author.email = match[1].str();
                        author.name  = match[2].str();
                    }
                    else if ( std::regex_match( s_author, match, full_regex_E2) )
                    {
                        //author.email = match[1].str();
                        author.name  = match[2].str();
                    }
                    else if ( std::regex_match( s_author, match, full_regex_U) )
                    {
                        //author.email = match[1].str();
                        author.name  = match[2].str();
                    }
                    else
                    {
                        std::abort();
                    }

                    Trim(author.name);
                    if ( false == author.name.empty() ) RecordAuthor(author.name, filename);

#if 0
                    cerr << "Author: " << author.name << " --- Email: " << author.email << " --- Affiliation: " << author.affiliation
                              << GetHash(author.name.c_str()) << endl;
#endif
                }
            }
            else if ( -1 != ifind(s,"<span class=\"metadata-key\">author") )
            {
                cerr << "This BikeShed has multiple span metadata-key's  -- " << entry.path().string() << endl;
            }
            else if ( -1 != ifind(s,"<dt class=\"editor\">editor") )
            {
                cerr << "This BikeShed has Editor -- " << entry.path().string() << endl;
            }
            else if ( -1 != ifind(s,"reply-to") )
            {
                cerr << "This BikeShed has Reply-to -- " << entry.path().string() << endl;
            }
            else if ( -1 != ifind(s,"polls</h1>") )
            {
                cerr << "This BikeShed is a poll -- " << entry.path().string() << endl;
            }
            else if ( -1 != ifind(s,"report</h1>") )
            {
                cerr << "This BikeShed is a report -- " << entry.path().string() << endl;
            }
            else
            {
                cerr << "Bikeshed doesn't have Author -- " << entry.path().string() << endl;
            }
        }
        else if ( filename.contains(".pdf") )
        {
            ++count_pdf;
        }
        else if ( filename.contains(".md") )
        {
            ++count_md;
        }
        else if ( filename.contains(".txt") )
        {
            ++count_txt;
        }
        else
        {
            ++count_other;
        }
    }

    cerr << "count_html  = " << count_html  << endl;
    cerr << "count_bikeshed  = " << count_bikeshed << endl;
    cerr << "count_pdf   = " << count_pdf   << endl;
    cerr << "count_md    = " << count_md    << endl;
    cerr << "count_txt   = " << count_txt   << endl;
    cerr << "count_other = " << count_other << endl;

    for ( auto &mypair : g_all_authors )
    {
        cerr << mypair.second.name << " has " << mypair.second.files.size() << " papers\n";
    }

    for ( auto &mypair : g_all_authors )
    {
        string &s = mypair.second.name;
        for ( size_t i = 0u; i < s.size(); ++i )
        {
            if ( '\"' != s[i] ) continue;
            s.insert(i++,"\\");
        }
    }

    std::ofstream  fs("../main_program/AUTO_GENERATED_tree_contents_author_char.hpp" );
    std::ofstream fws("../main_program/AUTO_GENERATED_tree_contents_author_wchar_t.hpp");

    fs  << "{\n";
    fws << "{\n";
    for ( auto &mypair : g_all_authors )
    {
        fs  << "    { " << mypair.first << "u, {  \"";
        fws << "    { " << mypair.first << "u, { L\"";

        string name_char    = mypair.second.name;
        if ( std::string_view(name_char).starts_with("Hana Dus") ) name_char = "Hana Dusikova";
        string name_wchar_t = name_char;

         replace_non_ascii_with_hex( name_char    );
        Lreplace_non_ascii_with_hex( name_wchar_t );

        fs  << name_char;
        fws << name_wchar_t;

        fs  << "\", { ";
        fws << "\", { ";

        for ( auto &e : mypair.second.files )
        {
            Paper paper(e);
            fs  << "{ " << paper.num << "u, " << paper.rev << "u }, ";
            fws << "{ " << paper.num << "u, " << paper.rev << "u }, ";
        }
        fs  << " } } },\n";
        fws << " } } },\n";
    }
    fs  << "}\n";
    fws << "}\n";
}


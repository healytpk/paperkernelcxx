#include <cctype>
#include <cstdlib>                    // EXIT_FAILURE
#include <filesystem>                 // directory_iterator, path
#include <fstream>                    // ifstream, ofstream
#include <iostream>                   // endl
#include <map>                        // map
#include <regex>                      // regex, smatch
#include <string>                     // string
#include <set>                        // set
#include "common.hpp"                 // ExtractTitleFromFileHTML/PDF
#include "escape_hex.hpp"

namespace fs = std::filesystem;
using std::string;

std::ofstream logfile;

struct RevInfo {
    unsigned n;
    string title, author;
    constexpr bool operator<(RevInfo const &rhs) const { return this->n < rhs.n; }
};

// Define the PaperTree structure
using PaperTree = std::map< unsigned, std::set<RevInfo> >;

string Title(void)
{
    static long unsigned n = 0u;
    return "Title" + std::to_string(++n);
}

string Author(void)
{
    static long unsigned n = 0u;
    return "Author" + std::to_string(++n);
}

void FixTitle(std::string &arg)
{
    // Step 1: Remove prefix "PxxxxRx: " or "PxxxxRxx: "
    static const std::regex prefix_pattern("^P\\d{4}R\\d{1,2}: ");
    arg = std::regex_replace(arg, prefix_pattern, "");

    // Step 2: Convert all whitespace to ' ', remove other non-printables
    std::string cleaned;
    for ( char c : arg )
    {
        unsigned char uc = static_cast<unsigned char>(c);
        if ( std::isprint(uc) )
        {
            cleaned += std::isspace(uc) ? ' ' : c;
        }
    }

    // Step 3: Collapse multiple spaces into one
    std::string collapsed;
    bool in_space = false;
    for ( char c : cleaned )
    {
        if ( c == ' ' )
        {
            if ( !in_space )
            {
                collapsed += ' ';
                in_space = true;
            }
        }
        else
        {
            collapsed += c;
            in_space = false;
        }
    }

    // Step 4: Trim leading and trailing spaces
    size_t start = collapsed.find_first_not_of(' ');
    size_t end = collapsed.find_last_not_of(' ');
    std::string trimmed;
    if ( start != std::string::npos )
    {
        trimmed = collapsed.substr(start, end - start + 1);
    }

    // Step 5: Escape all double quotes and remove backslashes before symbols
    std::string final;
    for ( size_t i = 0; i < trimmed.size(); ++i )
    {
        char c = trimmed[i];
        if ( c == '"' )
        {
            final += "\\\"";
        }
        else if ( c == '\\' )
        {
            if ( i + 1 < trimmed.size() )
            {
                char next = trimmed[i + 1];
                if ( !std::isalnum(static_cast<unsigned char>(next)) )
                {
                    // Skip the backslash
                    continue;
                }
            }
            final += '\\';
        }
        else
        {
            final += c;
        }
    }

    arg = final;
}

int main(void)
{
    std::cerr << "Attempting to create 'log.txt' in current directory. . .\n";
    logfile.open("log.txt");
    if ( ! logfile )
    {
        std::cerr << "Failed to create file 'logfile.txt' in current directory\n";
        return EXIT_FAILURE;
    }
    logfile << "First line in log file\n";

    std::ofstream  fs("../main_program/AUTO_GENERATED_tree_contents_paper.hpp" );

    // Path to the directory containing the .tokens files
    string const path = "../papers/";

    // Regex pattern to match files in the form "PxxxxRx.*"
    std::regex const pattern(R"(p(\d+)r(\d+)\.[A-z]*)", std::regex::icase);

    // PaperTree to store paper numbers and their corresponding revision numbers
    PaperTree papers;

    // Iterate through all files in the specified directory
    for ( auto const &entry : fs::directory_iterator(path) )
    {
        auto const &filename = entry.path().filename().string();

        // Check if the filename matches the pattern
        if ( false == std::regex_match(filename, pattern) ) continue;

        std::smatch match;
        if ( false == std::regex_search(filename, match, pattern) ) continue;

        // Extract the paper number and revision number
        unsigned const paper_number = std::stoul(match[1].str());
        unsigned const revision_number = std::stoul(match[2].str());

        string title, author;
        if ( filename.contains("htm") )
        {
            title  = ExtractTitleFromFileHTML (entry.path().string());
            author = ExtractAuthorFromFileHTML(entry.path().string());
        }
        else if ( filename.contains("pdf") )
        {
            title  = ExtractTitleFromFilePDF (entry.path().string());
            author = ExtractAuthorFromFilePDF(entry.path().string());
        }
        FixTitle(title);
        if ( title .empty() ) title  = Title ();
        if ( author.empty() ) author = Author();

        // Store the revision number under the corresponding paper number
        auto [ iterator, is_new ] =  papers[paper_number].insert( RevInfo{revision_number, title, author } );
        if ( false == is_new )
        {
            std::cout << " -- DUPLICATE PAPER NUMBER --\n";
            std::abort();
        }
    }

    // Print out the PaperTree structure in the desired format
    fs  << "{\n";
    for ( auto const &entry : papers )
    {
        fs  << "    { ";

        if ( entry.first <   10u ) fs << ' ';
        if ( entry.first <  100u ) fs << ' ';
        if ( entry.first < 1000u ) fs << ' ';

        fs  << entry.first << "u, RevList<";

        auto rev_it = entry.second.begin();

        bool is_first_rev_on_line = true;
        for ( auto const &e : entry.second )
        {
            if ( false == is_first_rev_on_line )
            {
                fs  << ",";
            }
            is_first_rev_on_line = false;

            string title  = e.title ;
            string author = e.author;
            if ( std::string_view(author).starts_with("Hana Dus") ) author = "Hana Dusikova";
            if ( author == "R. \"Tim" ) author = "R. Tim";
            Lreplace_non_ascii_with_hex(title );
            Lreplace_non_ascii_with_hex(author);
            fs << " Rev< " << e.n << "u, Arr64< Hash(wxS(\"" << author << "\")) >(), wxS(\"" << title << "\") > ";
        }

        fs  << " >() },\n" << std::flush;
    }
    fs  << "}\n";

    std::cerr << Title()  << std::endl;
    std::cerr << Author() << std::endl;

    return EXIT_SUCCESS;
}

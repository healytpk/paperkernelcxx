#include <cctype>
#include <cstdlib>                    // EXIT_FAILURE
#include <filesystem>                 // directory_iterator, path
#include <fstream>                    // ifstream, ofstream
#include <iostream>                   // endl
#include <map>                        // map
#include <regex>                      // regex, smatch
#include <sstream>                    // ostringstream
#include <string>                     // string
#include <set>                        // set
#include <gumbo.h>
#include "common.hpp"                 // ExtractTitleFromFileHTML/PDF

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
using std::unique_ptr;
using std::vector;

std::ofstream logfile;

struct RevInfo {
    unsigned n;
    string title, author;
    constexpr bool operator<(RevInfo const &rhs) const { return this->n < rhs.n; }
};

// Define the PaperTree structure
using PaperTree = std::map< unsigned, std::set<RevInfo> >;

string FakeAuthor(void)
{
    static long unsigned n = 0u;
    return "Author" + std::to_string(++n);
}

struct Author {
    std::string name;
    std::string email;
    std::string org;
};

bool has_class(GumboNode* node, const std::string& cls)
{
    if (node->type != GUMBO_NODE_ELEMENT) return false;

    GumboAttribute* attr = gumbo_get_attribute(&node->v.element.attributes, "class");
    if (!attr) return false;

    std::string class_attr = attr->value;
    // Simple substring search - for robust you might want to split by spaces
    return class_attr.find(cls) != std::string::npos;
}

std::string get_node_text(GumboNode* node)
{
    if (node->type == GUMBO_NODE_TEXT)
    {
        return std::string(node->v.text.text);
    }
    else if (node->type == GUMBO_NODE_ELEMENT ||
             node->type == GUMBO_NODE_DOCUMENT)
    {
        std::string text;
        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i)
        {
            GumboNode* child = static_cast<GumboNode*>(children->data[i]);
            text += get_node_text(child);
        }
        return text;
    }
    return "";
}

// Find <dt class="editor">Authors:</dt> node
GumboNode* find_authors_dt(GumboNode* node)
{
    if (node->type != GUMBO_NODE_ELEMENT) return nullptr;

    if (node->v.element.tag == GUMBO_TAG_DT && has_class(node, "editor"))
    {
        std::string text = get_node_text(node);
        if (text.find("Authors") != std::string::npos)
            return node;
    }

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i)
    {
        GumboNode* found = find_authors_dt(static_cast<GumboNode*>(children->data[i]));
        if (found) return found;
    }
    return nullptr;
}

// Get next sibling of a node in Gumbo DOM (not provided by Gumbo natively)
GumboNode* get_next_sibling(GumboNode* parent, GumboNode* node)
{
    if (parent->type != GUMBO_NODE_ELEMENT)
        return nullptr;

    GumboVector* siblings = &parent->v.element.children;
    for (unsigned int i = 0; i < siblings->length; ++i)
    {
        if (siblings->data[i] == node)
        {
            if (i + 1 < siblings->length)
                return static_cast<GumboNode*>(siblings->data[i + 1]);
            else
                return nullptr;
        }
    }
    return nullptr;
}

// Extract email from href="mailto:..."
std::string extract_email(GumboNode* a_node)
{
    if (a_node->type != GUMBO_NODE_ELEMENT) return "";
    GumboAttribute* href = gumbo_get_attribute(&a_node->v.element.attributes, "href");
    if (href && std::string(href->value).find("mailto:") == 0)
    {
        return std::string(href->value + 7);
    }
    return "";
}

std::vector<Author> extract_authors(GumboNode* root)
{
    std::vector<Author> authors;

    GumboNode* authors_dt = find_authors_dt(root);
    if (!authors_dt)
        return authors;

    // Get the parent node (the <dl>)
    GumboNode* dl_node = authors_dt->parent;

    GumboNode* sibling = get_next_sibling(dl_node, authors_dt);

    while (sibling)
    {
        if (sibling->type == GUMBO_NODE_ELEMENT)
        {
            if (sibling->v.element.tag == GUMBO_TAG_DT)
                break; // stop at next <dt>

            if (sibling->v.element.tag == GUMBO_TAG_DD &&
                has_class(sibling, "editor") &&
                has_class(sibling, "p-author"))
            {
                Author author;

                // Find <a class="p-name fn u-email email">
                GumboVector* children = &sibling->v.element.children;
                for (unsigned int i = 0; i < children->length; ++i)
                {
                    GumboNode* child = static_cast<GumboNode*>(children->data[i]);
                    if (child->type == GUMBO_NODE_ELEMENT &&
                        child->v.element.tag == GUMBO_TAG_A &&
                        has_class(child, "p-name"))
                    {
                        author.name = get_node_text(child);
                        author.email = extract_email(child);
                    }
                    else if (child->type == GUMBO_NODE_ELEMENT &&
                             child->v.element.tag == GUMBO_TAG_SPAN &&
                             has_class(child, "p-org"))
                    {
                        author.org = get_node_text(child);
                    }
                }
                authors.push_back(author);
            }
        }
        sibling = get_next_sibling(dl_node, sibling);
    }

    return authors;
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

        string author;
        if ( filename.contains("htm") )
        {
            ifstream file(path);
            if ( !file )
            {
                logfile << "-------------------- Failed to open HTML file: " << path << endl;
                return {};
            }

            ostringstream buffer;
            buffer << file.rdbuf();
            string const html = std::move(buffer).str();

            GumboOutput *const output = gumbo_parse( html.c_str() );
            std::cout << "A1\n";
            auto myvec = extract_authors( output->root );
            std::cout << "A2\n";
            for ( auto const &e : myvec )
            {
                std::cout << e.name << std::endl;
            }
        }
        /*
        else if ( filename.contains("pdf") )
        {
            author = ExtractAuthorFromFilePDF(entry.path().string());
        }
        */
        if ( author.empty() ) author = FakeAuthor();

        // Store the revision number under the corresponding paper number
        auto [ iterator, is_new ] =  papers[paper_number].insert( RevInfo{revision_number, "NoTitle", author } );
        if ( false == is_new )
        {
            std::cout << " -- DUPLICATE PAPER NUMBER --\n";
            std::abort();
        }
    }

#if 0
    // Print out the PaperTree structure in the desired format
    std::cout << "{\n";
    for ( auto const &entry : papers )
    {
        std::cout << "    { ";

        if ( entry.first <   10u ) std::cout << ' ';
        if ( entry.first <  100u ) std::cout << ' ';
        if ( entry.first < 1000u ) std::cout << ' ';

        std::cout << entry.first << "u, { ";

        auto rev_it = entry.second.begin();
        for ( auto const &e : entry.second )
        {
            std::cout << "{ " << e.n << "u, \"" << e.title << "\", \"" << e.author << "\" }, ";
        }

        std::cout << "} },\n";
    }
    std::cout << "};\n";
#endif

    std::cerr << FakeAuthor() << std::endl;

    return EXIT_SUCCESS;
}

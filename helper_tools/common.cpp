#include "common.hpp"
#include <cstdint>                    // uint_fast64_t
#include <cctype>                     // tolower
#include <cstddef>                    // size_t
#include <cstdint>                    // int32_t
#include <cstdlib>                    // EXIT_FAILURE
#include <filesystem>                 // directory_iterator, path
#include <fstream>                    // ifstream, ofstream
#include <functional>                 // function
#include <iostream>                   // cerr
#include <memory>                     // unique_ptr
#include <regex>                      // regex_search
#include <sstream>                    // ostringstream
#include <string>                     // string, getline
#include <utility>                    // move
#include <vector>                     // vector
#include <cmark-gfm.h>                // cmark_render_plaintext
#include <gumbo.h>                    // GumboOutput, gumbo_parse
#include <poppler-document.h>         // poppler::document
#include <poppler-page.h>             // poppler::page
#include "../main_program/Auto.h"     // The 'Auto' macro

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

namespace fs = std::filesystem;

extern ofstream logfile;

string GetPlainText_PDF(fs::path const &path)
{
    logfile << "++++ Converting input file to plain text (in memory): " << path.string() << endl;

    using poppler::document;
    using poppler::page;

    unique_ptr<document> doc( document::load_from_file(path.string()) );

    if ( nullptr == doc ) return {};

    int const num_pages = doc->pages();  /* Yes, pages() returns an int */
    string retval;
    for ( int i = 0; i < num_pages; ++i )
    {
        unique_ptr<page> p(  doc->create_page(i)  );
        if ( p ) retval += p->text().to_latin1();
    }

    string const newfilename = path.string() + ".converted_to_txt";
    ofstream f(newfilename);
    if ( f )
    {
        f << retval;
    }
    else
    {
        logfile << "------------------- Failed to create pre-tokens file: " << newfilename << endl;
    }

    return retval;
}

string GetPlainText_HTML(fs::path const &path)
{
    logfile << "++++ Converting HTML to plain text: " << path.string() << endl;

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
    if ( nullptr == output ) return {};
    Auto( gumbo_destroy_output(&kGumboDefaultOptions, output) );

    ostringstream text_out;

    std::function<void(const GumboNode *)> extract_text;
    extract_text = [&](const GumboNode *node)
      {
        if ( node->type == GUMBO_NODE_TEXT )
        {
            text_out << node->v.text.text << ' ';
        }
        else if ( node->type == GUMBO_NODE_ELEMENT )
        {
            GumboTag tag = node->v.element.tag;

            // Skip non-visible content
            if (tag == GUMBO_TAG_SCRIPT ||
                tag == GUMBO_TAG_STYLE ||
                tag == GUMBO_TAG_HEAD ||
                tag == GUMBO_TAG_META ||
                tag == GUMBO_TAG_NOSCRIPT)
            {
                return;
            }

            GumboVector const *const children = &node->v.element.children;
            for ( unsigned i = 0u; i < children->length; ++i)
            {
                extract_text(static_cast<GumboNode *>(children->data[i]));
            }

            // Add a newline after block-level elements to improve formatting
            if (tag == GUMBO_TAG_P ||
                tag == GUMBO_TAG_DIV ||
                tag == GUMBO_TAG_BR ||
                tag == GUMBO_TAG_SECTION ||
                tag == GUMBO_TAG_ARTICLE ||
                tag == GUMBO_TAG_H1 || tag == GUMBO_TAG_H2 || tag == GUMBO_TAG_H3 ||
                tag == GUMBO_TAG_H4 || tag == GUMBO_TAG_H5 || tag == GUMBO_TAG_H6)
            {
                text_out << '\n';
            }
        }
        else if ( node->type == GUMBO_NODE_WHITESPACE )
        {
            text_out << ' ';
        }
      };

    extract_text(output->root);

    string const result = text_out.str();

    string const newfilename = path.string() + ".converted_to_txt";
    ofstream out(newfilename);
    if (out)
    {
        out << result;
    }
    else
    {
        logfile << "-------------------- Failed to write pre-token output: " << newfilename << endl;
    }

    return result;
}

string GetPlainText_MD(fs::path const &path)
{
    logfile << "++++ Converting Markdown to plain text via cmark: " << path.string() << endl;

    ifstream file(path);
    if ( !file )
    {
        logfile << "-------------------- Failed to open file: " << path << endl;
        return {};
    }

    ostringstream buffer;
    buffer << file.rdbuf();
    string markdown = std::move(buffer).str();

    // Convert to CommonMark AST
    cmark_node *doc = cmark_parse_document(markdown.c_str(), markdown.length(), CMARK_OPT_DEFAULT);
    if ( nullptr == doc )
    {
        logfile << "-------------------- Failed to parse Markdown document\n";
        return {};
    }
    Auto( cmark_node_free(doc) );

    // Render as plain text
    string retval;
    char const *const plain = cmark_render_plaintext(doc, CMARK_OPT_DEFAULT, 80);
    if ( nullptr == plain )
    {
        logfile << "-------------------- Failed to render Markdown as plain text\n";
        return {};
    }
    Auto( std::free( const_cast<char*>(plain) ) );
    retval = plain;

    string const newfilename = path.string() + ".converted_to_txt";
    ofstream out(newfilename);
    if ( out )
    {
        out << retval;
    }
    else
    {
        logfile << "-------------------- Failed to write pre-token output: " << newfilename << endl;
    }

    return retval;
}

extern void IterateThroughFolder(
    std::filesystem::path const &where,
    std::function<bool(std::filesystem::path const &, std::string_view)> const &callback )
{
    for ( auto const &entry : fs::directory_iterator(where) )
    {
        if ( false == entry.is_regular_file() ) continue;

        fs::path const path = entry.path();

        string text;

        if ( ".tokens" == path.extension() ) continue;
        else if ( ".converted_to_txt" == path.extension() ) continue;
        else if ( ".txt" == path.extension() )
        {
            ifstream file(path);

            if ( ! file )
            {
                logfile << "-------------------- Failed to open input file: " << path << endl;
                continue;
            }

            ostringstream buffer;
            buffer << file.rdbuf();
            text = std::move(buffer).str();
        }
        else if ( ".pdf" == path.extension() )
        {
            text = GetPlainText_PDF(path);
        }
        else if ( (".html" == path.extension()) || (".htm" == path.extension()) )
        {
            text = GetPlainText_HTML(path);
        }
        else if ( ".md" == path.extension() )
        {
            text = GetPlainText_MD(path);
        }
        else
        {
            logfile << "-------------------- Unrecognised input file type: " << path << endl;
            continue;
        }

        if ( text.empty() )
        {
            logfile << "-------------------- Input file was empty after conversion to plain text: " << path << endl;
            continue;
        }

        callback(path, text);
    }
}

string ExtractTitleFromFileHTML(string const &filename)
{
    ifstream f(filename);
    if ( false == f.is_open() )
    {
        logfile << "Failed to open: " << filename << endl;
        std::abort();
    }
    ostringstream oss;
    oss << f.rdbuf();
    string s = std::move(oss).str();
	GumboOutput *const output = gumbo_parse(s.c_str());
    if ( nullptr == output ) return {};
    Auto(  gumbo_destroy_output(&kGumboDefaultOptions, output)  );

    std::function<GumboNode const *(GumboNode const *)> FindTitleNode =
        [&FindTitleNode](GumboNode const *const node) -> GumboNode const*
        {
            if ( GUMBO_NODE_ELEMENT != node->type ) return nullptr;

            if ( GUMBO_TAG_TITLE == node->v.element.tag )
            {
                if ( node->v.element.children.length > 0 )
                {
                    GumboNode const *const text_node = static_cast<GumboNode*>(node->v.element.children.data[0]);
                    if ( GUMBO_NODE_TEXT == text_node->type ) return text_node;
                }
                return nullptr;
            }

            for ( unsigned i = 0u; i < node->v.element.children.length; ++i )
            {
                GumboNode const *const child = static_cast<GumboNode*>(node->v.element.children.data[i]);
                GumboNode const *const result = FindTitleNode(child);
                if ( result ) return result;
            }

            return nullptr;
        };

	GumboNode const *const title_node = FindTitleNode(output->root);
	string result;
	if ( title_node && (GUMBO_NODE_TEXT == title_node->type) )
	{
		result = title_node->v.text.text;
	}

	return result;
}

string ExtractTitleFromFilePDF(string const &filename)
{
    std::unique_ptr<poppler::document> doc( poppler::document::load_from_file(filename) );
    if ( !doc ) return {};
    poppler::ustring us = doc->info_key("Title");
    if ( us.empty() ) return {};
    std::string const s = us.to_latin1();
    if ( s.empty() ) return {};
    return s;
}

static std::string Trim(std::string const &s)
{
    size_t const start = s.find_first_not_of(" \t\n\r\"'`.,:;!?()[]{}<>");
    if ( start == std::string::npos ) return {};

    size_t const end = s.find_last_not_of(" \t\n\r\"'`.,:;!?()[]{}<>");

    return s.substr(start, end - start + 1);
}

static std::string ExtractAuthor(std::string const &s)
{
    std::regex r(R"(author[s]{0,1}:\s*([^\s\n\r\t\(:<]*)\s([^\s\n\r\t\(:<]*))", std::regex::icase);
    std::smatch match;

    if ( std::regex_search(s, match, r) && (match.size() > 2u) )
    {
        string s = match[1].str() + ' ' + match[2].str();
        string st = Trim(s);
        //std::cerr << st << std::endl;
        return st;
    }

    std::regex r2(R"(Reply-to:\s*([^\s\n\r\t\(:<]*)\s([^\s\n\r\t\(:<]*))", std::regex::icase);
    if ( std::regex_search(s, match, r2) && (match.size() > 2u) )
    {
        string s = match[1].str() + ' ' + match[2].str();
        string st = Trim(s);
        //std::cerr << st << std::endl;
        return st;
    }

    return {};
}

std::string ExtractAuthorFromFileHTML(std::string const &filename)
{
    string s = GetPlainText_HTML(filename);
    return ExtractAuthor(s);
}

std::string ExtractAuthorFromFilePDF (std::string const &filename)
{
    string s = GetPlainText_PDF(filename);
    return ExtractAuthor(s);
}

void ReplaceHtmlNumericEscapesInPlace(std::string &text)
{
    std::regex numeric_escape(R"(&#(x?)([0-9a-fA-F]+);)", std::regex::icase);
    std::smatch match;

    std::string result;
    std::string::const_iterator search_start_iter = text.cbegin();

    while (std::regex_search(search_start_iter, text.cend(), match, numeric_escape))
    {
        // Append text before match
        result.append(search_start_iter, match.prefix().second);

        bool is_hex = !match[1].str().empty();
        std::string number_str = match[2].str();

        int codepoint = 0;
        std::istringstream iss(number_str);
        if (is_hex)
            iss >> std::hex >> codepoint;
        else
            iss >> std::dec >> codepoint;

        std::stringstream ss;
        if (codepoint <= 0xFFFF)
            ss << "\\u" << std::setfill('0') << std::setw(4) << std::uppercase << std::hex << codepoint;
        else
            ss << "\\U" << std::setfill('0') << std::setw(8) << std::uppercase << std::hex << codepoint;

        result.append(ss.str());

        search_start_iter = match.suffix().first;
    }

    // Append remaining text
    result.append(search_start_iter, text.cend());

    text.swap(result);
}

void ReplaceInPlace(std::string &text, std::string_view const target, std::string_view replacement)
{
    size_t pos = 0u;
    while ( (pos = text.find(target, pos)) != std::string::npos )
    {
        text.replace(pos, target.length(), replacement);
        pos += replacement.length(); // move past the replacement
    }
}

void TrimWhitespace(std::string &s)
{
    const std::string whitespace = " \t\n\r\f\v";

    size_t start = s.find_first_not_of(whitespace);
    if (start == std::string::npos)
    {
        s.clear();
        return;
    }

    size_t end = s.find_last_not_of(whitespace);

    // Erase leading whitespace first
    s.erase(0, start);

    // Erase trailing whitespace after adjusting for leading erase
    s.erase(end - start + 1);
}

std::vector<std::string_view> SplitByNewLines(std::string_view const sv)
{
    std::vector<std::string_view> result;
    size_t start = 0u;

    for (; /* ever */;)
    {
        size_t pos = sv.find('\n', start);
        if ( -1 == pos )
        {
            // Last line or no newline found
            if ( start < sv.size() ) result.emplace_back(sv.substr(start));
            break;
        }
        // Extract substring [start, pos)
        result.emplace_back( sv.substr(start, pos - start) );
        start = pos + 1u;
    }

    return result;
}

void EscapeNonAscii(std::string &str, bool escape_wide)
{
    auto decode_utf8_char = [](std::string const &s, size_t pos, uint32_t &codepoint) -> size_t
    {
        unsigned char c = static_cast<unsigned char>(s[pos]);
        if (c < 0x80)
        {
            codepoint = c;
            return 1;
        }
        else if ((c & 0xE0) == 0xC0 && pos + 1 < s.size())
        {
            codepoint = ((c & 0x1F) << 6) | (static_cast<unsigned char>(s[pos+1]) & 0x3F);
            return 2;
        }
        else if ((c & 0xF0) == 0xE0 && pos + 2 < s.size())
        {
            codepoint = ((c & 0x0F) << 12) | ((static_cast<unsigned char>(s[pos+1]) & 0x3F) << 6) | (static_cast<unsigned char>(s[pos+2]) & 0x3F);
            return 3;
        }
        else if ((c & 0xF8) == 0xF0 && pos + 3 < s.size())
        {
            codepoint = ((c & 0x07) << 18) | ((static_cast<unsigned char>(s[pos+1]) & 0x3F) << 12) | ((static_cast<unsigned char>(s[pos+2]) & 0x3F) << 6) | (static_cast<unsigned char>(s[pos+3]) & 0x3F);
            return 4;
        }
        // Invalid UTF-8 or truncated sequence: treat byte as codepoint
        codepoint = c;
        return 1;
    };

    std::ostringstream oss;
    size_t i = 0;

    while ( i < str.size() )
    {
        uint32_t cp = 0;
        size_t len = decode_utf8_char(str, i, cp);

        if ( cp < 0x80 )
        {
            oss << static_cast<char>(cp);
        }
        else if (escape_wide)
        {
            if ( cp <= 0xFFFF )
            {
                oss << "\\u"
                    << std::uppercase << std::hex << std::setw(4) << std::setfill('0')
                    << cp;
            }
            else
            {
                oss << "\\U"
                    << std::uppercase << std::hex << std::setw(8) << std::setfill('0')
                    << cp;
            }
        }
        else
        {
            // Encode codepoint as UTF-8 bytes and output each byte as \xHH
            if (cp <= 0x7F)
            {
                oss << "\\x" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << cp;
            }
            else if (cp <= 0x7FF)
            {
                char b1 = 0xC0 | (cp >> 6);
                char b2 = 0x80 | (cp & 0x3F);
                oss << "\\x" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (unsigned char)b1;
                oss << "\\x" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (unsigned char)b2;
            }
            else if (cp <= 0xFFFF)
            {
                char b1 = 0xE0 | (cp >> 12);
                char b2 = 0x80 | ((cp >> 6) & 0x3F);
                char b3 = 0x80 | (cp & 0x3F);
                oss << "\\x" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (unsigned char)b1;
                oss << "\\x" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (unsigned char)b2;
                oss << "\\x" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (unsigned char)b3;
            }
            else
            {
                char b1 = 0xF0 | (cp >> 18);
                char b2 = 0x80 | ((cp >> 12) & 0x3F);
                char b3 = 0x80 | ((cp >> 6) & 0x3F);
                char b4 = 0x80 | (cp & 0x3F);
                oss << "\\x" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (unsigned char)b1;
                oss << "\\x" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (unsigned char)b2;
                oss << "\\x" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (unsigned char)b3;
                oss << "\\x" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (unsigned char)b4;
            }
        }

        i += len;
    }

    str = std::move(oss).str();
}

void Erase(std::string &str, std::string_view const sv)
{
    size_t pos = 0u;

    while ( -1 != (pos = str.find(sv, pos)) )
    {
        str.erase( pos, sv.length() );
        // Don't increment pos — content shifted left, might be repeated
    }
}

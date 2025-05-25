#include "semantic.hpp"
#include <fstream>
#include <filesystem>
#include <sstream>                       // ostringstream
#include <iostream>         //////// ======== REMOVE THIS - cout, endl

namespace fs = std::filesystem;

using std::string, std::string_view;

void SemanticSearcher::Init( std::function<void(unsigned,unsigned)> SetProgress ) noexcept(false)
{
    SetProgress(0u, 100u);

    this->db.emplace( Xapian::InMemory::open() );

    Xapian::TermGenerator indexer;
    Xapian::Stem stemmer("en");
    indexer.set_stemmer(stemmer);

    size_t count = 0u;

    for ( auto const &e : fs::directory_iterator("./paperfiles/papers/") )
    {
        bool is_good = false;
        if ( ".txt" == e.path().extension() ) is_good = true;
        if ( ".pretokens" == e.path().extension() ) is_good = true;
        if ( false == is_good ) continue;
        ++count;
    }

    size_t i = 0u;

    for ( auto const &e : fs::directory_iterator("./paperfiles/papers/") )
    {
        bool is_good = false;
        if ( ".txt" == e.path().extension() ) is_good = true;
        if ( ".pretokens" == e.path().extension() ) is_good = true;
        if ( false == is_good ) continue;

        string filename = e.path().string();

        std::ifstream file(filename);
        if ( !file ) throw "Failed to open plain text file";

        std::ostringstream ss;
        ss << file.rdbuf();
        string const content = std::move(ss).str();

        Xapian::Document doc;
        indexer.set_document(doc);
        indexer.index_text(content);
        doc.set_data(filename);  // Store original filename as metadata
        db->add_document(doc);

        std::cout << "Indexed: " << filename << "\n";

        SetProgress(++i, count);
    }
}

void SemanticSearcher::Search(string_view const query_string)
{
    Xapian::QueryParser parser;
    parser.set_stemmer(Xapian::Stem("en"));
    parser.set_database( *db );
    parser.set_default_op(Xapian::Query::OP_AND);

    Xapian::Query query = parser.parse_query( string(query_string) );
    Xapian::Enquire enquire( *db );
    enquire.set_query(query);

    Xapian::MSet matches = enquire.get_mset(0, 20);
    std::cout << "\nTop matches for: \"" << query_string << "\"\n";

    for ( auto it = matches.begin(); it != matches.end(); ++it )
    {
        std::cout << "- Score: " << it.get_weight()
                  << " | File: " << it.get_document().get_data() << "\n";
    }
}

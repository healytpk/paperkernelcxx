#include <cstdlib>              // EXIT_FAILURE
#include <iostream>             // ----------------------------- remove this -------------------- just for debug
#include <filesystem>           // path
#include <fstream>              // ofstream
#include <fcntl.h>              // open
#include <xapian.h>             // WritableDatabase
#include "common.hpp"           // IterateThroughFolder

std::ofstream logfile;

void XapianIndexDirectory(std::filesystem::path const &whereZZZ, Xapian::WritableDatabase &db)
{
    auto mylambda = [&db](std::filesystem::path const &where, std::string_view const content) -> bool
      {
        std::string s(content);
        Xapian::Document doc;
        doc.set_data(s);

        Xapian::TermGenerator term_gen;
        term_gen.set_stemmer(Xapian::Stem("en"));
        term_gen.set_document(doc);
        term_gen.index_text(s);

        // Store file name as a unique identifier
        doc.add_value(  0, where.filename().string()  );

        // Add document to the database
        db.add_document(doc);

        std::cout << "Xapian indexed file: " << where.filename().string() << std::endl;

        return true;
      };

    IterateThroughFolder(whereZZZ, mylambda);
}

int main(void)
{
/*
    int const fd = open(, O_RDWR | O_CREAT, 0644);
    if ( -1 == fd )
    {
        std::cout << "Failed to open output file.\n";
        return EXIT_FAILURE;
    }
*/
    Xapian::WritableDatabase db("./database_xapian_multiple_files", Xapian::DB_CREATE|Xapian::DB_BACKEND_GLASS|Xapian::DBCOMPACT_SINGLE_FILE );
    XapianIndexDirectory("../papers/", db);
}

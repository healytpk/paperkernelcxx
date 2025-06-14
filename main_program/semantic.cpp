#include "semantic.hpp"
#include <cstdint>                  // intptr_t, uint32_t
#include <fstream>
#include <filesystem>
#include <sstream>                       // ostringstream
#include <iostream>         //////// ======== REMOVE THIS - cout, endl
#include <fcntl.h>                  // open, _O_RDWR

#ifdef _WIN32
#    include <Windows.h>            // CreateFileA
#    include <io.h>                 // _open_osfhandle
#elif __APPLE__
#    include <mach-o/dyld.h>        // _NSGetExecutablePath
#else

#    include <unistd.h>             // readlink
#endif

namespace fs = std::filesystem;

using std::string, std::string_view;

static std::filesystem::path GetExecutableDirectory(void)  // ----------------------- DOUBLE AND TRIPLE CHECK THIS FUNCTION! revisit - fix
{
    static char path[1024u];
    path[0] = '\0';

#if defined(_WIN32) || defined(_WIN64)
    DWORD const len = ::GetModuleFileNameA(nullptr, path, sizeof(path));
    if ( (0u == len) || (len >= sizeof(path)) ) path[0] = '\0';
#elif defined(__APPLE__)
    std::uint32_t size = sizeof(path);
    if ( 0 != ::_NSGetExecutablePath(path, &size) ) path[0] = '\0';
#else
    ssize_t const len = ::readlink("/proc/self/exe", path, sizeof(path) - 1u); // does not null-terminate
    if ( len <= 0 ) path[  0] = '\0';
    /********/ else path[len] = '\0'; // need to add a terminator
#endif

    if ( '\0' == path[0] ) return {};

    return std::filesystem::path(path).parent_path(); // Extract directory
}

void SemanticSearcher::Init( std::function<void(unsigned,unsigned)> SetProgress ) noexcept(false)
{
    SetProgress(0u, 100u);

    std::string const str = (GetExecutableDirectory() / "database_xapian_for_all_cxx_papers").string();
    char const *const dbfile = str.c_str();

#if defined(_WIN32) || defined(_WIN64)
    HANDLE const hFile = CreateFileA(
        dbfile,                               // File name
        GENERIC_READ,                         // Read/write access
        FILE_SHARE_READ,                      // No sharing
        nullptr,                              // Default security
        OPEN_EXISTING,                        // Open existing or create new
        FILE_ATTRIBUTE_NORMAL,                // Normal file attributes
        nullptr                               // No template file
    );

    if ( INVALID_HANDLE_VALUE == hFile ) throw std::runtime_error("Failed to open Xapian database file");

    this->fd = _open_osfhandle( reinterpret_cast<std::intptr_t>(hFile), _O_RDONLY );
    if ( -1 == this->fd ) throw std::runtime_error("Failed to get a file descriptor from a HANDLE on MS-Windows");
#else
    this->fd = open(dbfile, O_RDWR);
    if ( -1 == this->fd ) throw std::runtime_error("Failed to open Xapian database file");
#endif

    this->db.emplace( this->fd, Xapian::DB_OPEN|Xapian::DB_BACKEND_GLASS );
    SetProgress(100u, 100u);
}

void SemanticSearcher::Search(string_view const query_string, std::function<void(std::string_view)> callback)
{
    Xapian::QueryParser query_parser;
    query_parser.set_database( *db );
    query_parser.set_stemmer(Xapian::Stem("en")); // Optional: for stemming
    query_parser.set_stemming_strategy(Xapian::QueryParser::STEM_SOME);
    Xapian::Query query = query_parser.parse_query( string(query_string) );

    Xapian::Enquire enquire( *db );
    enquire.set_query(query);
    Xapian::MSet matches = enquire.get_mset(0, 10); // Get top 10 results

    for ( Xapian::MSetIterator it = matches.begin(); it != matches.end(); ++it )
    {
        Xapian::Document doc = it.get_document();
        std::cout << "Rank " << *it << ": " << doc.get_value(0) << std::endl;
        callback( doc.get_value(0) );
    }
}

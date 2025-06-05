#include "semantic.hpp"
#include <cstdint>                  // intptr_t, uint32_t
#include <fstream>
#include <filesystem>
#include <sstream>                       // ostringstream
#include <iostream>         //////// ======== REMOVE THIS - cout, endl

#ifdef _WIN32
#    include <Windows.h>            // CreateFileA
#elif __APPLE__
#    include <mach-o/dyld.h>        // _NSGetExecutablePath
#else
#    include <fcntl.h>              // open
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

    std::string const str = GetExecutableDirectory() / "database_xapian_for_all_cxx_papers";
    char const *const dbfile = str.c_str();

#ifdef _WIN32
    HANDLE const hFile = CreateFileA(
        dbfile,                               // File name
        GENERIC_READ | GENERIC_WRITE,         // Read/write access
        0,                                    // No sharing
        NULL,                                 // Default security
        OPEN_EXISTING,                        // Open existing or create new
        FILE_ATTRIBUTE_NORMAL,                // Normal file attributes
        NULL                                  // No template file
    );

    if ( INVALID_HANDLE_VALUE == hFile ) throw std::runtime_error("Failed to open Xapian database file");

    this->fd = _open_osfhandle( reinterpret_cast<std::intptr_t>(hFile), _O_RDWR );
    if ( -1 == this->fd ) throw std::runtime_error("Failed to get a file descriptor from a HANDLE on MS-Windows");
#else
    this->fd = open(dbfile, O_RDWR);
    if ( -1 == this->fd ) throw std::runtime_error("Failed to open Xapian database file");
#endif

    Xapian::Database db( this->fd, Xapian::DB_OPEN|Xapian::DB_BACKEND_GLASS );
    SetProgress(100u, 100u);
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

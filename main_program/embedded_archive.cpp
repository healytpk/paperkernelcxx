#include "embedded_archive.hpp"
#include <cassert>                             // assert
#include <cstring>                             // strcmp
#include <stdexcept>                           // runtime_error
#include <string>                              // string
#include <archive.h>                           // struct archive
#include <archive_entry.h>                     // struct archive_entry
#include "incbin.h"                            // INCBIN
#include "Auto.h"                              // The 'Auto' macro

#include <iostream>          // ----------- remove THIS -----------------------------------------------------------

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    INCBIN(_archive, "../all_cxx_papers.tar.xz");
#elif __APPLE__
    INCBIN(_archive, "../../all_cxx_papers.tar.xz");
#else
    INCBIN(_archive, "../../../all_cxx_papers.tar.lz4");
#endif

using std::runtime_error, std::string;

string ArchiveGetFile(char const *const arg_filename) noexcept
{
    assert( nullptr != arg_filename );

    std::cout << "----------    g_archiveSize = " << g_archiveSize << std::endl;
    struct archive *const a = archive_read_new();
    if ( nullptr == a ) return {};
    Auto( archive_read_free(a) );

  //archive_read_support_filter_xz (a);  // Enable XZ  decompression
    archive_read_support_filter_lz4(a);  // Enable LZ4 decompression
    archive_read_support_format_tar(a);  // Enable TAR format
    if ( ARCHIVE_OK != archive_read_open_memory(a, g_archiveData, g_archiveSize) ) return {};

    try
    {
      //string filename("./");
        string filename;
        filename += arg_filename;
        struct archive_entry *entry = nullptr;
        for ( ; ARCHIVE_OK == archive_read_next_header(a, &entry); archive_read_data_skip(a) )
        {
            char const *const archive_filename = archive_entry_pathname(entry);
          //std::cout << " ---------- " << archive_filename << std::endl;
            if ( 0 != std::strcmp(archive_filename, filename.c_str()) ) continue;
            auto const entry_size = archive_entry_size(entry);
            std::cout << "Archive entry size: " << entry_size << std::endl;
            if ( entry_size <= 0 ) return {};
            string buffer(entry_size, '\0');
            if ( archive_read_data(a, buffer.data(), entry_size) < 0 ) return {};
            return buffer;
        }
    }
    catch(...){}

    return {};
}

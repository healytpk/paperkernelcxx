#include "embedded_archive.hpp"
#include <cassert>                             // assert
#include <cstdlib>                             // abort
#include <cstring>                             // strcmp
#include <stdexcept>                           // runtime_error
#include <string>                              // string
#include <archive.h>                           // struct archive
#include <archive_entry.h>                     // struct archive_entry
#ifdef PAPERKERNEL_INDIVIDUAL_COMPRESSION
#    include <zstd.h>                          // ZSTD_decompress, ZSTD_getFrameContentSize
#endif
#include "incbin.h"                            // INCBIN
#include "Auto.h"                              // The 'Auto' macro

#include <iostream>          // ----------- remove THIS -----------------------------------------------------------

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#include <cstddef>          // size_t
#include <Windows.h>        // GetModuleHandle, FindResource
#include "resource.h"       // IDR_PAPERKERNELCXX_EMBEDDED_ARCHIVE

char unsigned const *g_archiveData = nullptr;
std::size_t g_archiveSize = 0u;

static void LoadEmbeddedArchiveFromExectuableResources(void) noexcept
{
    HMODULE const hModule = GetModuleHandle(nullptr);

    HRSRC const hResInfo = FindResource(hModule, MAKEINTRESOURCE(IDR_PAPERKERNELCXX_EMBEDDED_ARCHIVE), RT_RCDATA);
    if ( !hResInfo ) return;

    HGLOBAL const hResData = LoadResource(hModule, hResInfo);
    if ( !hResData ) return;

    g_archiveSize = SizeofResource(hModule, hResInfo);
    g_archiveData = static_cast<char unsigned const*>( LockResource(hResData) );
}

#elif __APPLE__
#   ifdef PAPERKERNEL_INDIVIDUAL_COMPRESSION
       INCBIN(_archive, "../../all_cxx_papers_individual_zst.tar");
#   else
       INCBIN(_archive, "../../all_cxx_papers.tar.zst");
#   endif
#else
#   ifdef PAPERKERNEL_INDIVIDUAL_COMPRESSION
       INCBIN(_archive, "../../../all_cxx_papers_individual_zst.tar");
#   else
       INCBIN(_archive, "../../../all_cxx_papers.tar.zst");
#   endif
#endif

using std::runtime_error, std::string;

string ArchiveGetFile(char const *const arg_filename) noexcept
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    static int const n = ( LoadEmbeddedArchiveFromExectuableResources(), 666 );
    assert(      0u != g_archiveSize );
    assert( nullptr != g_archiveData );
    if ( 0u == g_archiveSize ) std::abort();    // if NDEBUG
#endif

    assert( nullptr != arg_filename );

    try
    {
        std::cout << "----------    g_archiveSize = " << g_archiveSize << std::endl;
        struct archive *const a = archive_read_new();
        if ( nullptr == a ) return {};
        Auto( archive_read_free(a) );

#ifndef PAPERKERNEL_INDIVIDUAL_COMPRESSION
      //archive_read_support_filter_xz (a);   // Enable XZ  decompression
      //archive_read_support_filter_lz4(a);   // Enable LZ4 decompression
        archive_read_support_filter_zstd(a);  // Enable Zstd decompression
#endif

        archive_read_support_format_tar(a);   // Enable TAR format
        if ( ARCHIVE_OK != archive_read_open_memory(a, g_archiveData, g_archiveSize) ) return {};

        string filename(arg_filename);
        struct archive_entry *entry = nullptr;
        for ( ; ARCHIVE_OK == archive_read_next_header(a, &entry); archive_read_data_skip(a) )
        {
            char const *const archive_filename = archive_entry_pathname(entry);
            std::cout << " ---------- " << archive_filename << std::endl;
            if ( 0 != std::strcmp(archive_filename, filename.c_str()) ) continue;
            auto const entry_size = archive_entry_size(entry);
            std::cout << "Archive entry size: " << entry_size << std::endl;
            if ( entry_size <= 0 ) return {};
            string buffer(entry_size, '\0');
            if ( archive_read_data(a, buffer.data(), entry_size) < 0 ) return {};
#ifndef PAPERKERNEL_INDIVIDUAL_COMPRESSION
            return buffer;
#else
            // Decompress using zstd
            auto const decompressed_size = ZSTD_getFrameContentSize( buffer.data(), buffer.size() );
            if ( (decompressed_size==ZSTD_CONTENTSIZE_ERROR) || (decompressed_size==ZSTD_CONTENTSIZE_UNKNOWN) ) return {};
            string decompressed(decompressed_size, '\0');
            auto const res = ZSTD_decompress(decompressed.data(), decompressed.size(), buffer.data(), buffer.size());
            if ( ZSTD_isError(res) ) return {};
            return decompressed;
#endif
        }
    }
    catch(...){}

    return {};
}

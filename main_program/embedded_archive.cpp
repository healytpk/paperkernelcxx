#include "embedded_archive.hpp"
#include <cassert>                             // assert
#include <cstdlib>                             // abort
#include <cstring>                             // strcmp
#include <stdexcept>                           // runtime_error
#include <string>                              // string
#include <archive.h>                           // struct archive
#include <archive_entry.h>                     // struct archive_entry
#include "Auto.h"                              // The 'Auto' macro
#include "_Max.hpp"
#ifdef PAPERKERNEL_INDIVIDUAL_COMPRESSION
#    include <zstd.h>                          // ZSTD_decompress, ZSTD_getFrameContentSize
#    define PAPERKERNEL_ARCHIVE_FILENAME "all_cxx_papers_individual_zst.tar"
#else
#    define PAPERKERNEL_ARCHIVE_FILENAME "all_cxx_papers.tar.zst"
#endif
#ifdef PAPERKERNEL_EMBED_ARCHIVE
#    include "incbin.h"                        // INCBIN
#else
#    include <fstream>                         // ifstream
#    include <sstream>                         // stringstream
#endif

#include <iostream>          // ----------- remove THIS -----------------------------------------------------------

using std::runtime_error, std::string, std::string_view;

static string ArchiveGetFile_Common(std::string_view const arg_filename, std::string& extension, bool const prefix_only) noexcept;

#ifdef PAPERKERNEL_EMBED_ARCHIVE

#if defined(_WIN32) || defined(_WIN64)

#include <cstddef>          // size_t
#include <Windows.h>        // GetModuleHandle, FindResource
#include "resource.h"       // IDR_PAPERKERNELCXX_EMBEDDED_ARCHIVE

char unsigned const *g_archiveData = nullptr;
std::size_t g_archiveSize = 0u;

static void LoadEmbeddedArchiveFromExecutableResources(void) noexcept
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
    INCBIN(_archive, "../../" PAPERKERNEL_ARCHIVE_FILENAME);
#else
    INCBIN(_archive, "../../../" PAPERKERNEL_ARCHIVE_FILENAME);
#endif

using std::runtime_error, std::string, std::string_view;

string ArchiveGetFile(std::string_view const arg_filename, std::string &extension, bool const prefix_only) noexcept
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    static int const n = ( LoadEmbeddedArchiveFromExecutableResources(), 666 );
    assert(      0u != g_archiveSize );
    assert( nullptr != g_archiveData );
    if ( 0u == g_archiveSize ) std::abort();    // if NDEBUG
#endif

    return ArchiveGetFile_Common(arg_filename, extension, prefix_only);
}

#else // if PAPERKERNEL_EMBED_ARCHIVE not defined

char unsigned const *g_archiveData = nullptr;
std::size_t g_archiveSize = 0u;

using std::runtime_error, std::string, std::string_view;

static void LoadArchiveFileIntoMemory(void)
{
    static std::string g_archive_in_memory;

    std::ifstream f( PAPERKERNEL_ARCHIVE_FILENAME, std::ios::binary );
    if ( ! f )
    {
        std::cerr << "Failed to open archive file: " << g_archive_filename << std::endl;
        return;
    }

    std::stringstream ss;
    ss << std::move(f).rdbuf();
    g_archive_in_memory = std::move(ss).str();
    g_archiveSize = g_archive_in_memory.size();
    g_archiveData = static_cast<char unsigned*>(static_cast<void*>(g_archive_in_memory.data()));
}

string ArchiveGetFile(std::string_view const arg_filename, std::string &extension, bool const prefix_only) noexcept
{
#if defined(_WIN32) || defined(_WIN64)
    static int const n = ( LoadArchiveFileIntoMemory(), 666 );
    assert(      0u != g_archiveSize );
    assert( nullptr != g_archiveData );
    if ( 0u == g_archiveSize ) std::abort();    // if NDEBUG
#endif

    return ArchiveGetFile_Common(arg_filename, extension, prefix_only);
}

#endif // ifdef PAPERKERNEL_EMBED_ARCHIVE

static string ArchiveGetFile_Common(std::string_view const arg_filename, std::string &extension, bool const prefix_only = false) noexcept
{
    extension.clear();

    assert( false == arg_filename.empty() );

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

        struct archive_entry *entry = nullptr;
        for ( ; ARCHIVE_OK == archive_read_next_header(a, &entry); archive_read_data_skip(a) )
        {
            string_view const archive_filename = archive_entry_pathname(entry);
            std::cout << " ---------- " << archive_filename << std::endl;
            if ( prefix_only )
            {
                // If prefix_only is true, we only check the beginning of the filename
                if ( false == archive_filename.starts_with(arg_filename) ) continue;
                size_t const dot_pos = archive_filename.find('.');
                if ( (_Max != dot_pos) && ( (dot_pos + 1u) < archive_filename.size() ) )
                {
                    extension = std::string( archive_filename.substr(dot_pos + 1u) );
                }
            }
            else
            {
                // If prefix_only is false, we check the full filename
                if ( archive_filename != arg_filename ) continue;
            }
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

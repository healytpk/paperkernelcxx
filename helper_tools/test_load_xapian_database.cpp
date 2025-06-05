#include <cstdlib>              // EXIT_FAILURE
#include <cstring>
#include <iostream>             // ----------------------------- remove this -------------------- just for debug
#include <filesystem>           // path
#include <fstream>              // ofstream
#include <iostream>
#include <string>
#include <fcntl.h>              // open
#include <xapian.h>             // WritableDatabase
#include "../main_program/Auto.h"

#ifdef _WIN32
#    include <windows.h>
#else
#    include <sys/mman.h>
#    include <fcntl.h>
#    include <unistd.h>
#    include "../main_program/incbin.h"
#endif

INCBIN(_xapiandb, "../database_xapian_for_all_cxx_papers");

#define name_of_memfile "paperkernelcxx_XapianDB"

std::string GetFilesystemPathToXapianDatabaseInMemory(void) noexcept
{
#ifdef _WIN32

    HANDLE const hFile = ::CreateFile(name_of_memfile, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
    if ( INVALID_HANDLE_VALUE == hFile ) return {};
    Auto( ::CloseHandle(hFile) );
    DWORD bytesWritten = 0u;
    WriteFile(hFile, g_xapiandbData, g_xapiandbSize, &bytesWritten, NULL);
    if ( g_xapiandbSize != bytesWritten ) return {};
    return name_of_memfile;

#else

    int const fd = ::shm_open(name_of_memfile, O_CREAT | O_RDWR, 0666);
    if ( -1 == fd ) return {};

    if ( 0 != ::ftruncate(fd, g_xapiandbSize) )
    {
        ::close(fd);
        return {};
    }

    void *const mappedData = ::mmap(nullptr, g_xapiandbSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if ( MAP_FAILED == mappedData )
    {
        ::close(fd);
        return {};
    }

    std::memcpy(mappedData, g_xapiandbData, g_xapiandbSize);

    return "/dev/shm/" name_of_memfile;
    
#endif
}

int main(void)
{
#if 1
    std::string s = GetFilesystemPathToXapianDatabaseInMemory();
#elif 0
    std::string s = "../database_xapian_for_all_cxx_papers";
#else
    std::string s = "./database_xapian_multiple_files";
#endif

#if 0
    std::cout << "Xapian will load a database from: " << s << std::endl;
    Xapian::WritableDatabase db( s, Xapian::DB_OPEN|Xapian::DB_BACKEND_GLASS );
#else
    int const fd = open(s.c_str(), O_RDWR | O_CREAT, 0644);
    if ( -1 == fd )
    {
        std::cout << "Failed to open input file.\n";
        return EXIT_FAILURE;
    }
    Xapian::Database db( fd, Xapian::DB_OPEN|Xapian::DB_BACKEND_GLASS );
#endif
}

#!/bin/bash
echo OS Type: $OSTYPE

if grep -q "^LINUX PAPERKERNEL_INDIVIDUAL_COMPRESSION$" preprocessor_defines.txt; then
    ARCHIVE="all_cxx_papers_individual_zst.tar"
    URL="http://www.virjacode.com/downloads/all_cxx_papers_individual_zst.tar"
else
    ARCHIVE="all_cxx_papers.tar.zst"
    URL="http://www.virjacode.com/downloads/all_cxx_papers.tar.zst"
fi

if [ -e "../$ARCHIVE" ]; then
    echo "Tar archive of all C++ papers already exists"
else
    echo "Downloading tar archive of all C++ papers from virjacode.com"
    curl "$URL" -o "../$ARCHIVE"
    if [ ! -e "../$ARCHIVE" ]; then
        echo "Failed to retrieve all C++ papers from the internet."
        exit 1
    fi
fi

if [ -e "../database_xapian_for_all_cxx_papers" ]; then
    echo Xapian database for all papers already exists
else
    echo Downloading Xapian database for all papers from virjacode.com
    curl http://www.virjacode.com/downloads/database_xapian_for_all_cxx_papers -o ../database_xapian_for_all_cxx_papers
    if [ ! -e "../database_xapian_for_all_cxx_papers" ]; then
        echo "Failed to retrieve Xapian database for all papers from the internet."
        exit 1
    fi
fi

echo building Release configuration makefile
echo directory "LinuxRelease"
rm -dr "LinuxRelease"
mkdir "LinuxRelease"
cd "LinuxRelease"
cmake -DCMAKE_BUILD_TYPE:STRING=Release ../ -DCMAKE_CXX_STANDARD_LIBRARIES="-lwx_gtk3u_aui-`wx-config --version | cut -d '.' -f 1,2`" -DCMAKE_LDFLAGS="-Wl,-rpath,'$ORIGIN'"
cd ..

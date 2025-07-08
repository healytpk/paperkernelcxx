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

if [ ! -e "../xapian-core-1.4.29.tar.xz" ]; then
    echo Downloading libxapian from oligarchy.co.uk
    curl -o ../xapian-core-1.4.29.tar.xz https://oligarchy.co.uk/xapian/1.4.29/xapian-core-1.4.29.tar.xz
    if [ ! -e "../xapian-core-1.4.29.tar.xz" ]; then
        echo "Failed to retrieve libxapian source code from the internet."
        exit 1
    fi
fi

if [ ! -d "./xapian" ]; then
    tar xf ../xapian-core-1.4.29.tar.xz
    mv xapian-core-1.4.29 xapian
    cd xapian

    mkdir -p build_debug
    cd build_debug
    ../configure --enable-static=yes --enable-shared=no --enable-assertions --enable-log CPPFLAGS="-D_GLIBCXX_DEBUG" CXXFLAGS="-ggdb3"
    make -j `nproc`
    cd ../..
fi

echo building Debug configuration makefile
echo directory "LinuxDebug"
rm -dr "LinuxDebug"
mkdir "LinuxDebug"
cd "LinuxDebug"
cmake -DCMAKE_BUILD_TYPE:STRING=Debug ../ -DCMAKE_CXX_STANDARD_LIBRARIES="-lwx_gtk3u_aui-`wx-config --version | cut -d '.' -f 1,2`" -DCMAKE_LDFLAGS="-Wl,-rpath,'$ORIGIN'"
cd ..

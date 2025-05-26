#!/bin/bash
echo OS Type: $OSTYPE

if [ -e "../all_cxx_papers.tar.zst" ]; then
    echo Compressed tar archive of all C++ papers already exists
else
    echo Downloading compressed tar archive of all C++ papers from virjacode.com
    curl -o ../all_cxx_papers.tar.zst http://www.virjacode.com/downloads/all_cxx_papers.tar.zst
    if [ ! -e "../all_cxx_papers.tar.zst" ]; then
        echo "Failed to retrieve all C++ papers from the internet."
        exit
    fi
fi

if [ ! -e "../xapian-core-1.4.29.tar.xz" ]; then
    curl -o ../xapian-core-1.4.29.tar.xz https://oligarchy.co.uk/xapian/1.4.29/xapian-core-1.4.29.tar.xz
    if [ ! -e "../xapian-core-1.4.29.tar.xz" ]; then
        echo "Failed to retrieve libxapian source code from the internet."
        exit
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

    cd ../
    mkdir -p build_release
    cd build_release
    ../configure --enable-static=yes --enable-shared=no CPPFLAGS="-DNDEBUG" CXXFLAGS="-O3"
    make -j `nproc`
fi

echo building Debug configuration makefile
echo directory "LinuxDebug"
rm -dr "LinuxDebug"
mkdir "LinuxDebug"
cd "LinuxDebug"
cmake -DCMAKE_BUILD_TYPE:STRING=Debug ../ -DCMAKE_CXX_STANDARD_LIBRARIES="-lwx_gtk3u_aui-`wx-config --version | cut -d '.' -f 1,2`" -DCMAKE_LDFLAGS="-Wl,-rpath,'$ORIGIN'"
cd ..

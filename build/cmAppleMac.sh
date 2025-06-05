#!/bin/bash

if [ -e "../all_cxx_papers.tar.zst" ]; then
    echo Compressed tar archive of all C++ papers already exists
else
    echo Downloading compressed tar archive of all C++ papers from virjacode.com
    curl http://www.virjacode.com/downloads/all_cxx_papers.tar.zst -o ../all_cxx_papers.tar.zst
    if [ ! -e "../all_cxx_papers.tar.zst" ]; then
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

rm -dr Mac
mkdir Mac
cd Mac
cmake ../ -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_CXX_STANDARD_LIBRARIES="-lwx_osx_cocoau_aui-3.2"
cd ..

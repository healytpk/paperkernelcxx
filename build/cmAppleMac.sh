#!/bin/bash

if [ -e "../all_cxx_papers.tar.xz" ]; then
    echo Compressed tar archive of all C++ papers already exists
else
    echo Downloading compressed tar archive of all C++ papers from virjacode.com
    curl http://virjacode.com/downloads/all_cxx_papers.tar.xz -o ../all_cxx_papers.tar.xz
fi

rm -dr Mac
mkdir Mac
cd Mac
cmake ../ -DCMAKE_CXX_STANDARD_LIBRARIES="-lwx_osx_cocoau_aui-3.2"
cd ..

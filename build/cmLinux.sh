#!/bin/bash
echo OS Type: $OSTYPE

if [ -e "../../all_cxx_papers.tar.xz" ]; then
    echo Compressed tar archive of all C++ papers already exists
else
    echo Downloading compressed tar archive of all C++ papers from virjacode.com
    curl http://virjacode.com/downloads/all_cxx_papers.tar.xz -o ../../all_cxx_papers.tar.xz
fi

#-DCMAKE_CXX_COMPILER=clang++

# ----------------------------------
# build Debug configuration makefile
# ----------------------------------
echo building Debug configuration makefile
echo directory "LinuxDebug"
rm -dr "LinuxDebug"
mkdir "LinuxDebug"
cd "LinuxDebug"
#cmake -DCMAKE_BUILD_TYPE:STRING=Debug ../ -DCMAKE_CXX_STANDARD_LIBRARIES="-lwx_gtk3ud_aui-`wx-config --version | cut -d '.' -f 1,2`" -DCMAKE_LDFLAGS="-Wl,-rpath,'$ORIGIN'"
cmake -DCMAKE_BUILD_TYPE:STRING=Debug ../ -DCMAKE_CXX_STANDARD_LIBRARIES="-lwx_gtk3u_aui-`wx-config --version | cut -d '.' -f 1,2`" -DCMAKE_LDFLAGS="-Wl,-rpath,'$ORIGIN'"
cd ..

# ----------------------------------
# build Release configuration makefile
# ----------------------------------
echo building Release configuration makefile
echo directory "LinuxRelease"
rm -dr "LinuxRelease"
mkdir "LinuxRelease"
cd "LinuxRelease"
cmake -DCMAKE_BUILD_TYPE:STRING=Release ../ -DCMAKE_CXX_STANDARD_LIBRARIES="-lwx_gtk3u_aui-`wx-config --version | cut -d '.' -f 1,2`" -DCMAKE_LDFLAGS="-Wl,-rpath,'$ORIGIN'"
cd ..

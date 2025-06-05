rem @echo off

if exist "..\all_cxx_papers.tar.zst" (
    echo Compressed tar archive of all C++ papers already exists
) else (
    echo Downloading compressed tar archive of all C++ papers from virjacode.com
    curl -o ../all_cxx_papers.tar.zst http://www.virjacode.com/downloads/all_cxx_papers.tar.zst
)

if exist "..\database_xapian_for_all_cxx_papers" (
    echo Xapian database for all papers already exists
) else (
    echo Downloading Xapian database for all papers from virjacode.com
    curl -o ../database_xapian_for_all_cxx_papers http://www.virjacode.com/downloads/database_xapian_for_all_cxx_papers
)

rmdir /S /Q Win
mkdir Win
cd Win
cmake ../ -DCMAKE_BUILD_TYPE:STRING=Release -G "Visual Studio 17 2022" -A Win32 -DCMAKE_TOOLCHAIN_FILE=../../vcpkg/scripts/buildsystems/vcpkg.cmake
cd ..

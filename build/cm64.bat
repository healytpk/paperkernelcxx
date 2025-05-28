rem @echo off

if exist "..\..\all_cxx_papers.tar.zst" (
    echo Compressed tar archive of all C++ papers already exists
) else (
    echo Downloading compressed tar archive of all C++ papers from virjacode.com
    curl -o "..\..\all_cxx_papers.tar.zst" http://virjacode.com/downloads/all_cxx_papers.tar.zst
)

rmdir /S /Q Win
mkdir Win
cd Win
cmake ../ -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE=../../vcpkg/scripts/buildsystems/vcpkg.cmake
copy ..\main_program.sln64 .\main_program.sln
cd ..

rem @echo off

if exist "..\..\all_cxx_papers.tar.xz" (
    echo Compressed tar archive of all C++ papers already exists
) else (
    echo Downloading compressed tar archive of all C++ papers from virjacode.com
    curl -o "..\..\all_cxx_papers.tar.xz" http://virjacode.com/downloads/all_cxx_papers.tar.xz
)

rmdir /S /Q Win
mkdir Win
cd Win
cmake ../ -G "Visual Studio 17 2022" -A Win32
copy ..\wxModularHost.sln32 .\wxModularHost.sln
cd ..

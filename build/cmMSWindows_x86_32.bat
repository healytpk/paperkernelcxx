rem @echo off

set "INDIVIDUAL_COMPRESSION=0"
for /f "usebackq delims=" %%L in ("preprocessor_defines.txt") do (
    if "%%L"=="MSWIN PAPERKERNEL_INDIVIDUAL_COMPRESSION" set "INDIVIDUAL_COMPRESSION=1"
)

if "%INDIVIDUAL_COMPRESSION%"=="1" (
    set "ARCHIVE=all_cxx_papers_individual_zst.tar"
    set "URL=http://www.virjacode.com/downloads/all_cxx_papers_individual_zst.tar"
) else (
    set "ARCHIVE=all_cxx_papers.tar.zst"
    set "URL=http://www.virjacode.com/downloads/all_cxx_papers.tar.zst"
)

if exist "..\%ARCHIVE%" (
    echo Tar archive of all C++ papers already exists
) else (
    echo Downloading tar archive of all C++ papers from virjacode.com
    curl "%URL%" -o "..\%ARCHIVE%"
    if not exist "..\%ARCHIVE%" (
        echo Failed to retrieve all C++ papers from the internet.
        exit /b 1
    )
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

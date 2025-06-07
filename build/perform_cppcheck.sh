#!/bin/sh

if [ -d "./cppcheck-main" ]; then
    echo Cppcheck directory already exists.
else
    if [ -f "./cppcheck-main.zip" ]; then
        echo Cppcheck zip file already exists.
    else
        echo Downloading Cppcheck from the internet...
        curl -L https://github.com/danmar/cppcheck/archive/refs/heads/main.zip -o ./cppcheck-main.zip
        if [ ! -f "./cppcheck-main.zip" ]; then
            echo Failed to download Cppcheck zip file from the internet.
            exit 1
        fi
    fi

    unzip ./cppcheck-main.zip
fi

if [ ! -f ./cppcheck-main/cppcheck ]; then
    sudo apt install -y libpcre3-dev python3-pygments
    cd ./cppcheck-main/
    make MATCHCOMPILER=yes FILESDIR=/usr/share/cppcheck HAVE_RULES=yes CXXFLAGS="-O2 -DNDEBUG -Wall -Wno-sign-compare -Wno-unused-function" -j `nproc`
    cd ..
fi

if [ ! -f ./cppcheck-main/cppcheck ]; then
    echo Failed to build the Cppcheck executable file.
    exit 1
fi

echo We have a viable Cppcheck executable file to run! \:\-\)

rm -rf dir_cppcheck_work dir_cppcheck_html
mkdir -p dir_cppcheck_work dir_cppcheck_html
cd ./cppcheck-main/
./cppcheck --cppcheck-build-dir=../dir_cppcheck_work/ --inline-suppr --force --check-level=exhaustive \
           --library=std --library=boost --library=wxwidgets \
           --xml -v --std=c++26 --enable=all '--suppress=*:/usr/include/*' $(PREPROCESSOR_DEFINES) \
           ../../main_program/*.c ../../main_program/*.cpp 2> ../dir_cppcheck_work/err.xml
cd ./htmlreport/
./cppcheck-htmlreport --file=../../dir_cppcheck_work/err.xml --report-dir=../../dir_cppcheck_html/ --source-dir=../
cd ../../

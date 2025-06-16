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
mkdir -p dir_cppcheck_work/main_program/ dir_cppcheck_html/main_program/ \
         dir_cppcheck_work/summarise/    dir_cppcheck_html/summarise/ \
         dir_cppcheck_work/tree_maker/   dir_cppcheck_html/tree_maker/

cd ./cppcheck-main/

PREPROCESSOR_DEFINES="-D_GLIBC_DEBUG -D_GLIBCXX_DEBUG"
./cppcheck --cppcheck-build-dir=../dir_cppcheck_work/main_program/ --inline-suppr --force --check-level=exhaustive \
           --library=std --library=boost --library=wxwidgets \
           --xml -v --std=c++26 --enable=all '--suppress=*:/usr/include/*' ${PREPROCESSOR_DEFINES} \
           ../../main_program/*.c ../../main_program/*.cpp 2> ../dir_cppcheck_work/main_program/err.xml
cd ./htmlreport/
./cppcheck-htmlreport --file=../../dir_cppcheck_work/main_program/err.xml --report-dir=../../dir_cppcheck_html/main_program/ --source-dir=../
cd ../

PREPROCESSOR_DEFINES="-D_GLIBC_DEBUG -D_GLIBCXX_DEBUG -DPAPERKERNELCXX_MINIMAL_PAPER"
./cppcheck --cppcheck-build-dir=../dir_cppcheck_work/summarise/ --inline-suppr --force --check-level=exhaustive \
           --library=std \
           --xml -v --std=c++26 --enable=all '--suppress=*:/usr/include/*' ${PREPROCESSOR_DEFINES} \
           ../../helper_tools/summarise.cpp ../../main_program/paper.cpp 2> ../dir_cppcheck_work/summarise/err.xml
cd ./htmlreport/
./cppcheck-htmlreport --file=../../dir_cppcheck_work/summarise/err.xml --report-dir=../../dir_cppcheck_html/summarise/ --source-dir=../
cd ../

PREPROCESSOR_DEFINES="-D_GLIBC_DEBUG -D_GLIBCXX_DEBUG"
./cppcheck --cppcheck-build-dir=../dir_cppcheck_work/tree_maker/ --inline-suppr --force --check-level=exhaustive \
           --library=std \
           --xml -v --std=c++26 --enable=all '--suppress=*:/usr/include/*' ${PREPROCESSOR_DEFINES} \
           ../../helper_tools/tree_maker.cpp ../../helper_tools/common.cpp 2> ../dir_cppcheck_work/tree_maker/err.xml
cd ./htmlreport/
./cppcheck-htmlreport --file=../../dir_cppcheck_work/tree_maker/err.xml --report-dir=../../dir_cppcheck_html/tree_maker/ --source-dir=../
cd ../

cd ../    # and one last time at the end

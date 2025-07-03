#!/bin/sh

set -e

# Common compiler flags
CXX="g++ -std=c++23 -static-libstdc++ -static-libgcc -ggdb3 -D_GLIBC_DEBUG -D_GLIBCXX_DEBUG -L./lib/ -I./include/ -I/usr/include/poppler/cpp/"
LIB="-lcurl -lpoppler-cpp -lgumbo -lcmark-gfm -lssl -lcrypto -lxapian"

tree_maker()
{
    echo "Building tree_maker..."
    rm -f ./tree_maker
    $CXX -o tree_maker tree_maker.cpp common.cpp $LIB
    echo "Libraries needed by 'tree_maker':"
    readelf -a ./tree_maker | grep "(NEEDED)"
}

summarise()
{
    echo "Building summarise..."
    rm -f ./summarise
    $CXX -o summarise summarise.cpp ../main_program/paper.cpp $LIB -DPAPERKERNELCXX_MINIMAL_PAPER
    echo "Libraries needed by 'summarise':"
    readelf -a ./summarise | grep "(NEEDED)"
}

enumerate_authors()
{
    echo "Building enumerate_authors..."
    rm -f ./enumerate_authors
    $CXX -o enumerate_authors enumerate_authors.cpp common.cpp $LIB
    echo "Libraries needed by 'enumerate_authors':"
    readelf -a ./enumerate_authors | grep "(NEEDED)"
}

index_for_xapian()
{
    echo "Building index_for_xapian..."
    rm -f ./index_for_xapian
    $CXX -o index_for_xapian index_for_xapian.cpp common.cpp $LIB -g0 -U_GLIBC_DEBUG -U_GLIBCXX_DEBUG
    echo "Libraries needed by 'index_for_xapian':"
    readelf -a ./index_for_xapian | grep "(NEEDED)"
}

test_load_xapian_database()
{
    echo "Building test_load_xapian_database..."
    rm -f ./test_load_xapian_database
    $CXX -o test_load_xapian_database test_load_xapian_database.cpp $LIB -g0 -U_GLIBC_DEBUG -U_GLIBCXX_DEBUG
    echo "Libraries needed by 'test_load_xapian_database':"
    readelf -a ./test_load_xapian_database | grep "(NEEDED)"
}

tokenizer()
{
    echo "Building tokenizer..."
    rm -f ./tokenizer
    $CXX -o tokenizer tokenizer.cpp common.cpp -l:libllama.a -l:libggml.a -l:libggml-cpu.a -l:libggml-base.a -l:libgomp.a $LIB
    echo "Libraries needed by 'tokenizer':"
    readelf -a ./tokenizer | grep "(NEEDED)"
}

crawl()
{
    echo "Building crawl..."
    rm -f ./crawl
    $CXX -o crawl crawl.cpp common.cpp ../main_program/paper.cpp $LIB -DPAPERKERNELCXX_MINIMAL_PAPER
    echo "Libraries needed by 'crawl':"
    readelf -a ./crawl | grep "(NEEDED)"
}

equivalent_names()
{
    echo "Building equivalent_names..."
    rm -f ./equivalent_names
    $CXX -o equivalent_names equivalent_names.cpp common.cpp $LIB
    echo "Libraries needed by 'equivalent_names':"
    readelf -a ./equivalent_names | grep "(NEEDED)"
}

header()
{
    echo "Building header..."
    rm -f ./header
    $CXX -o header header.cpp -I../main_program/ `wx-config --cxxflags --libs` $LIB
    echo "Libraries needed by 'header':"
    readelf -a ./header | grep "(NEEDED)"
}

build_all()
{
    tree_maker
    summarise
    enumerate_authors
    index_for_xapian
    test_load_xapian_database
    tokenizer
    crawl
    equivalent_names
    header
}

# Entry point
if [ $# -eq 0 ]; then
    echo "No arguments provided. Must specify one or more of:"
    echo "    tree_maker"
    echo "    summarise"
    echo "    enumerate_authors"
    echo "    index_for_xapian"
    echo "    test_load_xapian_database"
    echo "    tokenizer"
    echo "    crawl"
    echo "    equivalent_names"
    echo "    header"
    echo "    all"
    exit 1
else
    for target in "$@"; do
        case "$target" in
            tree_maker)                tree_maker ;;
            summarise)                 summarise ;;
            enumerate_authors)         enumerate_authors ;;
            index_for_xapian)          index_for_xapian ;;
            test_load_xapian_database) test_load_xapian_database ;;
            tokenizer)                 tokenizer ;;
            crawl)                     crawl ;;
            equivalent_names)          equivalent_names ;;
            header)                    header ;;
            all)                       build_all ;;
            *)
                echo "Unknown target: $target"
                echo "Valid targets: tree_maker, summarise, enumerate_authors, index_for_xapian, test_load_xapian_database, tokenizer, crawl, equivalent_names, header, all"
                exit 1
                ;;
        esac
    done
fi

#!/bin/sh

set -e

# Common compiler flags
CXX="g++ -std=c++23 -static-libstdc++ -static-libgcc -ggdb3 -D_GLIBC_DEBUG -D_GLIBCXX_DEBUG -L./lib/ -I./include/ -I/usr/include/poppler/cpp/"
LIB="-lcurl -lpoppler-cpp -lgumbo -lcmark-gfm -lssl -lcrypto -lxapian"

tree_maker()
{
    ./tree_maker
}

summarise()
{
    ./summarise
}

enumerate_authors()
{
    ./enumerate_authors
}

index_for_xapian()
{
    ./index_for_xapian
    xapian-compact --single-file database_xapian_multiple_files ../database_xapian_for_all_cxx_papers
}

test_load_xapian_database()
{
    ./test_load_xapian_database
}

tokenizer()
{
    echo "This doesn't do anything -- it doesn't run the tokenizer binary."
}

crawl()
{
    ./crawl > names.txt
}

equivalent_names()
{
    if ! [ -f ./names.txt ]; then
        echo "You must run 'crawl' to create 'names.txt' before running 'equivalent_names'."
        exit 1
    fi

    cat ./names.txt | ./equivalent_names > header.hpp
}

header()
{
    if ! [ -f ./header.hpp ]; then
        echo "You must run 'equivalent_names' to create 'header.hpp' before running 'header'."
        exit 1
    fi

    ./header
}

run_all()
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
            all)                       run_all ;;
            *)
                echo "Unknown target: $target"
                echo "Valid targets: tree_maker, summarise, enumerate_authors, index_for_xapian, test_load_xapian_database, tokenizer, crawl, equivalent_names, header, all"
                exit 1
                ;;
        esac
    done
fi

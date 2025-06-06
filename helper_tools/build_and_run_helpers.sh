#!/bin/sh

rm index_for_xapian

g++ -o index_for_xapian -std=c++23               \
    -DNDEBUG -O3                                 \
    index_for_xapian.cpp common.cpp              \
    -static-libstdc++ -static-libgcc             \
    -I/usr/include/poppler/cpp/                  \
    -lpoppler-cpp -lgumbo -lcmark-gfm -lxapian

echo "Libraries needed by 'index_for_xapian': "
readelf -a ./index_for_xapian | grep "(NEEDED)"

./index_for_xapian
xapian-compact --single-file database_xapian_multiple_files ../database_xapian_for_all_cxx_papers

# Now let's see if we can open the single file database

rm test_load_xapian_database

g++ -o test_load_xapian_database -std=c++23      \
    -DNDEBUG -O3                                 \
    test_load_xapian_database.cpp                \
    -static-libstdc++ -static-libgcc             \
    -lxapian

echo "Libraries needed by 'test_load_xapian_database': "
readelf -a ./test_load_xapian_database | grep "(NEEDED)"

./test_load_xapian_database

exit 0

g++ -o tree_maker -std=c++23                 \
    tree_maker.cpp common.cpp                \
    -static-libstdc++ -static-libgcc         \
    -I/usr/include/poppler/cpp/              \
    -lpoppler-cpp -lgumbo -lcmark-gfm

echo "Libraries needed by 'tree_maker': "
readelf -a ./tree_maker | grep "(NEEDED)"
./tree_maker > ../main_program/paper_tree_contents.hpp

exit 0

g++ -o tokenizer -std=c++23                    \
    tokenizer.cpp common.cpp                   \
    -static-libstdc++ -static-libgcc           \
    -I/usr/include/poppler/cpp                 \
    -I../include/ -I../ggml/include/           \
    ../build/src/libllama.a                    \
    ../build/ggml/src/libggml.a                \
    ../build/ggml/src/libggml-cpu.a            \
    ../build/ggml/src/libggml-base.a           \
    /usr/lib/gcc/x86_64-linux-gnu/13/libgomp.a \
    -lgumbo -lpoppler-cpp -lcmark-gfm

echo
echo "Libraries needed by 'tokenizer': "
readelf -a ./tokenizer | grep "(NEEDED)"

#!/bin/sh

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

#!/bin/sh

g++ -o tree_maker                            \
    -x c++ tree_maker.cpp.STANDALONE -x none \
    -x c++ common.cpp.STANDALONE     -x none \
    -I /usr/include/poppler/cpp/             \
    -std=c++23                               \
    -static-libstdc++ -static-libgcc         \
    -lpoppler-cpp -lgumbo -lcmark-gfm

echo "Libraries needed by 'tree_maker': "
readelf -a ./tree_maker | grep "(NEEDED)"
./tree_maker > ./main_program/paper_tree_contents.hpp

exit 0

g++ -o tokenizer -x c++ tokenizer.cpp.STANDALONE common.cpp.STANDALONE -x none \
    -std=c++23 \
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

g++ -o prog *.cpp                              \
    -std=c++23                                 \
    -static-libstdc++ -static-libgcc           \
    `wx-config --libs` `wx-config --cxxflags`  \
    -I../include/ -I../ggml/include/           \
    ../build/src/libllama.a                    \
    ../build/ggml/src/libggml.a                \
    ../build/ggml/src/libggml-cpu.a            \
    ../build/ggml/src/libggml-base.a           \
    /usr/lib/x86_64-linux-gnu/libarchive.a     \
    /usr/lib/x86_64-linux-gnu/liblzma.a        \
    /usr/lib/gcc/x86_64-linux-gnu/13/libgomp.a \
    -lxapian

echo
echo "Libraries needed by 'prog':"
readelf -a ./prog | grep "(NEEDED)"

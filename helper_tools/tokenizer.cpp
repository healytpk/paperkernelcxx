#include <cstddef>                    // size_t
#include <cstdint>                    // int32_t
#include <cstdlib>                    // EXIT_FAILURE
#include <filesystem>                 // directory_iterator, path
#include <fstream>                    // ifstream, ofstream
#include <functional>                 // function
#include <iostream>                   // cerr
#include <memory>                     // unique_ptr
#include <sstream>                    // ostringstream
#include <string>                     // string, getline
#include <utility>                    // move
#include <vector>                     // vector
#include <cmark-gfm.h>                // cmark_render_plaintext
#include <gumbo.h>                    // GumboOutput, gumbo_parse
#include <poppler-document.h>         // poppler::document
#include <poppler-page.h>             // poppler::page
#include "llama.h"                    // llama_* APIs
#include "common.hpp"                 // GetPlainText_*
#include "../main_program/Auto.h"     // The 'Auto' macro

constexpr char model_path[] = "./deepseek.gguf";

using std::cerr;
using std::endl;
using std::int32_t;
using std::ifstream;
using std::ofstream;
using std::ostringstream;
using std::size_t;
using std::string;
using std::unique_ptr;
using std::vector;
namespace fs = std::filesystem;

ofstream logfile;

int main(int const argc, char **const argv)
{
    if ( 2 != argc )
    {
        cerr << "Usage: " << argv[0] << " <directory containing text files>\n";
        return EXIT_FAILURE;
    }

    cerr << "Attempting to create 'log.txt' in current directory. . .\n";
    logfile.open("log.txt");
    if ( ! logfile )
    {
        std::cerr << "Failed to create file 'logfile.txt' in current directory\n";
        return EXIT_FAILURE;
    }
    logfile << "First line in log file\n";

    fs::path const input_dir( argv[1] );

    llama_backend_init();
    Auto( llama_backend_free() );

    llama_model_params const model_params = llama_model_default_params();
    llama_model *const model = llama_model_load_from_file(model_path, model_params);
    if ( nullptr == model )
    {
        std::cerr << "Failed to load model: " << model_path << endl;
        return EXIT_FAILURE;
    }
    Auto( llama_model_free(model) );

    llama_vocab const *const vocab = llama_model_get_vocab(model);
    if ( nullptr == vocab )
    {
        std::cerr << "Failed to get vocabulary from model.\n";
        return EXIT_FAILURE;
    }

    vector<llama_token> tokens;

    IterateThroughFolder( input_dir,
      [&](std::filesystem::path const path, std::string_view const text) -> bool
      {
        constexpr size_t factor = 4uz;

        if ( text.size() >= ((0x7FFFFFFFuz / factor) + 1u) )
        {
            logfile << "-------------------- Input file is too big: " << path << "    ---- Size: " << megabytes(text) << " megabytes\n";
            return false;
        }

        size_t const initial_guess = factor * text.size();

        // Make sure the vector is large enough, but never shrink
        if ( tokens.size() < initial_guess )
        {
            tokens.resize(initial_guess);
            logfile << "-------- Resized token vector to: " << megabytes(tokens) << " megabytes\n";
        }

        auto tokenize = [&](void) -> decltype(auto)
          {
                return llama_tokenize(
                  vocab,
                  &text.front(),
                  static_cast<int32_t>(text.size()),
                  &tokens.front(),
                  static_cast<int32_t>(tokens.size()),
                  true,          // add_special
                  false          // parse_special
                );
          };

        int32_t n_tokens = tokenize();

        if ( n_tokens < 0 )
        {
            n_tokens = -n_tokens;
            tokens.resize(n_tokens); // Resize for larger text

            n_tokens = tokenize();

            if ( n_tokens < 0 )
            {
                logfile << "-------------------- Failed to tokenize after resizing: " << path << endl;
                return false;
            }
        }

        fs::path out_path = path;
        string const out_name = out_path.replace_extension(".tokens").string();
        std::ofstream out(out_name, std::ios::binary);
        if ( ! out )
        {
            logfile << "-------------------- Failed to open output file: " << out_name << endl;
            return false;
        }

        out.write( (char*)&tokens.front(), n_tokens * sizeof(tokens.front()) );
        if ( ! out )
        {
            logfile << "-------------------- Failed to write to output file: " << out_name << endl;
            return false;
        }

        logfile << "Wrote " << megabytes(tokens, n_tokens) << " megabytes to: " << out_name << endl;

        return true;
      });
}


#include <iostream>
#include <vector>
#include <fstream>
#include "argparse/argparse.h"
#include "boost/wave.hpp"
#include "boost/wave/cpplexer/cpp_lex_token.hpp"
#include "boost/wave/cpplexer/cpp_lex_iterator.hpp"

using namespace argparse;

const static std::string MACRO_PREFIX = "///#";

struct Range;
enum class EDirectiveType;

int main(int argc, const char* argv[]) {
    ArgumentParser parser("ShaderPreprocessor", "A tool to process WGSL files.");
    parser.add_argument()
            .names({"-f", "--file"})
            .description("Specific file been preprocess.")
            .required(true);
    parser.add_argument()
            .names({"-o", "--output"})
            .description("Processed file output path.")
            .required(true);
    parser.add_argument()
        .names({"-P", "--path"})
        .description("Add paths to locate file.")
        .required(true);
    parser.add_argument()
        .names({"-D", "--define"})
        .description("Add custom definition. e.g. '-D WOW=1'.");
    parser.enable_help();

    auto err = parser.parse(argc, argv);
    if (err) {
        std::cerr << err << std::endl;
        parser.print_help();
        return 1;
    }

    if (parser.exists("help")) {
        parser.print_help();
        return 0;
    }

    std::vector<std::string> arg_definitions = parser.get<std::vector<std::string>>("define");
    std::vector<std::string> arg_find_paths = parser.get<std::vector<std::string>>("path");
    std::string input_file_path = parser.get<std::string>("file");
    std::string output_file_path = parser.get<std::string>("output");

    std::ifstream input(input_file_path);
    std::ofstream output(output_file_path);

    if (!input.is_open() || !output.is_open()) {
        std::cerr << "Failed to open input/output file descriptor." << std::endl;
        return 2;
    }

    std::string input_code(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>{});

    using LexIteratorType = boost::wave::cpplexer::lex_iterator<boost::wave::cpplexer::lex_token<>>;
    using ContextType = boost::wave::context<std::string::iterator, LexIteratorType>;

    ContextType context(std::begin(input_code), std::end(input_code), input_file_path.c_str());

    for (const auto& path : arg_find_paths) {
        context.add_include_path(path.c_str());
    }

    ContextType::iterator_type first = std::begin(context);
    ContextType::iterator_type last = std::end(context);
    while (last != first) {
        output << (*first).get_value();
        ++first;
    }

    return 0;
}

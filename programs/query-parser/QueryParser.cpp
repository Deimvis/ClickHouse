#include <iostream>
#include <string>
#include <unordered_map>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <Parsers/ParserQuery.h>
#include <Parsers/parseQuery.h>

#include "JsonSerialization.hpp"
#include "ASTSerialization.hpp"


struct Args {
    enum Format {
        F_JSON,
        F_DOT,
    };

    Format serialization_format;
};

Args::Format parseFormat(const std::string& format_str) {
    static std::unordered_map<std::string, Args::Format> mapping = {
        {"json", Args::Format::F_JSON},
        {"dot", Args::Format::F_DOT},
    };
    auto it = mapping.find(format_str);
    if (it == mapping.end()) {
        throw std::runtime_error(fmt::format("No format found for `{}` (only `json` and `dot` are avaiable)", format_str));
    }
    return it->second;
}

std::string readStdin() {
    std::string input;
    std::cin >> std::noskipws;
    std::istreambuf_iterator<char> it(std::cin);
    std::istreambuf_iterator<char> end;
    input.assign(it, end);
    return input;
}

std::string strip(const std::string &inpt) {
    auto start_it = inpt.begin();
    auto end_it = inpt.rbegin();
    while (std::isspace(*start_it) && (start_it < end_it.base()))
        ++start_it;
    while (std::isspace(*end_it) && (start_it < end_it.base()))
        ++end_it;
    return std::string(start_it, end_it.base());
}

Args parseArgs(int argc, char** argv) {
    Args args;
    if (argc >= 2) {
        args.serialization_format = parseFormat(argv[1]);
    } else {
        args.serialization_format = Args::Format::F_JSON;
    }
    return args;
}

int mainEntryClickHouseQueryParser(int argc, char** argv) {
    using namespace DB;

    auto args = parseArgs(argc, argv);
    std::string query = strip(readStdin());
    if (query.empty()) {
        throw std::runtime_error("query string should be passed through stdin");
    }
    fmt::print(stderr, "Input query:\n`{}`\n", query);

    ParserQuery parser(query.data() + query.size(), true);
    ASTPtr ast = parseQuery(parser, query.data(), query.data() + query.size(), "", 0, 0, 0);
    std::string serialized_ast;
    switch (args.serialization_format) {
    case Args::Format::F_JSON:
        serialized_ast = SerializeToJSON(ast);
        break;
    case Args::Format::F_DOT:
        serialized_ast = SerializeToDot(ast);
        break;
    }
    fmt::print(stdout, "{}", serialized_ast);
    return 0;
}

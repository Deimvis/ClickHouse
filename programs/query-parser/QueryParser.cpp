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
    while (std::isspace(*start_it) != 0 && (start_it < end_it.base()))
        ++start_it;
    while (std::isspace(*end_it) != 0 && (start_it < end_it.base()))
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

std::string parseQuery(const std::string& query, const Args::Format& format) {
    using namespace DB;

    ParserQuery parser(query.data() + query.size(), false);
    ASTPtr ast = parseQuery(parser, query.data(), query.data() + query.size(), "", 0, 0);
    std::string serialized_ast;
    switch (format) {
    case Args::Format::F_JSON:
        serialized_ast = SerializeToJSON(ast);
        break;
    case Args::Format::F_DOT:
        serialized_ast = SerializeToDot(ast);
        break;
    }
    return serialized_ast;
}

int mainEntryClickHouseQueryParser(int argc, char** argv) {
    using namespace DB;

    auto args = parseArgs(argc, argv);
    std::string query = strip(readStdin());
    if (query.empty()) {
        throw std::runtime_error("query string should be passed through stdin");
    }
    fmt::print(stderr, "Input query:\n`{}`\n", query);

    std::string serialized_ast = parseQuery(query, args.serialization_format);
    fmt::print(stdout, "{}", serialized_ast);
    return 0;
}

extern "C" {
    char* __attribute__((visibility("default"))) chqp_2b52ae1fb9f4ec8c46b8c527df829c25_parse_query(char* query, char* exc) {
        std::string serialized_ast;
        try {
            serialized_ast = parseQuery(std::string(query), Args::Format::F_JSON);
        } catch (const std::exception& e) {
            strncpy(exc, e.what(), 1024);
            exc[1023] = '\0';
            return nullptr;
        }
        char* result = reinterpret_cast<char*>(malloc(serialized_ast.size() + 1));
        std::strcpy(result, serialized_ast.c_str());
        return result;
    }
    
    void __attribute__((visibility("default"))) chqp_2b52ae1fb9f4ec8c46b8c527df829c25_free_ast(char* ast) {
        free(ast);
    }

    // return values: ast_json, error_msg
    void __attribute__((visibility("default"))) chqp_2b52ae1fb9f4ec8c46b8c527df829c25_parse_query_v2(char* query, char** ast_json, char** error_msg) {
        std::string serialized_ast;
        try {
            serialized_ast = parseQuery(std::string(query), Args::Format::F_JSON);
        } catch (const std::exception& e) {
            const char* msg = e.what();
            *error_msg = reinterpret_cast<char*>(malloc(strlen(msg) + 1));
            strcpy(*error_msg, msg);
            (*error_msg)[strlen(msg)] = '\0';
            return;
        }
        *ast_json = reinterpret_cast<char*>(malloc(serialized_ast.size() + 1));
        strcpy(*ast_json, serialized_ast.c_str());
    }
    
    void __attribute__((visibility("default"))) chqp_2b52ae1fb9f4ec8c46b8c527df829c25_free_ast_v2(char* ast_json) {
        free(ast_json);
    }
    
    void __attribute__((visibility("default"))) chqp_2b52ae1fb9f4ec8c46b8c527df829c25_free_error_v2(char* error_msg) {
        free(error_msg);
    }
}

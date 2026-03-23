#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <set>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <Parsers/ParserQuery.h>
#include <Parsers/ParserSelectQuery.h>
#include <Parsers/parseQuery.h>
#include <Parsers/formatAST.h>
#include <Parsers/IAST.h>
#include <Parsers/ASTSelectWithUnionQuery.h>
#include <Parsers/ASTSelectQuery.h>
#include <Parsers/ASTIdentifier.h>
#include <Parsers/ASTExpressionList.h>
#include <Parsers/ASTWithAlias.h>
#include <IO/ReadBufferFromString.h>
#include <IO/WriteBufferFromString.h>
#include <IO/Operators.h>
#include <Common/Exception.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>

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

std::string parseQuery(const std::string& query, const Args::Format& format) {
    using namespace DB;

    ParserQuery parser(query.data() + query.size(), false);
    ASTPtr ast = parseQuery(parser, query.data(), query.data() + query.size(), "", 0, 0, 0);
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


namespace DB
{
    namespace ErrorCodes
    {
        extern const int SYNTAX_ERROR;
    }
}

static DB::ASTPtr parseSqlToAST(const std::string & sql)
{
    using namespace DB;
    ParserQuery parser(sql.data() + sql.size(), false);
    const char * begin = sql.data();
    const char * end = sql.data() + sql.size();
    return parseQuery(parser, begin, end, "", 0, 0, 0);
}

static DB::ASTPtr parseExpressionToAST(const std::string & expr_str)
{
    using namespace DB;
    
    std::string wrapped_query = "SELECT " + expr_str;
    
    ParserSelectQuery parser; 
    const char * begin = wrapped_query.data();
    const char * end = wrapped_query.data() + wrapped_query.size();
    
    DB::ASTPtr select_ast = parseQuery(parser, begin, end, "", 0, 0, 0);
    
    if (!select_ast)
        throw Exception(DB::ErrorCodes::SYNTAX_ERROR, "Failed to parse expression: {}", expr_str);
        
    // Извлекаем первый элемент из списка SELECT
    if (auto * select = dynamic_cast<DB::ASTSelectQuery *>(select_ast.get()))
    {
        DB::ASTPtr list = select->select();
        if (list && !list->children.empty())
        {
            return list->children[0];
        }
    }
    
    throw Exception(DB::ErrorCodes::SYNTAX_ERROR, "Failed to extract expression from SELECT wrapper");
}

static std::string formatASTtoSQL(DB::ASTPtr ast)
{
    using namespace DB;
    WriteBufferFromOwnString out;
    formatAST(*ast, out, true, false);
    out << ";";
    out.finalize();
    return out.str();
}

static void collectSelects(DB::ASTPtr node, std::vector<DB::ASTSelectQuery *> & out_selects)
{
    if (!node) return;
    if (auto * union_select = dynamic_cast<DB::ASTSelectWithUnionQuery *>(node.get()))
    {
        auto list_of_selects = union_select->list_of_selects;
        if (list_of_selects)
        {
            for (const auto & child : list_of_selects->children)
            {
                collectSelects(child, out_selects);
            }
        }
    }
    else if (auto * select = dynamic_cast<DB::ASTSelectQuery *>(node.get()))
    {
        out_selects.push_back(select);
    }
}

static bool columnExists(const DB::ASTPtr & expression_list, const std::string & col_name, const std::string & alias_name)
{
    if (!expression_list) return false;
    
    for (const auto & child : expression_list->children)
    {
        std::string existing_alias = "";
        if (auto * with_alias = dynamic_cast<DB::ASTWithAlias *>(child.get()))
        {
            existing_alias = with_alias->alias;
        }
        
        std::string existing_name = "";
        if (auto * identifier = dynamic_cast<DB::ASTIdentifier *>(child.get()))
        {
            existing_name = identifier->shortName();
        }

        if (!alias_name.empty() && existing_alias == alias_name) return true;
        if (!col_name.empty() && existing_name == col_name) return true;
    }
    return false;
}

struct AddColumnParams {
    bool format_only = false;
    std::string column_name;
};

static AddColumnParams parseParams(const std::string & params_json_str)
{
    AddColumnParams params;
    if (params_json_str.empty()) return params;

    try {
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(params_json_str);
        Poco::JSON::Object::Ptr obj = result.extract<Poco::JSON::Object::Ptr>();

        if (obj->has("format_only"))
            params.format_only = obj->get("format_only").convert<bool>();

        if (obj->has("column_name"))
            params.column_name = obj->get("column_name").toString();

    } catch (...) {
    }
    return params;
}

static std::string addExtraColumnImpl(const std::string & sql, const AddColumnParams & params)
{
    using namespace DB;

    ASTPtr ast = parseSqlToAST(sql);
    if (!ast)
        throw Exception(DB::ErrorCodes::SYNTAX_ERROR, "Failed to parse SQL");

    if (params.format_only)
    {
        return formatASTtoSQL(ast);
    }

    if (params.column_name.empty())
    {
        return formatASTtoSQL(ast);
    }

    std::vector<DB::ASTSelectQuery *> selects;
    collectSelects(ast, selects);

    std::cerr << "[C++] Found " << selects.size() << " SELECT queries to modify." << std::endl;

    size_t as_pos = params.column_name.find(" as ");
    std::string potential_alias;
    if (as_pos != std::string::npos)
    {
        potential_alias = params.column_name.substr(as_pos + 4);
    }

    for (auto * select : selects)
    {
        ASTPtr select_expression = select->select();
        if (select_expression)
        {
            if (!columnExists(select_expression, params.column_name, potential_alias))
            {
                std::cerr << "[C++] Adding column: " << params.column_name << std::endl;
                
                DB::ASTPtr new_col_node = parseExpressionToAST(params.column_name);

                select_expression->children.push_back(new_col_node);
            }
            else
            {
                std::cerr << "[C++] Column " << params.column_name << " already exists, skipping." << std::endl;
            }
        }
    }

    return formatASTtoSQL(ast);
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

    char* __attribute__((visibility("default"))) chqp_add_column_to_sql(char* sql, char* params_json) {
        thread_local static std::vector<char> buffer;
        
        try {
            AddColumnParams params = parseParams(std::string(params_json));
            std::string result = addExtraColumnImpl(std::string(sql), params);
            
            buffer.resize(result.size() + 1);
            std::memcpy(buffer.data(), result.c_str(), result.size() + 1);
            
            return buffer.data();
        } catch (...) {
            return nullptr;
        }
    }
}

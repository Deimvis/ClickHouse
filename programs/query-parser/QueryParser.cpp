#include <string>
#include <unordered_map>
#include <vector>

#include <fmt/format.h>

#include <Parsers/ParserQuery.h>
#include <Parsers/ParserSelectQuery.h>
#include <Parsers/parseQuery.h>
#include <Parsers/formatAST.h>
#include <Parsers/IAST.h>
#include <Parsers/ASTSelectWithUnionQuery.h>
#include <Parsers/ASTSelectQuery.h>
#include <Parsers/ASTIdentifier.h>
#include <Parsers/ASTWithAlias.h>
#include <Parsers/ASTFunction.h>
#include <Parsers/ASTLiteral.h>
#include <IO/WriteBufferFromString.h>
#include <IO/Operators.h>
#include <Common/Exception.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>

#include "JsonSerialization.hpp"
#include "ASTSerialization.hpp"


#include <Common/checkStackSize.h>

void checkStackSize() {}


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
        throw std::runtime_error(fmt::format("No format found for `{}` (only `json` and `dot` are available)", format_str));
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
    if (inpt.empty()) return inpt;
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
    formatAST(*ast, out, false, false);
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

static bool columnExists(const DB::ASTPtr & expression_list, const std::string & target_name)
{
    if (!expression_list) return false;
    
    for (const auto & child : expression_list->children)
    {
        if (auto * with_alias = dynamic_cast<DB::ASTWithAlias *>(child.get()))
        {
            if (!with_alias->alias.empty() && with_alias->alias == target_name)
                return true;
        }
        
        if (auto * identifier = dynamic_cast<DB::ASTIdentifier *>(child.get()))
        {
            if (identifier->shortName() == target_name)
                return true;
        }
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

    size_t as_pos = params.column_name.find(" as ");
    std::string target_name;
    if (as_pos != std::string::npos)
    {
        target_name = params.column_name.substr(as_pos + 4);
    }
    else
    {
        target_name = params.column_name;
    }

    DB::ASTPtr new_col_template = parseExpressionToAST(params.column_name);

    for (auto * select : selects)
    {
        ASTPtr select_expression = select->select();
        if (select_expression)
        {
            if (!columnExists(select_expression, target_name))
            {
                select_expression->children.push_back(new_col_template->clone());
            }
        }
    }

    return formatASTtoSQL(ast);
}


static void collectSecretIdsFromTree(DB::ASTPtr node, std::vector<std::string> & out_ids)
{
    if (!node) return;

    if (auto * func = dynamic_cast<DB::ASTFunction *>(node.get()))
    {
        if (func->name == "$secret_id")
        {
            if (func->arguments && !func->arguments->children.empty())
            {
                auto & first_arg = func->arguments->children[0];
                if (auto * literal = dynamic_cast<DB::ASTLiteral *>(first_arg.get()))
                {
                    if (literal->value.getType() == DB::Field::Types::String)
                        out_ids.push_back(literal->value.safeGet<std::string>());
                }
            }
            return;
        }
    }

    for (const auto & child : node->children)
        collectSecretIdsFromTree(child, out_ids);
}

static void findRemoteSecretIds(DB::ASTPtr node, std::vector<std::string> & out_ids)
{
    if (!node) return;

    if (auto * func = dynamic_cast<DB::ASTFunction *>(node.get()))
    {
        if (func->name == "remote" || func->name == "remoteSecure")
        {
            if (func->arguments)
            {
                for (const auto & arg : func->arguments->children)
                    collectSecretIdsFromTree(arg, out_ids);
            }
        }
    }

    for (const auto & child : node->children)
        findRemoteSecretIds(child, out_ids);
}

static std::string extractSecretIdsImpl(const std::string & sql)
{
    using namespace DB;

    ASTPtr ast = parseSqlToAST(sql);
    if (!ast)
        throw Exception(ErrorCodes::SYNTAX_ERROR, "Failed to parse SQL");

    std::vector<std::string> secret_ids;
    findRemoteSecretIds(ast, secret_ids);

    return SerializeToJSON(secret_ids);
}

static void replaceSecretIdInTree(DB::ASTPtr & node, const std::unordered_map<std::string, std::string> & replacements)
{
    if (!node) return;

    if (auto * func = dynamic_cast<DB::ASTFunction *>(node.get()))
    {
        if (func->name == "$secret_id")
        {
            if (func->arguments && !func->arguments->children.empty())
            {
                auto & first_arg = func->arguments->children[0];
                if (auto * literal = dynamic_cast<DB::ASTLiteral *>(first_arg.get()))
                {
                    if (literal->value.getType() == DB::Field::Types::String)
                    {
                        auto it = replacements.find(literal->value.safeGet<std::string>());
                        if (it != replacements.end())
                        {
                            auto new_literal = std::make_shared<DB::ASTLiteral>(DB::Field(std::string(it->second)));
                            node = new_literal;
                            return;
                        }
                    }
                }
            }
        }
    }

    for (auto & child : node->children)
        replaceSecretIdInTree(child, replacements);
}

static void replaceRemoteSecretIds(DB::ASTPtr node, const std::unordered_map<std::string, std::string> & replacements)
{
    if (!node) return;

    if (auto * func = dynamic_cast<DB::ASTFunction *>(node.get()))
    {
        if (func->name == "remote" || func->name == "remoteSecure")
        {
            if (func->arguments)
            {
                for (auto & arg : func->arguments->children)
                    replaceSecretIdInTree(arg, replacements);
            }
        }
    }

    for (const auto & child : node->children)
        replaceRemoteSecretIds(child, replacements);
}

static std::unordered_map<std::string, std::string> parseReplacements(const std::string & json_str)
{
    std::unordered_map<std::string, std::string> replacements;
    if (json_str.empty()) return replacements;

    try {
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(json_str);
        Poco::JSON::Object::Ptr obj = result.extract<Poco::JSON::Object::Ptr>();

        for (auto it = obj->begin(); it != obj->end(); ++it)
            replacements[it->first] = it->second.convert<std::string>();
    } catch (...) {
    }
    return replacements;
}

static std::string replaceSecretIdsImpl(const std::string & sql, const std::string & replacements_json)
{
    using namespace DB;

    auto replacements = parseReplacements(replacements_json);

    ASTPtr ast = parseSqlToAST(sql);
    if (!ast)
        throw Exception(ErrorCodes::SYNTAX_ERROR, "Failed to parse SQL");

    if (!replacements.empty())
        replaceRemoteSecretIds(ast, replacements);

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
    void __attribute__((visibility("default"))) chqp_parse_query(char* query, char** ast_json, char** error_msg) {
        *ast_json = nullptr;
        *error_msg = nullptr;
        std::string serialized_ast;
        try {
            serialized_ast = parseQuery(std::string(query), Args::Format::F_JSON);
        } catch (const std::exception& e) {
            const char* msg = e.what();
            size_t msg_len = strlen(msg);
            *error_msg = reinterpret_cast<char*>(malloc(msg_len + 1));
            std::memcpy(*error_msg, msg, msg_len + 1);
            return;
        } catch (...) {
            const char* msg = "Unknown error";
            *error_msg = reinterpret_cast<char*>(malloc(strlen(msg) + 1));
            std::memcpy(*error_msg, msg, strlen(msg) + 1);
            return;
        }
        *ast_json = reinterpret_cast<char*>(malloc(serialized_ast.size() + 1));
        std::memcpy(*ast_json, serialized_ast.c_str(), serialized_ast.size() + 1);
    }
    
    void __attribute__((visibility("default"))) chqp_free_ast(char* ast_json) {
        free(ast_json);
    }
    
    void __attribute__((visibility("default"))) chqp_free_error(char* error_msg) {
        free(error_msg);
    }

    void __attribute__((visibility("default"))) chqp_format_query(char* sql, char** result_sql, char** error_msg) {
        *result_sql = nullptr;
        *error_msg = nullptr;
        try {
            AddColumnParams params;
            params.format_only = true;
            std::string result = addExtraColumnImpl(std::string(sql), params);
            
            *result_sql = reinterpret_cast<char*>(malloc(result.size() + 1));
            std::memcpy(*result_sql, result.c_str(), result.size() + 1);
        } catch (const std::exception& e) {
            const char* msg = e.what();
            size_t msg_len = strlen(msg);
            *error_msg = reinterpret_cast<char*>(malloc(msg_len + 1));
            std::memcpy(*error_msg, msg, msg_len + 1);
        } catch (...) {
            const char* msg = "Unknown error";
            *error_msg = reinterpret_cast<char*>(malloc(strlen(msg) + 1));
            std::memcpy(*error_msg, msg, strlen(msg) + 1);
        }
    }
    
    void __attribute__((visibility("default"))) chqp_free_format_result(char* result_sql) {
        free(result_sql);
    }

    void __attribute__((visibility("default"))) chqp_free_format_error(char* error_msg) {
        free(error_msg);
    }

    void __attribute__((visibility("default"))) chqp_add_column_to_sql(char* sql, char* params_json, char** result_sql, char** error_msg) {
        *result_sql = nullptr;
        *error_msg = nullptr;
        try {
            AddColumnParams params = parseParams(std::string(params_json));
            std::string result = addExtraColumnImpl(std::string(sql), params);
            
            *result_sql = reinterpret_cast<char*>(malloc(result.size() + 1));
            std::memcpy(*result_sql, result.c_str(), result.size() + 1);
        } catch (const std::exception& e) {
            const char* msg = e.what();
            size_t msg_len = strlen(msg);
            *error_msg = reinterpret_cast<char*>(malloc(msg_len + 1));
            std::memcpy(*error_msg, msg, msg_len + 1);
        } catch (...) {
            const char* msg = "Unknown error";
            *error_msg = reinterpret_cast<char*>(malloc(strlen(msg) + 1));
            std::memcpy(*error_msg, msg, strlen(msg) + 1);
        }
    }
    
    void __attribute__((visibility("default"))) chqp_free_add_column_result(char* result_sql) {
        free(result_sql);
    }

    void __attribute__((visibility("default"))) chqp_free_add_column_error(char* error_msg) {
        free(error_msg);
    }

    void __attribute__((visibility("default"))) chqp_extract_secret_ids(char* sql, char** result_json, char** error_msg) {
        *result_json = nullptr;
        *error_msg = nullptr;
        try {
            std::string result = extractSecretIdsImpl(std::string(sql));
            *result_json = reinterpret_cast<char*>(malloc(result.size() + 1));
            std::memcpy(*result_json, result.c_str(), result.size() + 1);
        } catch (const std::exception& e) {
            const char* msg = e.what();
            size_t msg_len = strlen(msg);
            *error_msg = reinterpret_cast<char*>(malloc(msg_len + 1));
            std::memcpy(*error_msg, msg, msg_len + 1);
        } catch (...) {
            const char* msg = "Unknown error";
            *error_msg = reinterpret_cast<char*>(malloc(strlen(msg) + 1));
            std::memcpy(*error_msg, msg, strlen(msg) + 1);
        }
    }

    void __attribute__((visibility("default"))) chqp_free_extract_secret_ids_result(char* result_json) {
        free(result_json);
    }

    void __attribute__((visibility("default"))) chqp_free_extract_secret_ids_error(char* error_msg) {
        free(error_msg);
    }

    void __attribute__((visibility("default"))) chqp_replace_secret_ids(char* sql, char* replacements_json, char** result_sql, char** error_msg) {
        *result_sql = nullptr;
        *error_msg = nullptr;
        try {
            std::string result = replaceSecretIdsImpl(std::string(sql), std::string(replacements_json));
            *result_sql = reinterpret_cast<char*>(malloc(result.size() + 1));
            std::memcpy(*result_sql, result.c_str(), result.size() + 1);
        } catch (const std::exception& e) {
            const char* msg = e.what();
            size_t msg_len = strlen(msg);
            *error_msg = reinterpret_cast<char*>(malloc(msg_len + 1));
            std::memcpy(*error_msg, msg, msg_len + 1);
        } catch (...) {
            const char* msg = "Unknown error";
            *error_msg = reinterpret_cast<char*>(malloc(strlen(msg) + 1));
            std::memcpy(*error_msg, msg, strlen(msg) + 1);
        }
    }

    void __attribute__((visibility("default"))) chqp_free_replace_secret_ids_result(char* result_sql) {
        free(result_sql);
    }

    void __attribute__((visibility("default"))) chqp_free_replace_secret_ids_error(char* error_msg) {
        free(error_msg);
    }
}

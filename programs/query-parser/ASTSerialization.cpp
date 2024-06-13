#include "ASTSerialization.hpp"

#include <Parsers/formatAST.h>

#include "ASTCustomAttributes.hpp"


ASTNodeBuilder MakeBuilder(DB::ASTPtr ast) {
    ASTNodeBuilder builder("n" + std::to_string(reinterpret_cast<std::uintptr_t>(ast.get())));
    builder.emplaceAttribute("label", SerializeToJSON(ast->getID(' ')));
    builder.emplaceAttribute("sql", SerializeToJSON(DB::serializeAST(*ast)));
    for (const auto& attr: GetCustomAttributes(ast.get())) {
        builder.addAttribute(attr);
    }
    for (const auto& child : ast->children) {
        builder.addChild(MakeBuilder(child));
    }
    return builder;
}

std::string SerializeToJSON(DB::ASTPtr ast) {
    auto builder = MakeBuilder(ast);
    return builder.buildJSON();
}

std::string SerializeToDot(DB::ASTPtr ast) {
    auto builder = MakeBuilder(ast);
    return builder.buildDOT();
}

#pragma once

#include <Parsers/IAST.h>

#include "ASTNodeBuilder.hpp"

ASTNodeBuilder MakeBuilder(DB::ASTPtr ast);

std::string SerializeToJSON(DB::ASTPtr ast);

std::string SerializeToDot(DB::ASTPtr ast);

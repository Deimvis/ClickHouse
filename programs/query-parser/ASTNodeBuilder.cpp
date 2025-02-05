#include "ASTNodeBuilder.hpp"
#include "JsonSerialization.hpp"

#include <ranges>
#include <iostream>


std::string ASTNodeBuilder::buildDOTNodes(std::vector<Edge>& edges) const {
    auto attr2str = [](const Attribute& attr){
        auto value = attr.value;
        if (value.size() == 0 || value[0] != '"' || value[value.size() - 1] != '"') {
            value = fmt::format("\"{}\"", escape(value));
        }
        return fmt::format("{}={}", attr.key, value);
    };
    std::string node_dot = fmt::format("{} [{}];", id, fmt::join(attributes | std::views::transform(attr2str), " "));
    std::vector<std::string> node_dots = {node_dot};
    for (const auto& child : children) {
        edges.emplace_back(id, child.id);
        node_dots.push_back(child.buildDOTNodes(edges));
    }
    return fmt::format("    {}", fmt::join(node_dots, "\n"));
}

std::string ASTNodeBuilder::buildDOT() const {
    std::vector<Edge> edges;
    auto edge2str = [](const Edge& e){ return fmt::format("    {} -> {};", e.first, e.second); };
    std::string node_dots = buildDOTNodes(edges);
    return fmt::format("digraph {{\n{}\n{}\n}}",
                       node_dots,
                       fmt::join(edges | std::views::transform(edge2str), "\n"));
}

std::string ASTNodeBuilder::buildJSON() const {
    JSONObjectBuilder json;
    json.add("id", id);

    JSONObjectBuilder attributes_json;
    for (const auto& attr : attributes) {
        attributes_json.addSerialized(attr.key, attr.value);
    }
    json.addSerialized("attributes", attributes_json.serialize());

    JSONArrayBuilder children_json;
    for (const auto& child : children) {
        children_json.addSerialized(child.buildJSON());
    }
    json.addSerialized("children", children_json.serialize());

    return json.serialize();
}

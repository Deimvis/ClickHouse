#pragma once

#include <string>
#include <vector>

#include "JsonSerialization.hpp"

struct Attribute {
    std::string key;
    std::string value; // value serialized in JSON
};

template <typename T>
std::string SerializeAttributeValue(const T& v) {
    return SerializeToJSON(v);
}

class ASTNodeBuilder {
public:
    explicit ASTNodeBuilder(const std::string& id_)
        : id(id_)
        {}

    template <typename... Args>
    void emplaceAttribute(Args&&... args) {
        attributes.emplace_back(std::forward<Args>(args)...);
    }

    void addAttribute(const Attribute& attr) {
        attributes.push_back(attr);
    }
    
    void addChild(const ASTNodeBuilder& child) {
        children.push_back(child);
    }
    
    std::string buildDOT() const;
    
    std::string buildJSON() const;

private:
    using NodeID = std::string;
    using Edge = std::pair<NodeID, NodeID>;

    std::string buildDOTImpl(std::vector<Edge>& edges) const;

    std::string id;
    std::vector<Attribute> attributes;
    std::vector<ASTNodeBuilder> children;    
};

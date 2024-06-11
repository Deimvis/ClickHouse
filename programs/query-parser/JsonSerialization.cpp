#include "JsonSerialization.hpp"

std::string SerializeToJSON(bool b) {
    return (b) ? "true" : "false";
}

std::string SerializeToJSON(char c) {
    return SerializeToJSON(std::string(1, c));
}

std::string SerializeToJSON(const DB::SettingChange& v) {
    return SerializeToJSON(std::unordered_map<std::string, DB::Field>{{v.name, v.value}});
}

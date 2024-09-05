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

std::string escape(std::string_view s) {
    std::ostringstream out;
    for (const auto& c : s) {
        switch (c) {
        case '"': out << "\\\""; break;
        case '\\': out << "\\\\"; break;
        case '\b': out << "\\b"; break;
        case '\f': out << "\\f"; break;
        case '\n': out << "\\n"; break;
        case '\r': out << "\\r"; break;
        case '\t': out << "\\t"; break;
        default:
            if ('\x00' <= c && c <= '\x1f') {
                out << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
            } else {
                out << c;
            }
        }
    }
    return out.str();
}

#pragma once

#include <concepts>
#include <ranges>
#include <type_traits>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <Core/Field.h>
#include <Common/SettingsChanges.h>


template <typename T>
concept SerializableEnum = std::is_enum_v<T> && requires (T v) {
    { toString(v) } -> std::convertible_to<const char*>;
};

template <typename T>
concept Integral = std::is_integral_v<T> && !std::is_same_v<T, char>;

template <typename T>
concept StringLike = std::convertible_to<T, std::string_view>;

template <typename T>
concept Dumpable = requires(T v) {
    { v.dump() } -> std::convertible_to<std::string>;
};

std::string SerializeToJSON(bool b);

std::string SerializeToJSON(char c);

std::string SerializeToJSON(const DB::SettingChange& v);

template <Integral T>
std::string SerializeToJSON(const T& v) {
    return fmt::format("{}", v);
}

template <StringLike T>
std::string SerializeToJSON(const T& v) {
    return fmt::format("\"{}\"", v);
}

template <Dumpable T>
std::string SerializeToJSON(const T& v) {
    return SerializeToJSON(v.dump());
}

template <SerializableEnum T>
std::string SerializeToJSON(const T& v) {
    return SerializeToJSON(toString(v));
}

template <typename T>
std::string SerializeToJSON(const std::optional<T>& v) {
    if (!v.has_value()) {
        return "null";
    }
    return SerializeToJSON(*v);
}

template <typename T, typename U>
std::string SerializeToJSON(const std::pair<T, U>& p) {
    return fmt::format("[{},{}]", SerializeToJSON(p.first), SerializeToJSON(p.second));
}

template <std::ranges::input_range R> requires(!StringLike<R>)
std::string SerializeToJSON(const R& range) {
    auto el2str = [](const auto& el) { return SerializeToJSON(el); };
    return fmt::format("[{}]", fmt::join(range | std::views::transform(el2str), ","));
}

template <StringLike K, typename V>
std::string SerializeToJSON(const std::unordered_map<K, V>& v) {
    auto pair2str = [](const auto& p) { return fmt::format("{}:{}", SerializeToJSON(p.first), SerializeToJSON(p.second)); };
    return fmt::format("{{{}}}", fmt::join(v | std::views::transform(pair2str), ","));
}


template <typename T>
concept JSONSerializable = requires(T v) {
    { SerializeToJSON(v) } -> std::same_as<std::string>;
};

class JSONArrayBuilder {
public:
    template <JSONSerializable T>
    void add(const T& el) {
        elements.push_back(SerializeToJSON(el));
    }
    
    void addSerialized(const std::string& el) {
        elements.push_back(el);
    }

    std::string serialize(bool sort_keys = false) const {
        const std::vector<std::string>* work_elements = &elements;
        if (sort_keys) {
            auto sorted_work_elements = elements;
            std::ranges::sort(sorted_work_elements);
            work_elements = &sorted_work_elements;
        }
        return fmt::format("[{}]", fmt::join(*work_elements, ","));
    }

private:
    std::vector<std::string> elements;
};

class JSONObjectBuilder {
public:
    template <JSONSerializable T>
    void add(const std::string& key, const T& value) {
        data[key] = SerializeToJSON(value);
    }
    
    void addSerialized(const std::string& key, const std::string& value) {
        data[key] = value;
    }

    std::string serialize() const {
        auto pair2str = [](const auto& p) { return fmt::format("\"{}\":{}", p.first, p.second); };
        return fmt::format("{{{}}}", fmt::join(data | std::views::transform(pair2str), ","));
    }

private:
    std::unordered_map<std::string, std::string> data;
};

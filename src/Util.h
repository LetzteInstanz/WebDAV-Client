#pragma once

#include <nlohmann/json.hpp>

template <typename T>
constexpr int to_int(T value) noexcept { return static_cast<int>(value); }

template <typename T2, typename T1>
constexpr T2 to_type(T1&& value) noexcept { return static_cast<T2>(value); }

template <typename ReturnType>
auto get_value_from_json(const nlohmann::json& json, const char* key) {
    try {
        return json.at(key).get<ReturnType>();
    } catch (const nlohmann::json::exception& e) {
        qWarning(qUtf8Printable(QObject::tr("data.json: %s: %s")), key, e.what());
        throw e;
    }
};

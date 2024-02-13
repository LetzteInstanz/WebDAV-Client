#pragma once

#include <QString>

template <typename T>
constexpr int to_int(T value) noexcept { return static_cast<int>(value); }

template <typename T2, typename T1>
constexpr T2 to_type(T1&& value) noexcept { return static_cast<T2>(value); }

void json_value_exist_warning(const char* key);
void json_value_type_warning(const char* key, const QString& value_type);

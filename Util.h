#pragma once

template <typename T>
constexpr int toInt(T value) { return static_cast<int>(value); }

template <typename T>
constexpr T toType(int value) { return static_cast<T>(value); }

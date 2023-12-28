#pragma once

template <typename T>
constexpr int to_int(T value) { return static_cast<int>(value); }

template <typename T>
constexpr T to_type(const int value) { return static_cast<T>(value); }

#pragma once

#include <cstddef>

template <typename T>
constexpr int to_int(T value) noexcept { return static_cast<int>(value); }

template <typename T>
constexpr size_t to_size_t(T value) noexcept { return static_cast<size_t>(value); }

template <typename T>
constexpr T to_type(const int value) noexcept { return static_cast<T>(value); }

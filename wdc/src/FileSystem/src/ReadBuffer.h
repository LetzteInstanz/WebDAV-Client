#pragma once

#ifdef TESTS
constexpr std::size_t bufferSize = 11;
#else
constexpr std::size_t bufferSize = 256*1024;
#endif
using ReadBuffer = std::array<char, bufferSize>;

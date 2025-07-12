#pragma once

class SizeDisplayer {
public:
    static QString to_string(std::uint64_t bytes);

private:
    constexpr static std::size_t size = 7;
    static const std::array<QString, size> _prefixes;
    static const QLocale _locale;
};

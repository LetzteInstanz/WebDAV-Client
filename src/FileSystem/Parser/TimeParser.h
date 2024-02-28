#pragma once

#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <QChar>
#include <QString>
#include <QStringView>

class TimeParser {
public:
    enum class Format {Rfc2616, Rfc3339};

    static std::chrono::sys_seconds to_sys_seconds(const QStringView& str, Format f);

private:
    struct CustomTime {
        std::chrono::year year;
        std::chrono::month month;
        std::chrono::day day;
        std::chrono::hours hours;
        std::chrono::minutes minutes;
        std::chrono::seconds seconds;
        int time_zone_sign;
    };
    enum class Token {DayName, Day, Month, MonthName, Year, Hours, Minutes, Seconds, ZoneHours, ZoneMinutes};
    using CharSet = std::unordered_set<QChar>;
    using TokenOrder = std::vector<Token>;

    static const CharSet& get_delimiters(Format f);
    static const TokenOrder& get_order(const QStringView& str, Format f);
    static void parse(CustomTime& time, const QStringView& lexem, bool& ok, Token token);

private:
    static const CharSet _rfc2616_delimiters;
    static const CharSet _rfc3339_delimiters;
    static const std::unordered_map<QString, int> _month_map;
    static const TokenOrder _rfc2616_order_1;
    static const TokenOrder _rfc2616_order_2;
    static const TokenOrder _rfc3339_order_1;
    static const TokenOrder _rfc3339_order_2;
};

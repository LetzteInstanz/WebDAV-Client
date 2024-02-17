#pragma once

#include <ctime>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <QChar>
#include <QString>
#include <QStringView>

class TimeParser {
public:
    enum class Format {Rfc2616, Rfc3339};

    static time_t to_time_t(const QStringView& str, Format f);

private:
    struct CustomTm {
        std::tm tm;
        int zone_hours;
        int zone_minutes;
    };
    enum class Token {DayName, Day, Month, MonthName, Year, Hours, Minutes, Seconds, ZoneHours, ZoneMinutes};
    using CharSet = std::unordered_set<QChar>;
    using TokenOrder = std::vector<Token>;

    static const CharSet& get_delimiters(Format f);
    static const TokenOrder& get_order(const QStringView& str, Format f);
    static void parse(CustomTm& time, const QStringView& lexem, bool& ok, Token token);

private:
    static const CharSet _rfc2616_delimiters;
    static const CharSet _rfc3339_delimiters;
    static const std::unordered_map<QString, int> _month_map;
    static const TokenOrder _rfc2616_order_1;
    static const TokenOrder _rfc2616_order_2;
    static const TokenOrder _rfc3339_order_1;
    static const TokenOrder _rfc3339_order_2;
};

module;

#include <chrono>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <QChar>
#include <QHash>
#include <QString>
#include <QStringView>

#include "../Util.h"

export module Parser;

export class TimeParser {
public:
    enum class Format {Rfc2616, Rfc3339};

    static std::chrono::sys_seconds to_sys_seconds(const QStringView& str, Format f);

#ifndef NDEBUG
    static void test();
#endif

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
    static const std::unordered_map<QString, std::chrono::month> _month_map;
    static const TokenOrder _rfc2616_order_1;
    static const TokenOrder _rfc2616_order_2;
    static const TokenOrder _rfc3339_order_1;
    static const TokenOrder _rfc3339_order_2;
};

std::chrono::sys_seconds TimeParser::to_sys_seconds(const QStringView& str, Format f) {
    CustomTime time;
    const auto str_end = std::end(str);
    const CharSet& delimiters = get_delimiters(f);
    const TokenOrder& order = get_order(str, f);
    const auto token_end = std::end(order);
    auto token_it = std::begin(order);
    auto from = std::begin(str);
    bool prev_is_delimiter = false;
    for (auto it = from; token_it != token_end; ++it) {
        const auto is_str_end = it == str_end;
        const bool is_delimiter = is_str_end || delimiters.contains(*it);
        const bool is_lexem_end = is_delimiter && !prev_is_delimiter;
        bool ok = true;
        if (is_lexem_end) {
            const Token token = *token_it;
            const auto prev_delimiter_is_sign = token == Token::ZoneHours;
            if (prev_delimiter_is_sign) {
                assert(from != std::begin(str));
                --from;
            }
            const QStringView lexem(from, it - from);
            parse(time, lexem, ok, token);
            if (!ok)
                throw std::runtime_error("timestamp parse error");

            ++token_it;
        }
        if (is_str_end)
            break;

        from = !is_delimiter && prev_is_delimiter ? it : from;
        prev_is_delimiter = is_delimiter;
    }
    if (token_it != token_end)
        throw std::runtime_error("timestamp parse error");

    const std::chrono::sys_days days = std::chrono::year_month_day(time.year, time.month, time.day);
    auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(days);
    seconds += time.hours;
    seconds += time.minutes;
    seconds += time.seconds; // todo: use std::chrono::utc_clock type and std::chrono::std::chrono::clock_cast(), when GCC will support this
    return seconds;
}

#ifndef NDEBUG
void TimeParser::test() {
    using namespace std::chrono_literals;

    for (const auto& str : std::vector<QString>{"Sun, 06 Nov 1999 08:49:37 GMT", "Sunday, 06-Nov-99 08:49:37 GMT", "Sun Nov 6 08:49:37 1999"}) {
        const std::chrono::sys_seconds seconds = TimeParser::to_sys_seconds(str, TimeParser::Format::Rfc2616);
        const std::chrono::year_month_day yyyy_mm_dd(std::chrono::time_point_cast<std::chrono::days>(seconds));
        assert(yyyy_mm_dd.year() == 1999y);
        assert(yyyy_mm_dd.month() == std::chrono::November);
        assert(yyyy_mm_dd.day() == 6d);
        const std::chrono::hh_mm_ss<std::chrono::sys_seconds::duration> hh_mm_ss(seconds - to_type<std::chrono::sys_days>(yyyy_mm_dd));
        assert(hh_mm_ss.hours() == 8h);
        assert(hh_mm_ss.minutes() == 49min);
        assert(hh_mm_ss.seconds() == 37s);
    }
    std::chrono::sys_seconds seconds = TimeParser::to_sys_seconds(QString("Sun, 06 Nov 2024 08:49:37 GMT"), TimeParser::Format::Rfc2616);
    std::chrono::year_month_day yyyy_mm_dd(std::chrono::time_point_cast<std::chrono::days>(seconds));
    assert(yyyy_mm_dd.year() == 2024y);

    seconds = TimeParser::to_sys_seconds(QString("1985-04-12T23:20:50.52Z"), TimeParser::Format::Rfc3339);
    yyyy_mm_dd = std::chrono::year_month_day(std::chrono::time_point_cast<std::chrono::days>(seconds));
    assert(yyyy_mm_dd.year() == 1985y);
    assert(yyyy_mm_dd.month() == std::chrono::April);
    assert(yyyy_mm_dd.day() == 12d);
    std::chrono::hh_mm_ss<std::chrono::sys_seconds::duration> hh_mm_ss(seconds - to_type<std::chrono::sys_days>(yyyy_mm_dd));
    assert(hh_mm_ss.hours() == 23h);
    assert(hh_mm_ss.minutes() == 20min);
    assert(hh_mm_ss.seconds() == 51s);

    seconds = TimeParser::to_sys_seconds(QString("1996-12-19T16:39:57.473-08:21"), TimeParser::Format::Rfc3339);
    yyyy_mm_dd = std::chrono::year_month_day(std::chrono::time_point_cast<std::chrono::days>(seconds));
    assert(yyyy_mm_dd.year() == 1996y);
    assert(yyyy_mm_dd.month() == std::chrono::December);
    assert(yyyy_mm_dd.day() == 20d);
    hh_mm_ss = std::chrono::hh_mm_ss<std::chrono::sys_seconds::duration>(seconds - to_type<std::chrono::sys_days>(yyyy_mm_dd));
    assert(hh_mm_ss.hours() == 1h);
    assert(hh_mm_ss.minutes() == 0min);
    assert(hh_mm_ss.seconds() == 57s);

    seconds = TimeParser::to_sys_seconds(QString("1996-12-19T16:29:57+08:30"), TimeParser::Format::Rfc3339);
    yyyy_mm_dd = std::chrono::year_month_day(std::chrono::time_point_cast<std::chrono::days>(seconds));
    assert(yyyy_mm_dd.year() == 1996y);
    assert(yyyy_mm_dd.month() == std::chrono::December);
    assert(yyyy_mm_dd.day() == 19d);
    hh_mm_ss = std::chrono::hh_mm_ss<std::chrono::sys_seconds::duration>(seconds - to_type<std::chrono::sys_days>(yyyy_mm_dd));
    assert(hh_mm_ss.hours() == 7h);
    assert(hh_mm_ss.minutes() == 59min);
    assert(hh_mm_ss.seconds() == 57s);
}
#endif

const TimeParser::CharSet& TimeParser::get_delimiters(Format f) { return f == Format::Rfc2616 ? _rfc2616_delimiters : _rfc3339_delimiters; }

const TimeParser::TokenOrder& TimeParser::get_order(const QStringView& str, Format f) {
    switch (f) {
        case Format::Rfc2616:
            return str.back() == 'T' ? _rfc2616_order_1 : _rfc2616_order_2;

        default:
            return str.back() == 'Z' ? _rfc3339_order_1 : _rfc3339_order_2;
    }
}

void TimeParser::parse(CustomTime& time, const QStringView& lexem, bool& ok, Token token) {
    switch (token) {
        case Token::DayName: {
            break;
        }

        case Token::Day: {
            time.day = std::chrono::day(lexem.toInt(&ok));
            break;
        }

        case Token::Month: {
            time.month = std::chrono::month(lexem.toInt(&ok));
            break;
        }

        case Token::MonthName: {
            const auto it = _month_map.find(lexem.toString());
            if (it == std::end(_month_map))
                throw std::runtime_error("month parse error");

            time.month = it->second;
            break;
        }

        case Token::Year: {
            const auto number = lexem.toInt(&ok);
            time.year = std::chrono::year(number <= 99 ? number + 1900 : number);
            break;
        }

        case Token::Hours: {
            time.hours = std::chrono::hours(lexem.toInt(&ok));
            break;
        }

        case Token::Minutes: {
            time.minutes = std::chrono::minutes(lexem.toInt(&ok));
            break;
        }

        case Token::Seconds: {
            const auto sec = lexem.toFloat(&ok);
            time.seconds = std::chrono::seconds(to_int(std::round(sec)));
            break;
        }

        case Token::ZoneHours: {
            const auto number = lexem.toInt(&ok);
            time.hours -= std::chrono::hours(number);
            time.time_zone_sign = std::copysign(1, number);
            break;
        }

        case Token::ZoneMinutes: {
            time.minutes -= time.time_zone_sign * std::chrono::minutes(lexem.toInt(&ok));
            break;
        }
    }
}

const TimeParser::CharSet TimeParser::_rfc2616_delimiters{' ', ',', '-', ':'};
const TimeParser::CharSet TimeParser::_rfc3339_delimiters{'-', '+', ':', 'T', 'Z', 't', 'z'};

const std::unordered_map<QString, std::chrono::month> TimeParser::_month_map{{"Jan", std::chrono::January}, {"Feb", std::chrono::February}, {"Mar", std::chrono::March}, {"Apr", std::chrono::April}, {"May", std::chrono::May}, {"Jun", std::chrono::June},
                                                                             {"Jul", std::chrono::July}, {"Aug", std::chrono::August}, {"Sep", std::chrono::September}, {"Oct", std::chrono::October}, {"Nov", std::chrono::November}, {"Dec", std::chrono::December}};

const TimeParser::TokenOrder TimeParser::_rfc2616_order_1{Token::DayName, Token::Day, Token::MonthName, Token::Year, Token::Hours, Token::Minutes, Token::Seconds};
const TimeParser::TokenOrder TimeParser::_rfc2616_order_2{Token::DayName, Token::MonthName, Token::Day, Token::Hours, Token::Minutes, Token::Seconds, Token::Year};
const TimeParser::TokenOrder TimeParser::_rfc3339_order_1{Token::Year, Token::Month, Token::Day, Token::Hours, Token::Minutes, Token::Seconds};
const TimeParser::TokenOrder TimeParser::_rfc3339_order_2{Token::Year, Token::Month, Token::Day, Token::Hours, Token::Minutes, Token::Seconds, Token::ZoneHours, Token::ZoneMinutes};

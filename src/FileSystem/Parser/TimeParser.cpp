#include "TimeParser.h"

#include "../Util.h"

std::chrono::sys_seconds TimeParser::to_sys_seconds(const QStringView& str, Format f) {
    CustomTime time; // todo: replace with std::chrono::parse(), when GCC will support this
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
    seconds += time.seconds;
    return seconds;
}

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

            time.month = std::chrono::month(it->second);
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

const std::unordered_map<QString, int> TimeParser::_month_map{{"Jan", 1}, {"Feb", 2}, {"Mar", 3}, {"Apr", 4}, {"May", 5}, {"Jun", 6},
                                                              {"Jul", 7}, {"Aug", 8}, {"Sep", 9}, {"Oct", 10}, {"Nov", 11}, {"Dec", 12}};

const TimeParser::TokenOrder TimeParser::_rfc2616_order_1{Token::DayName, Token::Day, Token::MonthName, Token::Year, Token::Hours, Token::Minutes, Token::Seconds};
const TimeParser::TokenOrder TimeParser::_rfc2616_order_2{Token::DayName, Token::MonthName, Token::Day, Token::Hours, Token::Minutes, Token::Seconds, Token::Year};
const TimeParser::TokenOrder TimeParser::_rfc3339_order_1{Token::Year, Token::Month, Token::Day, Token::Hours, Token::Minutes, Token::Seconds};
const TimeParser::TokenOrder TimeParser::_rfc3339_order_2{Token::Year, Token::Month, Token::Day, Token::Hours, Token::Minutes, Token::Seconds, Token::ZoneHours, Token::ZoneMinutes};

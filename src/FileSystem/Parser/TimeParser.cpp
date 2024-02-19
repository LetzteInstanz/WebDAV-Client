#include "TimeParser.h"

time_t TimeParser::to_time_t(const QStringView& str, Format f) {
    CustomTm time = {.tm = {.tm_isdst = 0}, .zone_hours = 0, .zone_minutes = 0};
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

#ifdef Q_OS_WIN // todo: replace with std::gmtime(), when the compiler will support that
    time_t seconds = _mkgmtime(&time.tm);
#else
    time_t seconds = timegm(&time.tm);
#endif
    const auto sign = std::copysign(1, time.zone_hours);
    seconds -= time.zone_hours * 60 * 60 + sign * time.zone_minutes * 60;
    return seconds;
}

const TimeParser::CharSet& TimeParser::get_delimiters(Format f) {
    switch (f) {
        case Format::Rfc2616:
            return _rfc2616_delimiters;

        default:
            return _rfc3339_delimiters;
    }
}

const TimeParser::TokenOrder& TimeParser::get_order(const QStringView& str, Format f) {
    switch (f) {
        case Format::Rfc2616:
            return str.back() == 'T' ? _rfc2616_order_1 : _rfc2616_order_2;

        default:
            return str.back() == 'Z' ? _rfc3339_order_1 : _rfc3339_order_2;
    }
}

void TimeParser::parse(CustomTm& time, const QStringView& lexem, bool& ok, Token token) {
    switch (token) {
        case Token::DayName: {
            break;
        }

        case Token::Day: {
            time.tm.tm_mday = lexem.toInt(&ok);
            break;
        }

        case Token::Month: {
            time.tm.tm_mon = lexem.toInt(&ok) - 1;
            break;
        }

        case Token::MonthName: {
            const auto it = _month_map.find(lexem.toString());
            if (it == std::end(_month_map))
                throw std::runtime_error("month parse error");

            time.tm.tm_mon = it->second;
            break;
        }

        case Token::Year: {
            int year = lexem.toInt(&ok);
            time.tm.tm_year = year > 99 ? year - 1900 : year;
            break;
        }

        case Token::Hours: {
            time.tm.tm_hour = lexem.toInt(&ok);
            break;
        }

        case Token::Minutes: {
            time.tm.tm_min = lexem.toInt(&ok);
            break;
        }

        case Token::Seconds: {
            time.tm.tm_sec = std::round(lexem.toFloat(&ok));
            break;
        }

        case Token::ZoneHours: {
            time.zone_hours = lexem.toInt(&ok);
            break;
        }

        case Token::ZoneMinutes: {
            time.zone_minutes = lexem.toInt(&ok);
            break;
        }
    }
}

const TimeParser::CharSet TimeParser::_rfc2616_delimiters{' ', ',', '-', ':'};
const TimeParser::CharSet TimeParser::_rfc3339_delimiters{'-', '+', ':', 'T', 'Z', 't', 'z'};

const std::unordered_map<QString, int> TimeParser::_month_map{{"Jan", 0}, {"Feb", 1}, {"Mar", 2}, {"Apr", 3}, {"May", 4}, {"Jun", 5},
                                                              {"Jul", 6}, {"Aug", 7}, {"Sep", 8}, {"Oct", 9}, {"Nov", 10}, {"Dec", 11}};

const TimeParser::TokenOrder TimeParser::_rfc2616_order_1{Token::DayName, Token::Day, Token::MonthName, Token::Year, Token::Hours, Token::Minutes, Token::Seconds};
const TimeParser::TokenOrder TimeParser::_rfc2616_order_2{Token::DayName, Token::MonthName, Token::Day, Token::Hours, Token::Minutes, Token::Seconds, Token::Year};
const TimeParser::TokenOrder TimeParser::_rfc3339_order_1{Token::Year, Token::Month, Token::Day, Token::Hours, Token::Minutes, Token::Seconds};
const TimeParser::TokenOrder TimeParser::_rfc3339_order_2{Token::Year, Token::Month, Token::Day, Token::Hours, Token::Minutes, Token::Seconds, Token::ZoneHours, Token::ZoneMinutes};

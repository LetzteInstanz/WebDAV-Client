#include "TimeParser.h"

#include "../Util.h"

std::chrono::sys_seconds Parser::CurrentState::TimeParser::to_sys_seconds(QStringView str, Format f) {
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
void Parser::CurrentState::TimeParser::test() {
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

const Parser::CurrentState::TimeParser::CharSet& Parser::CurrentState::TimeParser::get_delimiters(Format f) { return f == Format::Rfc2616 ? _rfc2616_delimiters : _rfc3339_delimiters; }

const Parser::CurrentState::TimeParser::TokenOrder& Parser::CurrentState::TimeParser::get_order(QStringView str, Format f) {
    switch (f) {
        case Format::Rfc2616:
            return str.back() == 'T' ? _rfc2616_order_1 : _rfc2616_order_2;

        default:
            return str.back() == 'Z' ? _rfc3339_order_1 : _rfc3339_order_2;
    }
}

void Parser::CurrentState::TimeParser::parse(CustomTime& time, QStringView lexem, bool& ok, Token token) {
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

const Parser::CurrentState::TimeParser::CharSet Parser::CurrentState::TimeParser::_rfc2616_delimiters{' ', ',', '-', ':'};
const Parser::CurrentState::TimeParser::CharSet Parser::CurrentState::TimeParser::_rfc3339_delimiters{'-', '+', ':', 'T', 'Z', 't', 'z'};

const std::unordered_map<QString, std::chrono::month> Parser::CurrentState::TimeParser::_month_map{{"Jan", std::chrono::January}, {"Feb", std::chrono::February}, {"Mar", std::chrono::March}, {"Apr", std::chrono::April}, {"May", std::chrono::May}, {"Jun", std::chrono::June},
                                                                                                   {"Jul", std::chrono::July}, {"Aug", std::chrono::August}, {"Sep", std::chrono::September}, {"Oct", std::chrono::October}, {"Nov", std::chrono::November}, {"Dec", std::chrono::December}};

const Parser::CurrentState::TimeParser::TokenOrder Parser::CurrentState::TimeParser::_rfc2616_order_1{Token::DayName, Token::Day, Token::MonthName, Token::Year, Token::Hours, Token::Minutes, Token::Seconds};
const Parser::CurrentState::TimeParser::TokenOrder Parser::CurrentState::TimeParser::_rfc2616_order_2{Token::DayName, Token::MonthName, Token::Day, Token::Hours, Token::Minutes, Token::Seconds, Token::Year};
const Parser::CurrentState::TimeParser::TokenOrder Parser::CurrentState::TimeParser::_rfc3339_order_1{Token::Year, Token::Month, Token::Day, Token::Hours, Token::Minutes, Token::Seconds};
const Parser::CurrentState::TimeParser::TokenOrder Parser::CurrentState::TimeParser::_rfc3339_order_2{Token::Year, Token::Month, Token::Day, Token::Hours, Token::Minutes, Token::Seconds, Token::ZoneHours, Token::ZoneMinutes};
